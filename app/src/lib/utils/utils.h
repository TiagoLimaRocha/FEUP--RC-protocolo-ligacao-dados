#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <termios.h>

#include "../types.h"
#include "../macros.h"
#include "../HashMap/HashMap.h"

char *my_itoa(const int value, const int bufferSize);
int logger(LogType type, LogCode code);
void print_usage(const char *arg);
char *timestamp();
int write_to_file(char *filePath, char *msg);
int get_termios_baudrate(int baudrate);
int send_raw_data(int fd, char *buffer, int length);