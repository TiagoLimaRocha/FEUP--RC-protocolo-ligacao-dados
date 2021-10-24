#include "ApplicationLayer.h"

ControlPacket fileCP; // Control Packet with file information
static alInfo applicationLayerInfo;
static int alFragSize = MAX_FRAGMENT_SIZE;

void al_update_info(struct timeval *startTimer)
{
  llInfo linkLayerInfo = ll_info();

  applicationLayerInfo.fileSize = fileCP.size;
  applicationLayerInfo.transmissionDurationSecs = clock_seconds_since(startTimer);
  applicationLayerInfo.framesTotal = linkLayerInfo.framesTotal;
  applicationLayerInfo.framesLost = linkLayerInfo.framesLost;
  applicationLayerInfo.avgBitsPerSecond =
      (float)(applicationLayerInfo.fileSize * 8) / applicationLayerInfo.transmissionDurationSecs;
}

alInfo al_info()
{
  return applicationLayerInfo;
}

void al_print_info()
{
  float floss = (float)applicationLayerInfo.framesLost / (float)applicationLayerInfo.framesTotal;
  float eff =
      (float)applicationLayerInfo.avgBitsPerSecond / (float)applicationLayerInfo.baudrate * 100.0f;

  printf("Info:\n");
  printf(
      " baudrate %d bits/s \taverage bitrate %d bits/s \tEfficiency %.2f%% \n",
      applicationLayerInfo.baudrate,
      applicationLayerInfo.avgBitsPerSecond, eff);

  printf(" file size %d bytes \tmax fragment size %d bytes \tpackets sent %d\n",
         applicationLayerInfo.fileSize,
         alFragSize,
         applicationLayerInfo.dataPacketCount);

  printf(" transmission time %.2f seconds\n", applicationLayerInfo.transmissionDurationSecs);

  printf(" total Frames %d \tlost frames %d \tframe loss %.2f\n",
         applicationLayerInfo.framesTotal, applicationLayerInfo.framesLost, floss);
}

void al_setup(int timeout, int baudrate, int maxRetries, int fragSize)
{
  applicationLayerInfo.timeout = timeout;
  applicationLayerInfo.retries = maxRetries;
  applicationLayerInfo.dataPacketCount = 0;

  if (baudrate > MAX_BAUDRATE)
    baudrate = MAX_BAUDRATE;

  applicationLayerInfo.baudrate = baudrate;
  alFragSize = fragSize;

  if (alFragSize > MAX_FRAGMENT_SIZE)
    alFragSize = MAX_FRAGMENT_SIZE;

  ll_setup(timeout, maxRetries, baudrate);
}

int al_send_file(const char *filename, int port)
{
  int nameLength = strlen(filename);
  if (nameLength > MAX_FILE_NAME)
  {
    logger(ERROR, AL_ERROR_SEND_FILE_NAME_LENGTH);
    return APP_ERROR_GENERAL;
  }
  fileCP.nameLength = nameLength;

  // Open File Stream
  FILE *fptr = fopen(filename, "r");
  if (fptr == NULL)
  {
    logger(ERROR, AL_ERROR_SEND_FILE_OPEN);
    return APP_ERROR_GENERAL;
  }

  // Establish LL Connection
  int fd = ll_open(port, TRANSMITTER);
  if (fd == -1)
  {
    logger(ERROR, AL_ERROR_SEND_FILE_CONNECTION);
    return APP_ERROR_GENERAL;
  }

  // Get File Information
  al_get_file_info(filename, fptr);

  // Send start control packet
  if (send_control_packet(fd, CONTROL_START) == -1)
    return APP_ERROR_GENERAL;

  struct timeval startTimer;
  gettimeofday(&startTimer, NULL);
  printf("al: starting file transmission\n");

  // Send data packets until the file is read
  DataPacket packet;
  packet.data = (char *)malloc(alFragSize + DATA_HEADER_SIZE);
  packet.sequenceNo = 0;
  packet.size = 1;

  unsigned int bytesTransferred = 0;
  while (true)
  {
    packet.size =
        fread(&packet.data[L1_FIELD + 1], sizeof(uchar_t), alFragSize, fptr);

    if (packet.size <= 0)
      break;

    ++applicationLayerInfo.dataPacketCount;
    ++packet.sequenceNo;
    packet.sequenceNo %= 256;

    if (send_data_packet(fd, &packet) == -1)
    {
      logger(ERROR, AL_ERROR_SEND_FILE_TRANSMISSION);
      ll_abort(fd);
      return APP_ERROR_GENERAL;
    }
    // Progress
    bytesTransferred += packet.size;
    print_progress(bytesTransferred, fileCP.size);
  }
  printf("\n");
  free(packet.data);

  // Send end control packet
  if (send_control_packet(fd, CONTROL_END) == -1)
    return APP_ERROR_GENERAL;

  logger(INFO, AL_INFO_SEND_FILE_TRANSMISSION_DONE);
  al_update_info(&startTimer);

  // Close connection and cleanup
  ll_close(fd);
  free(fileCP.name);
  fclose(fptr);

  return EXIT_SUCCESS;
}

