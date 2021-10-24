#include "LinkLayer.h"

volatile bool alarmTriggered;
volatile unsigned int transmissionAttempts = 0;

struct termios oldtio;

static LinkLayer linkLayer;
static LinkType linkType;
static llInfo linkLayerInfo;
static bool linkLayerInit = false;
static int afd;

// SIGNALS

void sig_alarm_handler(int sigNum)
{
  if (sigNum == SIGALRM)
  {
    ++transmissionAttempts;
    alarmTriggered = true;
  }
}

void set_alarm(unsigned int seconds)
{
  alarmTriggered = false;
  alarm(seconds);
}

void set_alarm_handler()
{
  signal(SIGALRM, sig_alarm_handler);
  alarmTriggered = false;
  transmissionAttempts = 0;
}

void reset_alarm_handler()
{
  alarm(0);
  signal(SIGALRM, NULL);
  alarmTriggered = false;
  transmissionAttempts = 0;
}

bool was_alarm_triggered()
{
  if (alarmTriggered)
  {
    logger(WARN, LL_WARN_SIG_CONNECTION_TIMED_OUT);
    return true;
  }
  return false;
}

void sig_int_handler(int sig)
{
  if (sig == SIGINT)
  {
    tcsetattr(afd, TCSANOW, &oldtio);
    exit(-1);
  }
}

// FUNCTIONAL

void ll_abort(int fd)
{
  close_serial_port(fd);
}

void ll_setup(int timeout, int maxRetries, int baudrate)
{
  linkLayer.timeout = timeout;
  linkLayer.transmissionsNo = maxRetries;
  linkLayer.baudrate = get_termios_baudrate(baudrate);
  linkLayerInit = true;
}

int ll_open(int port, LinkType type)
{
  signal(SIGINT, sig_int_handler);
  afd = open_serial_port(port, type);
  if (afd == -1)
    return APP_ERROR_GENERAL;

  if (type == TRANSMITTER)
    return ll_open_transmitter(afd);

  return ll_open_receiver(afd);
}

int ll_close(int fd)
{
  if (linkType == TRANSMITTER)
  {
    // Send Disc, receive DISC
    ByteBuffer discFrame;
    ll_build_control_frame(&discFrame, LL_DISC);
    if (ll_frame_exchange(fd, &discFrame, LL_DISC) == -1)
    {
      logger(ERROR, LL_ERROR_CLOSE_DISCONECT);
      free_byte_buffer(&discFrame);
      close_serial_port(fd);
      return APP_ERROR_GENERAL;
    }

    free_byte_buffer(&discFrame);

    // Send UA
    ll_send_control_frame(fd, LL_UA);
    usleep(50);
    logger(INFO, LL_INFO_CLOSE_DISCONECT);

    close_serial_port(fd);
    return 1;
  }

  if (linkType == RECEIVER)
  {
    while (true)
    {
      /* Wait for DISC */
      ByteBuffer replyFrame;
      int res = ll_read_frame(fd, &replyFrame);
      bool isDisc = ll_is_frame_control_type(&replyFrame, LL_DISC);
      free_byte_buffer(&replyFrame);

      if (!isDisc)
        logger(WARN, LL_WARN_CLOSE_FRAME_IGNORED);

      // If invalid frame or not a DISC command, retry
      if (res != -1 && isDisc)
        break;
    }

    /* Send DISC, receive UA */
    ByteBuffer discFrame;
    ll_build_control_frame(&discFrame, LL_DISC);

    int res = ll_frame_exchange(fd, &discFrame, LL_UA);
    if (res == -1)
      logger(ERROR, LL_ERROR_CLOSE_DISCONECT);

    logger(INFO, LL_INFO_CLOSE_DISCONECT);
    close_serial_port(fd);
    return res;
  }

  close_serial_port(fd);
  return APP_ERROR_GENERAL;
}

int ll_write(int fd, char *buffer, int length)
{
  ByteBuffer frame;
  ll_build_data_frame(&frame, buffer, length);
  if (ll_frame_exchange(fd, &frame, LL_RR) == -1)
  {
    logger(ERROR, LL_ERROR_WRITE_FAIL);
    free_byte_buffer(&frame);
    return APP_ERROR_GENERAL;
  }

  int size = frame.size;
  linkLayer.sequenceNo ^= 1;
  free_byte_buffer(&frame);
  return size;
}

