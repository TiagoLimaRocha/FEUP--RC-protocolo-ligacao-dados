#include "utils.h"

char *remove_filename_ext(char *myStr)
{
  char *retStr;
  char *lastExt;

  if (myStr == NULL)
    return NULL;

  if ((retStr = malloc(strlen(myStr) + 1)) == NULL)
    return NULL;

  strcpy(retStr, myStr);
  lastExt = strrchr(retStr, '.');

  if (lastExt != NULL)
    *lastExt = '\0';

  return retStr;
}

const char *get_filename_ext(const char *filename)
{
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

char *my_itoa(const int value, const int bufferSize)
{
  char *str = malloc(sizeof(char) * bufferSize);
  sprintf(str, "%d", value);
  return str;
}

int write_to_file(char *filePath, char *msg)
{
  FILE *f = fopen(filePath, "a");

  if (f == NULL)
  {
    perror("Error in write_to_file");
    exit(EXIT_FAILURE);
  }

  fprintf(f, "%s\n", msg);
  fclose(f);

  return EXIT_SUCCESS;
}

void print_progress(int done, int total)
{
  float percent = ((float)done / (float)total) * 100.0f;
  int blocks = percent / 10;

  printf("\rProgress ");
  for (int i = 0; i < blocks; ++i)
  {
    printf("#");
  }
  printf("[%.2f]", percent);
  fflush(stdout);
}

void print_usage(const char *arg)
{
  printf("Usage:\n");
  printf("%s <file_name> <T|R> <port_number> [options]\n", arg);
  printf("\nT - Transmitter, R - Receiver\n");
  printf("\nOptions:\n");
  printf("  -timeout=<seconds> \t\tSeconds until a frame is timed out\n");
  printf("  -baudrate=<rate> \t\tSerial port rate\n");
  printf(
      "  -max_retries=<retries> \tTimes a frame transmission can be "
      "retried\n");
  printf("  -fragSize=<size> \t\tMax size for data fragments\n");
  printf("\nExample: '%s static/pinguim.gif T 10'\n", arg);
}

int send_raw_data(int fd, char *buffer, int length)
{
  return write(fd, buffer, length);
}

float clock_seconds_since(struct timeval *startTimer)
{
  struct timeval endTimer;
  gettimeofday(&endTimer, NULL);
  float elapsed = (endTimer.tv_sec - startTimer->tv_sec);
  elapsed += (endTimer.tv_usec - startTimer->tv_usec) / 1000000.0f;
  return elapsed;
}

char *timestamp()
{
  time_t currentTime;
  char *cTimeString;

  /* Obtain current time. */
  currentTime = time(NULL);

  if (currentTime == ((time_t)-1))
  {
    (void)fprintf(stderr, "Failure to obtain the current time.\n");
    exit(EXIT_FAILURE);
  }

  /* Convert to local time format. */
  cTimeString = ctime(&currentTime);

  if (cTimeString == NULL)
  {
    (void)fprintf(stderr, "Failure to convert the current time.\n");
    exit(EXIT_FAILURE);
  }

  return cTimeString;
}

int logger(LogType type, LogCode code)
{

  char log[BUFFER], typeStr[32], codeStr[32], pathToTracefile[BUFFER];

  HashTable *logTypes = create_table(HT_CAPACITY);
  HashTable *logMessages = create_table(HT_CAPACITY);

  ht_insert(logTypes, "0", "INFO");
  ht_insert(logTypes, "1", "WARN");
  ht_insert(logTypes, "2", "ERROR");

  ht_insert(logMessages, "-1", "app.general.error");
  ht_insert(logMessages, "1", "app.usage.error");
  ht_insert(logMessages, "2", "bb.create.error");
  ht_insert(logMessages, "3", "bb.push.error");
  ht_insert(logMessages, "4", "bb.remove.error");
  ht_insert(logMessages, "5", "bb.print.empty.warn");
  ht_insert(logMessages, "6", "bb.free.empty.warn");
  ht_insert(logMessages, "7", "ll.close.disconect.info");
  ht_insert(logMessages, "-8", "ll.close.communicate_disconect.error");
  ht_insert(logMessages, "9", "ll.close.frame_ignored.warn");
  ht_insert(logMessages, "-10", "ll.write.error");
  ht_insert(logMessages, "11", "ll.read.frame_ignored.unexpected_SET_control.warn");
  ht_insert(logMessages, "12", "ll.read.frame_ignored.unexpected_control_field.warn");
  ht_insert(logMessages, "13", "ll.read.frame_ignored.duplicate_frame.warn");
  ht_insert(logMessages, "14", "ll.open.connect.info");
  ht_insert(logMessages, "15", "ll.open_receiver.waiting_for_connection.info");
  ht_insert(logMessages, "16", "ll.open_receiver.frame_ignored.unexpected_control_field.warn");
  ht_insert(logMessages, "-17", "ll.open_receiver.unable_to_send_UA_packet.error");
  ht_insert(logMessages, "18", "ll.frame_exchange.timeout_or_invalid_frame.warn");
  ht_insert(logMessages, "19", "ll.frame_exchange.frame_ignored.duplicate_frame.warn");
  ht_insert(logMessages, "20", "ll.frame_exchange.frame_rejected.warn");
  ht_insert(logMessages, "21", "ll.frame_exchange.frame_ignored.unexpected_control_field.warn");
  ht_insert(logMessages, "-22", "ll.frame_exchange.connection_failed.transmission_attempts_exceeded.error");
  ht_insert(logMessages, "-23", "ll.send_frame.write_frame_to_port.error");
  ht_insert(logMessages, "-24", "ll.read_frame.frame_ignored.header_validation.error");
  ht_insert(logMessages, "-25", "ll.validate_control_frame.null_frame_or_frame_buffer.error");
  ht_insert(logMessages, "-2", "ll.validate_control_frame.frame_too_small.error");
  ht_insert(logMessages, "-3", "ll.validate_control_frame.bad_start_flag.error");
  ht_insert(logMessages, "-4", "ll.validate_control_frame.bad_address.error");
  ht_insert(logMessages, "-5", "ll.validate_control_frame.bad_bcc1.error");
  ht_insert(logMessages, "-6", "ll.validate_control_frame.bad_end_flag.error");
  ht_insert(logMessages, "-7", "ll.sig.connection_timed_out.warn");
  ht_insert(logMessages, "-26", "al.send_file.filename_length_exceeded.error");
  ht_insert(logMessages, "-27", "al.send_file.open_file.error");
  ht_insert(logMessages, "-28", "al.send_file.connection.error");
  ht_insert(logMessages, "-29", "al.send_file.transmission.error");
  ht_insert(logMessages, "30", "al.send_file.file_transmission_done.info");
  ht_insert(logMessages, "31", "al.receive_file.waiting_for_connection.info");
  ht_insert(logMessages, "32", "al.receive_file.ignoring_duplicate_data_packet.warn");
  ht_insert(logMessages, "33", "al.receive_file.file_transmission_over.info");
  ht_insert(logMessages, "-34", "al.receive_file.connection.error");
  ht_insert(logMessages, "-35", "al.receive_file.write_selected_file.error");
  ht_insert(logMessages, "36", "al.receive_file.starting_file_transmission.info");
  ht_insert(logMessages, "-37", "al.read_data_packet.read_packet.error");
  ht_insert(logMessages, "-38", "al.read_data_packet.unexpected_control_packet.error");
  ht_insert(logMessages, "-39", "al.send_control_packet.send_packet.error");
  ht_insert(logMessages, "40", "al.send_control_packet.control_packet_sent.info");
  ht_insert(logMessages, "-41", "al.read_control_packet.failed_to_receive_packet.error");
  ht_insert(logMessages, "-42", "al.parse_control_packet.invalid_control_packet.error");
  ht_insert(logMessages, "43", "al.send_file.starting_file_transmission.info");

  // Cast to string (itoa)
  sprintf(typeStr, "%d", type);
  sprintf(codeStr, "%d", code);

  const char *logType = ht_search(logTypes, typeStr);
  const char *logMessage = ht_search(logMessages, codeStr);
  const char *currTimestamp = timestamp();

  if (getcwd(pathToTracefile, sizeof(pathToTracefile)) == NULL)
  {
    perror("getcwd() error");
    return 1;
  }

  printf("logger: (%s, {type: %s, timestamp: %s})\n\n", logType, logMessage, currTimestamp);
  sprintf(log, "%s - %s - %s", currTimestamp, logType, logMessage);
  strcat(pathToTracefile, "/src/logs/tracefile.txt");
  write_to_file(pathToTracefile, log);

  free_hashtable(logTypes);
  free_hashtable(logMessages);

  return EXIT_SUCCESS;
}

int get_termios_baudrate(int baudrate)
{
  if (baudrate < 0)
    return DEFAULT_BAUDRATE;

  HashTable *baudratesMap = create_table(HT_CAPACITY);

  ht_insert(baudratesMap, "0", my_itoa(B0, 2));
  ht_insert(baudratesMap, "50", my_itoa(B50, 2));
  ht_insert(baudratesMap, "75", my_itoa(B75, 2));
  ht_insert(baudratesMap, "110", my_itoa(B110, 3));
  ht_insert(baudratesMap, "134", my_itoa(B134, 3));
  ht_insert(baudratesMap, "150", my_itoa(B150, 3));
  ht_insert(baudratesMap, "200", my_itoa(B200, 3));
  ht_insert(baudratesMap, "300", my_itoa(B300, 3));
  ht_insert(baudratesMap, "600", my_itoa(B600, 3));
  ht_insert(baudratesMap, "1200", my_itoa(B1200, 4));
  ht_insert(baudratesMap, "1800", my_itoa(B1800, 4));
  ht_insert(baudratesMap, "2400", my_itoa(B2400, 4));
  ht_insert(baudratesMap, "4800", my_itoa(B4800, 4));
  ht_insert(baudratesMap, "9600", my_itoa(B9600, 4));
  ht_insert(baudratesMap, "19200", my_itoa(B19200, 5));
  ht_insert(baudratesMap, "38400", my_itoa(B38400, 5));
  ht_insert(baudratesMap, "57600", my_itoa(B57600, 5));
  ht_insert(baudratesMap, "115200", my_itoa(B115200, 6));
  ht_insert(baudratesMap, "230400", my_itoa(B230400, 6));
  ht_insert(baudratesMap, "460800", my_itoa(B460800, 6));

  char *key = my_itoa(baudrate, 16);
  int termiosBaudrate = atoi(ht_search(baudratesMap, key));
  free_hashtable(baudratesMap);

  return termiosBaudrate;
}