int al_receive_file(const char *filename, int port)
{
  logger(INFO, AL_INFO_RECEIVE_FILE_WAITING_CONNECTION);

  static int8_t seqNumber = 1;

  // Establish LL Connection
  int fd = ll_open(port, RECEIVER);
  if (fd == -1)
  {
    logger(ERROR, AL_ERROR_RECEIVE_FILE_CONNECTION);
    return APP_ERROR_GENERAL;
  }

  FILE *fptr = fopen(filename, "w");
  if (fptr == NULL)
  {
    logger(ERROR, AL_ERROR_RECEIVE_FILE_WRITE);
    return APP_ERROR_GENERAL;
  }

  fileCP.type = CONTROL_DATA;
  while (fileCP.type != CONTROL_START)
  {
    read_control_packet(fd, &fileCP);
  }

  struct timeval startTimer;
  gettimeofday(&startTimer, NULL);
  logger(INFO, AL_INFO_RECEIVE_FILE_STARTING_TRANSMISSION);

  unsigned int bytesTransferred = 0;
  DataPacket dataPacket;
  while (bytesTransferred < fileCP.size)
  {
    char *buffer = NULL;
    if (read_data_packet(fd, &dataPacket, buffer) == -1)
      return APP_ERROR_GENERAL;

    if (dataPacket.sequenceNo != seqNumber)
    {
      logger(WARN, AL_WARN_RECEIVE_FILE_DATA_PACKET_IGNORE);
      free(buffer);
      continue;
    }
    ++seqNumber;
    seqNumber %= 256;

    fwrite(dataPacket.data, sizeof(char), dataPacket.size, fptr);

    bytesTransferred += dataPacket.size;
    free(buffer);
    print_progress(bytesTransferred, fileCP.size);
  }
  printf("\n");

  // Wait for end control packet
  ControlPacket controlPacket;
  controlPacket.type = CONTROL_DATA;
  while (controlPacket.type != CONTROL_END)
  {
    read_control_packet(fd, &controlPacket);
  }

  logger(INFO, AL_INFO_RECEIVE_FILE_TRANSMISSION_OVER);
  al_update_info(&startTimer);

  // Close connection and cleanup
  free(fileCP.name);
  ll_close(fd);
  fclose(fptr);

  return EXIT_SUCCESS;
}

int read_data_packet(int fd, DataPacket *packet, char *buffer)
{
  int res = ll_read(fd, &buffer);
  if (res < 0)
  {
    logger(ERROR, AL_ERROR_READ_DATA_PACKET);
    return APP_ERROR_GENERAL;
  }

  if (packet == NULL || buffer[CP_CFIELD] != CONTROL_DATA)
  {
    logger(ERROR, AL_ERROR_READ_DATA_PACKET_CONTROL_PACKET);
    return APP_ERROR_GENERAL;
  }

  packet->sequenceNo = buffer[SEQ_FIELD];
  packet->size = (uchar_t)buffer[L2_FIELD] * 256;
  packet->size += (uchar_t)buffer[L1_FIELD];
  packet->data = &buffer[L1_FIELD + 1];

  return EXIT_SUCCESS;
}

int send_data_packet(int fd, DataPacket *packet)
{
  packet->data[CP_CFIELD] = CONTROL_DATA;
  packet->data[SEQ_FIELD] = (uchar_t)packet->sequenceNo;
  packet->data[L2_FIELD] = (uchar_t)(packet->size / 256);
  packet->data[L1_FIELD] = (uchar_t)(packet->size % 256);

  int res = ll_write(fd, (char *)packet->data, packet->size + DATA_HEADER_SIZE);

  if (res >= packet->size)
    return EXIT_SUCCESS;

  return APP_ERROR_GENERAL;
}

int send_control_packet(int fd, alControlType type)
{
  ByteBuffer buffer;
  build_control_packet(type, &buffer);
  print_control_packet(&fileCP);

  if (ll_write(fd, (char *)buffer.buffer, buffer.size) == -1)
  {
    logger(ERROR, AL_ERROR_SEND_CONTROL_PACKET);
    return APP_ERROR_GENERAL;
  }

  logger(INFO, AL_INFO_SEND_CONTROL_PACKET);
  free_byte_buffer(&buffer);

  return EXIT_SUCCESS;
}