int ll_read(int fd, char **buffer)
{
  ByteBuffer frame;
  while (true)
  {
    if (ll_read_frame(fd, &frame) == -1)
    {
      free_byte_buffer(&frame);
      continue;
    }

    // Ignore
    if (!ll_is_frame_control_type(&frame, LL_INF))
    {
      if (ll_is_frame_control_type(&frame, LL_SET))
      {
        logger(WARN, LL_WARN_READ_FRAME_IGNORED_SET);
        ll_send_control_frame(fd, LL_UA);
      }
      else
        logger(WARN, LL_WARN_READ_FRAME_IGNORED_CONTROL);

      free_byte_buffer(&frame);
      continue;
    }

    // Check seq number for duplicate frames
    if ((frame.buffer[C_FIELD] >> 6) == (uchar_t)linkLayer.sequenceNo)
    {
      logger(WARN, LL_WARN_READ_FRAME_IGNORED_DUPLICATE);
      ll_send_control_frame(fd, LL_RR);
      continue;
    }

    ByteBuffer packet;
    create_byte_buffer(&packet, INF_FRAME_SIZE);

    // Get the packet BBC2 value, check for ESC_MOD
    uchar_t bcc2 = 0x00; // Calculated BCC2
    uchar_t packetBcc2 = frame.buffer[frame.size - 2];
    unsigned int dataLimit = frame.size - 2;

    // Adjust for BCC2 escape flag
    if (frame.buffer[frame.size - 3] == LL_ESC)
    {
      packetBcc2 ^= LL_ESC_MOD;
      --dataLimit;
#ifdef DEBUG_PRINT_INFORMATION
      printf("BCC2 after byte destuffing: %x\n", packetBcc2);
#endif
    }
    // Destuff bytes and calculate BCC2
    for (unsigned int i = DATA_FIELD; i < dataLimit; ++i)
    {
      uchar_t temp;
      if (frame.buffer[i] == LL_ESC)
        temp = (uchar_t)(frame.buffer[++i]) ^ LL_ESC_MOD;
      else
        temp = (uchar_t)frame.buffer[i];

      bcc2 ^= temp;
      bb_push(&packet, temp);
    }

    // BCC2 check
    if (bcc2 != packetBcc2)
    {
      printf("ll: frame rejected - failed BBC2 check, expected: %x, received %x\n", bcc2, packetBcc2);
      ll_send_control_frame(fd, LL_REJ);
      free_byte_buffer(&packet);
      free_byte_buffer(&frame);
      continue;
    }

#ifdef FER
    unsigned int randomN = rand() % 100;
    if (randomN < FERPROB)
    {
      send_control_frame(fd, LL_REJ);
      free_byte_buffer(&packet);
      free_byte_buffer(&frame);
      continue;
    }
#endif
    // Frame read successfuly, flip seq number and reply with RR
    linkLayer.sequenceNo ^= 1;
    ll_send_control_frame(fd, LL_RR);
    free_byte_buffer(&frame);
    *buffer = (char *)packet.buffer;
    return packet.size;
  }
  return APP_ERROR_GENERAL;
}

llInfo ll_info()
{
  return linkLayerInfo;
}

// HELPERS

int ll_open_transmitter(int fd)
{
  linkLayer.sequenceNo = 1;

  ByteBuffer setFrame;
  ll_build_control_frame(&setFrame, LL_SET);

  if (ll_frame_exchange(fd, &setFrame, LL_UA) == -1)
  {
    free_byte_buffer(&setFrame);
    return APP_ERROR_GENERAL;
  }

  free_byte_buffer(&setFrame);
  logger(INFO, LL_INFO_OPEN_CONNECT);
  return fd;
}

int ll_open_receiver(int fd)
{
  linkLayer.sequenceNo = 0;
  while (true)
  {
    logger(INFO, LL_INFO_OPEN_RECEIVER_WAITING);

    llControlType type = LL_DISC;
    while (type != LL_SET)
    {
      ByteBuffer setFrame;
      if (ll_read_frame(fd, &setFrame) == -1)
        continue;

      type = setFrame.buffer[C_FIELD];

      if (type != LL_SET)
        logger(WARN, LL_WARN_OPEN_RECEIVER_FRAME_IGNORED);

      free_byte_buffer(&setFrame);
    }
    if (ll_send_control_frame(fd, LL_UA) == -1)
    {
      logger(ERROR, LL_ERROR_OPEN_RECEIVER_UA_PACKET_SEND);
      return APP_ERROR_GENERAL;
    }

    logger(INFO, LL_INFO_OPEN_CONNECT);
    return fd;
  }
}

