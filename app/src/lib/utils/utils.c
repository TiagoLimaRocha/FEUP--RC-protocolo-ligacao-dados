#include "utils.h"

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

  ht_insert(logMessages, "1", "app.usage.error");
  ht_insert(logMessages, "2", "bb.create.error");
  ht_insert(logMessages, "3", "bb.push.error");
  ht_insert(logMessages, "4", "bb.remove.error");
  ht_insert(logMessages, "5", "bb.print.empty.warn");
  ht_insert(logMessages, "6", "bb.free.empty.warn");
  ht_insert(logMessages, "0", "ll.ok.error");
  ht_insert(logMessages, "-1", "ll.general.error");
  ht_insert(logMessages, "-2", "ll.frame_too_small.error");
  ht_insert(logMessages, "-3", "ll.bad_start_flag.error");
  ht_insert(logMessages, "-4", "ll.bad_address.error");
  ht_insert(logMessages, "-5", "ll.bad_bcc1.error");
  ht_insert(logMessages, "-6", "ll.bad_end_flag.error");

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
  printf("  -frag_size=<size> \t\tMax size for data fragments\n");
  printf("\nExample: '%s pinguim.gif T 10'\n", arg);
}