int read_control_packet(int fd, ControlPacket *packet)
{
  char *buffer;
  int size = ll_read(fd, &buffer);

  if (size == -1)
  {
    logger(ERROR, AL_ERROR_READ_CONTROL_PACKET_RECEIVE_PACKET);
    free(buffer);
    return APP_ERROR_GENERAL;
  }
  parse_control_packet(buffer, size, packet);

#ifdef AL_PRINT_CPACKETS
  printf("al: received control Packet ");
  print_control_packet(packet);
#endif

  free(buffer);
  return EXIT_SUCCESS;
}

void build_control_packet(alControlType type, ByteBuffer *packet)
{
  /**
   * [C][T Size][L Size][  V Size  ][T Name][L Name][  V Name  ]
   * C - Control Field  T - Type  L - Length  V - Value
   *
   * Size: (C + 2*(T+L) = 5) + length of size + length of name
   */
  fileCP.type = type;
  int packetSize = 5 + fileCP.sizeLength + fileCP.nameLength;

  create_byte_buffer(packet, packetSize);
  bb_push(packet, (char)type);

  // SIZE TLV
  bb_push(packet, (char)TLV_SIZE_T);
  bb_push(packet, (char)fileCP.sizeLength);
  unsigned int size = fileCP.size;

  for (uint8_t i = 0; i < fileCP.sizeLength; ++i)
  {
    bb_push(packet, (char)size & 0x000000FF);
    size >>= 8;
  }

  // NAME TLV
  bb_push(packet, (char)TLV_NAME_T);
  bb_push(packet, (char)fileCP.nameLength);

  for (int i = 0; i < fileCP.nameLength; ++i)
    bb_push(packet, fileCP.name[i]);
}

void print_control_packet(ControlPacket *packet)
{
  if (packet == NULL)
    return;

  printf("[C %d][T 0][L %d][V %d][T 1][V %d][V %s]\n",
         packet->type,
         packet->sizeLength,
         packet->size,
         packet->nameLength,
         packet->name);
}

int parse_control_packet(char *packetBuffer, int size, ControlPacket *packet)
{
  if (packetBuffer == NULL)
    return -1;

  if (size < CP_MIN_SIZE)
    return -1;

  int index = 0;
  packet->type = packetBuffer[index++];

  if (packetBuffer[index++] != TLV_SIZE_T)
  {
    logger(ERROR, AL_ERROR_PARSE_CONTROL_PACKET_INVALID_PACKET);
    return APP_ERROR_GENERAL;
  }

  packet->sizeLength = (int8_t)packetBuffer[index++];

  packet->size = 0;
  for (int8_t i = 0; i < packet->sizeLength; ++i)
  {
    if (index > size - 1)
    {
      logger(ERROR, AL_ERROR_PARSE_CONTROL_PACKET_INVALID_PACKET);
      return APP_ERROR_GENERAL;
    }
    packet->size |= ((uchar_t)packetBuffer[index++]) << (8 * i);
  }

  if ((index > (size - 1)) || (packetBuffer[index++] != TLV_NAME_T))
  {
    logger(ERROR, AL_ERROR_PARSE_CONTROL_PACKET_INVALID_PACKET);
    return APP_ERROR_GENERAL;
  }

  if (index > (size - 1))
  {
    logger(ERROR, AL_ERROR_PARSE_CONTROL_PACKET_INVALID_PACKET);
    return APP_ERROR_GENERAL;
  }

  packet->nameLength = (int8_t)packetBuffer[index++];
  packet->name = (char *)malloc(packet->nameLength * sizeof(char) + 1);
  int namePos = index;

  for (int i = 0; index < (namePos + packet->nameLength); ++index)
  {
    if (index > size)
    {
      logger(ERROR, AL_ERROR_PARSE_CONTROL_PACKET_INVALID_PACKET);
      return APP_ERROR_GENERAL;
    }
    packet->name[i++] = packetBuffer[index];
  }

  packet->name[packet->nameLength] = 0x00; // Terminate string
  return EXIT_SUCCESS;
}

int al_get_file_info(const char *filename, FILE *fptr)
{
  if (fptr == NULL)
    return APP_ERROR_GENERAL;

  // Name
  fileCP.name = (char *)malloc(sizeof(char) * fileCP.nameLength + 1);
  strcpy(fileCP.name, filename);

  // Size
  fseek(fptr, 0L, SEEK_END);
  fileCP.size = ftell(fptr);
  rewind(fptr);

  // Bytes needed for length
  fileCP.sizeLength = 1;
  int size = fileCP.size;

  for (unsigned int i = 1; i < sizeof(int); ++i)
  {
    size >>= 8;
    if (size > 0)
      ++fileCP.sizeLength;
    else
      break;
  }

  return 0;
}