// FRAMES

int ll_frame_exchange(int fd, ByteBuffer *frame, llControlType reply)
{
  set_alarm_handler();
  while (transmissionAttempts < linkLayer.transmissionsNo)
  {
    ll_send_frame(fd, frame);
    set_alarm(linkLayer.timeout);

    while (true)
    {
      ByteBuffer replyFrame;
      int res = ll_read_frame(fd, &replyFrame);

      // Timeout or invalid frame received
      if (res == -1)
      {
        logger(WARN, LL_WARN_FRAME_EXCHANGE_TIMEOUT_OR_INVALID_FRAME);
        tcflush(fd, TCIOFLUSH);
        free_byte_buffer(&replyFrame);
        break;
      }

      bool isRej = ll_is_frame_control_type(&replyFrame, LL_REJ);
      bool isRr = ll_is_frame_control_type(&replyFrame, LL_RR);

      // Verify if RR is a duplicate
      if (isRr)
      {
        uchar_t replySeq = ((uchar_t)(replyFrame.buffer[C_FIELD]) >> 7);
        if (replySeq != (uchar_t)(linkLayer.sequenceNo))
        {
          ++linkLayerInfo.framesLost;
          logger(WARN, LL_WARN_FRAME_EXCHANGE_FRAME_IGNORED_DUPLICATE);
          free_byte_buffer(&replyFrame);
          continue;
        }
      }

      // Received REJ, resend frame if not duplicate
      if (isRej)
      {
        uchar_t replySeq = ((uchar_t)(replyFrame.buffer[C_FIELD]) >> 7);

        if (replySeq != (uchar_t)(linkLayer.sequenceNo ^ 1))
        {
          ++linkLayerInfo.framesLost;
          logger(WARN, LL_WARN_FRAME_EXCHANGE_FRAME_IGNORED_DUPLICATE);
          free_byte_buffer(&replyFrame);
          continue;
        }

        free_byte_buffer(&replyFrame);
        logger(WARN, LL_WARN_FRAME_EXCHANGE_FRAME_REJECTED);

        ++linkLayerInfo.framesLost;
        ++transmissionAttempts;
        break;
      }

      // Exchange was sucessful
      if (ll_is_frame_control_type(&replyFrame, reply))
      {
        free_byte_buffer(&replyFrame);
        reset_alarm_handler();
        return 1;
      }
      else
      {
        ++linkLayerInfo.framesLost;
        logger(WARN, LL_WARN_FRAME_EXCHANGE_FRAME_IGNORED_CONTROL);
      }
    }
  }

  reset_alarm_handler();
  logger(ERROR, LL_ERROR_FRAME_EXCHANGE_ATTEMPTS_EXCEEDED);

  return APP_ERROR_GENERAL;
}

int ll_send_control_frame(int fd, llControlType type)
{
  ByteBuffer frame;
  ll_build_control_frame(&frame, type);
  int res = ll_send_frame(fd, &frame);
  free_byte_buffer(&frame);
  return res;
}

int ll_send_frame(int fd, ByteBuffer *frame)
{
  if (write(fd, frame->buffer, frame->size) < 0)
  {
    logger(ERROR, LL_ERROR_SEND_FRAME_WRITE_TO_PORT);
    return APP_ERROR_GENERAL;
  }
  ll_log_frame(frame, "sent");
  return 0;
}

