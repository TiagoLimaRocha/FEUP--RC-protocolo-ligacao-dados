#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "../types.h"
#include "../HashMap/HashMap.h"

int logger(LogType type, LogCode code);
void print_usage(const char *arg);
char *timestamp();
int write_to_file(char *filePath, char *msg);
