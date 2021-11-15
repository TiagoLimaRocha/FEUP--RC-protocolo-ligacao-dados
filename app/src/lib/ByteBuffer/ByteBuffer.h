#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "../types.h"

/** @defgroup Byte Buffer
 * @{
 *
 * Functions for the byte stuffing process
 */

typedef struct ByteBuffer ByteBuffer;

/** @struct ByteBuffer
 *  @brief This structure is used to create a new empty packet and stuff it with
 *         Bytes
 * 
 *  @var ByteBuffer::buffer 
 *  Member 'buffer' contains the byte array
 *  @var ByteBuffer::size
 *  Member 'size' contains the current size of the packet
 *  @var ByteBuffer::capacity
 *  Member 'capacity' contains the total capacity of the packet
 */
struct ByteBuffer
{
  unsigned char *buffer;
  unsigned int size;
  unsigned int capacity;
};

/**
 * @brief Initializes a new instance of the byte buffer with initial values
 * @param bb empty byte buffer instance
 * @param initSize initial size of the packet
 */
int create_byte_buffer(ByteBuffer *bb, int initSize);

/**
 * @brief Stuffs a new byte into the packet buffer
 * @param bb byte buffer instance
 * @param byte byte to be stuffed
 */
int bb_push(ByteBuffer *bb, char byte);

/**
 * @brief Destuffs a given byte in the packet buffer
 * @param bb byte buffer instance
 * @param pos position of the byte to be removed in the array
 */
int bb_remove(ByteBuffer *bb, unsigned int pos);

/**
 * @brief Prints bytebuffer in binary format
 * @param bb byte buffer instance
 */
void bb_print(ByteBuffer *bb);

/**
 * @brief Prints bytebuffer in hexadecimal format
 * @param bb byte buffer instance
 */
void bb_print_hex(ByteBuffer *bb);

/**
 * @brief Frees up the allocated memory for a given byte buffer instance
 * @param bb byte buffer instance
 */
void free_byte_buffer(ByteBuffer *bb);