int ll_read_frame(int fd, ByteBuffer *frame)
{
  if (frame == NULL)
    return APP_ERROR_GENERAL;

  create_byte_buffer(frame, CONTROL_FRAME_SIZE);

  char incByte = 0x00;
  int readStatus = 0;

  // Clear buffer and wait for a flag
  while (readStatus <= 0 || (uchar_t)incByte != LL_FLAG)
  {
    if (was_alarm_triggered())
      return APP_ERROR_GENERAL;

    readStatus = read(fd, &incByte, 1);
  }
  bb_push(frame, (uchar_t)incByte);

  // Reset vars
  readStatus = 0;
  incByte = 0x00;

  // Read serial until flag is found
  while (incByte != LL_FLAG)
  {
    if (was_alarm_triggered())
      return APP_ERROR_GENERAL;

    readStatus = read(fd, &incByte, 1);
    if (readStatus <= 0)
      continue;

    bb_push(frame, incByte);
  }

  ++linkLayerInfo.framesTotal;

#ifdef LL_LOG_BUFFER
  bb_print_hex(frame);
#endif

  if (ll_validate_control_frame(frame) < 0)
  {
    logger(ERROR, LL_ERROR_READ_FRAME_IGNORED);
    ++linkLayerInfo.framesLost;
    return APP_ERROR_GENERAL;
  }

  return EXIT_SUCCESS;
}

int ll_validate_control_frame(ByteBuffer *frame)
{
  if (frame == NULL || frame->buffer == NULL)
  {
    logger(ERROR, LL_ERROR_VALIDATE_CONTROL_FRAME_NULL);
    return APP_ERROR_GENERAL;
  }

  if (frame->size < CONTROL_FRAME_SIZE)
  {
    bb_print_hex(frame);
    logger(ERROR, LL_ERROR_FRAME_TOO_SMALL);
    return LL_ERROR_FRAME_TOO_SMALL;
  }

#ifdef LL_LOG_FRAMES
  ll_log_frame(frame, "received");
#endif

  // Start Flag
  if (frame->buffer[FD_FIELD] != LL_FLAG)
  {
    logger(ERROR, LL_ERROR_BAD_START_FLAG);
    return LL_ERROR_BAD_START_FLAG;
  }

  // Check address
  char expectedAF = ll_get_address_field(linkType ^ 1, frame->buffer[C_FIELD]);
  if (frame->buffer[AF_FIELD] != expectedAF)
  {
    logger(ERROR, LL_ERROR_BAD_ADDRESS);
    return LL_ERROR_BAD_ADDRESS;
  }

  // Check BCC1
  if (frame->buffer[BCC1_FIELD] != (uchar_t)(expectedAF ^ frame->buffer[C_FIELD]))
  {
    logger(ERROR, LL_ERROR_BAD_BCC1);
    return LL_ERROR_BAD_BCC1;
  }

  // Last element flag
  if (frame->buffer[frame->size - 1] != (uchar_t)LL_FLAG)
    return LL_ERROR_BAD_END_FLAG;

  return EXIT_SUCCESS;
}

void ll_build_control_frame(ByteBuffer *frame, llControlType type)
{
  create_byte_buffer(frame, CONTROL_FRAME_SIZE);
  bb_push(frame, LL_FLAG);                              // FLAG
  bb_push(frame, ll_get_address_field(linkType, type)); // ADDRESS

  if (type == LL_RR || type == LL_REJ)
    type |= linkLayer.sequenceNo << 7; // N(r)

  bb_push(frame, type);                                // Control type
  bb_push(frame, frame->buffer[1] ^ frame->buffer[2]); // BCC1
  bb_push(frame, LL_FLAG);                             // FLAG
}

void ll_build_data_frame(ByteBuffer *frame, char *buffer, int length)
{
  create_byte_buffer(frame, length + INF_FRAME_SIZE);
  bb_push(frame, LL_FLAG);                                // FLAG
  bb_push(frame, ll_get_address_field(linkType, LL_INF)); // ADDRESS
  bb_push(frame, LL_INF | (linkLayer.sequenceNo << 6));   // Control and N(s)
  bb_push(frame, frame->buffer[1] ^ frame->buffer[2]);    // BCC1

  // Add buffer to frame and calculate bcc2
  uchar_t bcc2 = 0x00;
  for (int i = 0; i < length; ++i)
  {
    // BCC2
    bcc2 ^= (uchar_t)buffer[i];

    // Byte stuffing
    if (buffer[i] == (uchar_t)LL_FLAG || buffer[i] == (uchar_t)LL_ESC)
    {
      bb_push(frame, (uchar_t)LL_ESC);
      bb_push(frame, buffer[i] ^ (uchar_t)LL_ESC_MOD);
    }
    else
      bb_push(frame, buffer[i]);
  }

  // Bytestuffin on BBC2 when needed
  if (bcc2 == (uchar_t)LL_ESC || bcc2 == (uchar_t)LL_FLAG)
  {
    bb_push(frame, (uchar_t)LL_ESC);
    bb_push(frame, (uchar_t)(bcc2 ^ (uchar_t)LL_ESC_MOD));
  }
  else
    bb_push(frame, bcc2);

  bb_push(frame, (uchar_t)LL_FLAG);

#ifdef LL_LOG_BUFFER
  bb_print_hex(frame);
#endif
}

