#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "../types.h"

typedef struct ByteBuffer ByteBuffer;

struct ByteBuffer
{
  unsigned char *buffer;
  unsigned int size;
  unsigned int capacity;
};

int create_byte_buffer(ByteBuffer *bb, int initSize);
int bb_push(ByteBuffer *bb, char byte);
int bb_remove(ByteBuffer *bb, unsigned int pos);
void bb_print(ByteBuffer *bb);
void bb_print_hex(ByteBuffer *bb);
void free_byte_buffer(ByteBuffer *bb);