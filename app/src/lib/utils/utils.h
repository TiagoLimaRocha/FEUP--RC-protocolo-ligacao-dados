#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <termios.h>

#include "../types.h"
#include "../macros.h"
#include "../HashMap/HashMap.h"

char *my_itoa(const int value, const int bufferSize);
void print_usage(const char *arg);
void print_progress(int done, int total);
int write_to_file(char *filePath, char *msg);
int send_raw_data(int fd, char *buffer, int length);
float clock_seconds_since(struct timeval *startTimer);
char *timestamp();
int logger(LogType type, LogCode code);
int get_termios_baudrate(int baudrate);