char ll_get_address_field(LinkType lnk, llControlType type)
{
  type &= 0x0F;
  if (lnk == RECEIVER && ll_is_control_command(type))
    return LL_AF2;

  if (lnk == TRANSMITTER && !ll_is_control_command(type))
    return LL_AF2;

  return LL_AF1;
}

bool ll_is_control_command(llControlType type)
{
  type &= 0x0F;
  if (type == LL_INF || type == LL_DISC || type == LL_SET)
    return true;

  return false;
}

bool ll_is_frame_control_type(ByteBuffer *frame, llControlType type)
{
  if (frame == NULL || frame->buffer == NULL)
    return false;

  type &= 0x0F;
  llControlType frameType = frame->buffer[C_FIELD] & 0x0F;

  return frameType == type;
}

const char *ll_get_control_type_str(llControlType type)
{
  type &= 0x0F;

  HashTable *controlTypesMap = create_table(HT_CAPACITY);

  ht_insert(controlTypesMap, my_itoa(LL_INF, 2), "INF");
  ht_insert(controlTypesMap, my_itoa(LL_SET, 2), "SET");
  ht_insert(controlTypesMap, my_itoa(LL_DISC, 2), "DISC");
  ht_insert(controlTypesMap, my_itoa(LL_UA, 2), "UA");
  ht_insert(controlTypesMap, my_itoa(LL_RR, 2), "RR");
  ht_insert(controlTypesMap, my_itoa(LL_REJ, 2), "REJ");

  char *key = my_itoa(type, 16);
  int controlTypeStr = ht_search(controlTypesMap, key);

  free_hashtable(controlTypesMap);

  if (controlTypeStr == NULL)
    return "UNK";

  return controlTypeStr;
}

void ll_log_frame(ByteBuffer *frame, const char *type)
{
#ifdef LL_LOG_FRAMES
  printf("ll: %s packet %s", type, ll_get_control_type_str(frame->buffer[C_FIELD]));

  // Header
  printf("\t\t\t[F %x][A %x][C %x][BCC1 %x]",
         frame->buffer[FD_FIELD],
         frame->buffer[AF_FIELD], (uchar_t)frame->buffer[C_FIELD],
         (uchar_t)frame->buffer[BCC1_FIELD]);
  // BCC2
  if ((frame->buffer[C_FIELD] & 0x0F) == LL_INF)
    printf("[BCC2 %x]", (uchar_t)frame->buffer[frame->size - 2]);

  // Tail
  printf("[F %x] Frame size: %d bytes\n", frame->buffer[frame->size - 1], frame->size);
#endif
}

// SERIAL PORT

int open_serial_port(int port, LinkType type)
{
  snprintf(linkLayer.port, MAX_PORT_LENGTH, "%s%d", PORT_NAME, port);
  if (!linkLayerInit)
    ll_setup(TIMEOUT_DURATION, MAX_TRANSMISSION_ATTEMPS, DEFAULT_BAUDRATE);

  linkType = type;

  int fd = open(linkLayer.port, O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(linkLayer.port);
    return APP_ERROR_GENERAL;
  }

  struct termios newtio;

  if (tcgetattr(fd, &oldtio) == -1)
  {
    // save current port settings
    perror("tcgetattr");
    close(fd);
    return APP_ERROR_GENERAL;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = linkLayer.baudrate | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  // set input mode (non-canonical, no echo,...)
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 20; // inter-character timer unused
  newtio.c_cc[VMIN] = 0;   // blocking read until 5 chars received

  // VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  // leitura do(s) proximo(s) caracter(es)
  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    close(fd);
    return APP_ERROR_GENERAL;
  }

  return fd;
}

void close_serial_port(int fd)
{
  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
}