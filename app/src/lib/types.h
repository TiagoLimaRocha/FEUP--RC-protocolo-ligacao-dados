#pragma once

#include <stdint.h>
#include "macros.h"

typedef unsigned char uchar_t;

typedef struct alControlPacket alControlPacket;
typedef struct DataPacket DataPacket;
typedef struct alInfo alInfo;
typedef struct llInfo llInfo;
typedef struct LinkLayer LinkLayer;

typedef enum alControlType
{
  CONTROL_START = 0x02,
  CONTROL_END = 0x03,
  CONTROL_DATA = 0x01
} alControlType;

typedef enum llControlType
{
  LL_INF = 0x00,
  LL_SET = 0x03,
  LL_DISC = 0x0B,
  LL_UA = 0x07,
  LL_RR = 0x05,
  LL_REJ = 0x01
} llControlType;

typedef enum LinkType
{
  TRANSMITTER = 0x00,
  RECEIVER = 0x01
} LinkType;

typedef enum llControlFrameField
{
  FD_FIELD = 0x00,   // Start Flag
  AF_FIELD = 0x01,   // Address
  C_FIELD = 0x02,    // Control
  BCC1_FIELD = 0x03, // BCC1
  DATA_FIELD = 0x04, // Data field start
} llControlFrameField;

typedef enum LogType
{
  INFO,
  WARN,
  ERROR,
} LogType;

typedef enum LogCode
{
  APP_ERROR_USAGE = 1,
  BB_ERROR_CREATE = 2,
  BB_ERROR_PUSH = 3,
  BB_ERROR_REMOVE = 4,
  BB_WARN_PRINT_EMPTY = 5,
  BB_WARN_FREE_EMPTY = 6,
  LL_ERROR_OK = 0,
  LL_ERROR_GENERAL = -1,
  LL_ERROR_FRAME_TOO_SMALL = -2,
  LL_ERROR_BAD_START_FLAG = -3,
  LL_ERROR_BAD_ADDRESS = -4,
  LL_ERROR_BAD_BCC1 = -5,
  LL_ERROR_BAD_END_FLAG = -6
} LogCode;

struct alControlPacket
{
  alControlType type;
  char *name;
  int8_t nameLength;
  uint32_t size;
  uint8_t sizeLength;
};

struct DataPacket
{
  alControlType type;
  int8_t sequenceNo;
  uint16_t size;
  char *data;
};

struct alInfo
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
};

struct llInfo
{
  unsigned int framesTotal;
  unsigned int framesLost;
};

struct LinkLayer
{
  char port[MAX_PORT_LENGTH];
  int baudrate;
  unsigned int sequenceNo;
  unsigned int timeout;
  unsigned int transmissionsNo;
};
