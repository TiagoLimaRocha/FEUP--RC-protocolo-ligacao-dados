#include "ByteBuffer.h"

int create_byte_buffer(ByteBuffer *bb, int initSize)
{
  if (bb == NULL)
  {
    logger(ERROR, BB_ERROR_CREATE);
    return BB_ERROR_CREATE;
  }

  bb->buffer = (unsigned char *)malloc(initSize * sizeof(unsigned char));
  if (bb->buffer == NULL)
  {
    logger(ERROR, BB_ERROR_CREATE);
    return BB_ERROR_CREATE;
  }

  bb->size = 0;
  bb->capacity = initSize * sizeof(unsigned char);

  return EXIT_SUCCESS;
}

int bb_push(ByteBuffer *bb, char byte)
{
  if (bb == NULL || bb->buffer == NULL)
  {
    logger(ERROR, BB_ERROR_PUSH);
    return BB_ERROR_PUSH;
  }

  if (bb->size * sizeof(unsigned char) >= bb->capacity)
  {
    bb->buffer = (unsigned char *)realloc(bb->buffer, bb->capacity * 2);
    if (bb->buffer == NULL)
    {
      logger(ERROR, BB_ERROR_PUSH);
      return BB_ERROR_PUSH;
    }
    bb->capacity *= 2;
  }

  bb->buffer[bb->size] = byte;
  ++bb->size;

  return 0;
}

int bb_remove(ByteBuffer *bb, unsigned int pos)
{
  if (bb == NULL || bb->buffer == NULL || pos > bb->size + 1)
  {
    logger(ERROR, BB_ERROR_REMOVE);
    return BB_ERROR_REMOVE;
  }

  if (pos == bb->size + 1)
  {
    bb->buffer[pos] = (unsigned char)0x00;
    --bb->size;
    return EXIT_SUCCESS;
  }

  memmove(bb->buffer + pos * sizeof(unsigned char),
          bb->buffer + (pos + 1) * sizeof(unsigned char),
          bb->capacity - (pos + 1) * sizeof(unsigned char));

  --bb->size;

  return EXIT_SUCCESS;
}

void bb_print(ByteBuffer *bb)
{
  if (bb == NULL || bb->buffer == NULL)
  {
    logger(WARN, BB_WARN_PRINT_EMPTY);
    return;
  }

  printf("Buffer Content: ");

  for (unsigned int i = 0; i < bb->size; ++i)
    printf("%c ", bb->buffer[i]);

  printf(" Elements: %d Capacity: %d", bb->size, bb->capacity);
  printf("\n\n");
}

void bb_print_hex(ByteBuffer *bb)
{
  if (bb == NULL || bb->buffer == NULL)
  {
    logger(WARN, BB_WARN_PRINT_EMPTY);
    return;
  }

  printf("Buffer Content: ");

  for (unsigned int i = 0; i < bb->size; ++i)
    printf("%x ", bb->buffer[i]);

  printf(" Elements: %d Capacity: %d", bb->size, bb->capacity);
  printf("\n\n");
}

void free_byte_buffer(ByteBuffer *bb)
{
  if (bb == NULL || bb->buffer == NULL)
  {
    logger(WARN, BB_WARN_FREE_EMPTY);
    return;
  }

  free(bb->buffer);
}