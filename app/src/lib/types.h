#pragma once

#include <stdint.h>
#include "macros.h"

typedef unsigned char uchar_t;

typedef enum
{
  CONTROL_START = 0x02,
  CONTROL_END = 0x03,
  CONTROL_DATA = 0x01
} alControlType;

typedef struct
{
  alControlType type;
  char *name;
  int8_t nameLength;
  uint32_t size;
  uint8_t sizeLength;
} alControlPacket;

typedef struct
{
  alControlType type;
  int8_t sequenceNr;
  uint16_t size;
  char *data;
} DataPacket;

typedef struct
{
  unsigned int baudrate;
  unsigned int timeout;
  unsigned int retries;
  unsigned int avgBitsPerSecond;
  unsigned int dataPacketCount;
  unsigned int fileSize;
  unsigned int framesTotal;
  unsigned int framesLost;
  double transmissionDurationSecs;
} alInfo;

typedef enum
{
  LL_INF = 0x00,
  LL_SET = 0x03,
  LL_DISC = 0x0B,
  LL_UA = 0x07,
  LL_RR = 0x05,
  LL_REJ = 0x01
} llControlType;

typedef struct
{
  unsigned int framesTotal;
  unsigned int framesLost;
} llInfo;

/**
 * Enum of types of Link Layer connection. RECEIVER(0x01) or TRANSMITTER(0x01).
 */
typedef enum
{
  TRANSMITTER = 0x00,
  RECEIVER = 0x01
} LinkType;

typedef struct
{
  char port[MAX_PORT_LENGTH];
  int baud_rate;
  unsigned int sequence_number;
  unsigned int timeout;
  unsigned int num_transmissions;
} LinkLayer;

typedef enum
{
  FD_FIELD = 0x00,   // Start Flag
  AF_FIELD = 0x01,   // Address
  C_FIELD = 0x02,    // Control
  BCC1_FIELD = 0x03, // BCC1
  DATA_FIELD = 0x04, // Data field start
} llControlFrameField;

typedef enum
{
  INFO,
  WARN,
  ERROR,
} LogType;

typedef enum
{
  APP_ERROR_USAGE = 1,
  LL_ERROR_OK = 0,
  LL_ERROR_GENERAL = -1,
  LL_ERROR_FRAME_TOO_SMALL = -2,
  LL_ERROR_BAD_START_FLAG = -3,
  LL_ERROR_BAD_ADDRESS = -4,
  LL_ERROR_BAD_BCC1 = -5,
  LL_ERROR_BAD_END_FLAG = -6
} LogCode;