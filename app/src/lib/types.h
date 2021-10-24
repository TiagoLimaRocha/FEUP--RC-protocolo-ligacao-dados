#pragma once

#include <stdint.h>
#include "macros.h"

typedef unsigned char uchar_t;

typedef struct ControlPacket ControlPacket;
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
  LL_INFO_CLOSE_DISCONECT = 7,
  LL_ERROR_CLOSE_DISCONECT = -8,
  LL_WARN_CLOSE_FRAME_IGNORED = 9,
  LL_ERROR_WRITE_FAIL = -10,
  LL_WARN_READ_FRAME_IGNORED_SET = 11,
  LL_WARN_READ_FRAME_IGNORED_CONTROL = 12,
  LL_WARN_READ_FRAME_IGNORED_DUPLICATE = 13,
  LL_INFO_OPEN_CONNECT = 14,
  LL_INFO_OPEN_RECEIVER_WAITING = 15,
  LL_WARN_OPEN_RECEIVER_FRAME_IGNORED = 16,
  LL_ERROR_OPEN_RECEIVER_UA_PACKET_SEND = -17,
  LL_WARN_FRAME_EXCHANGE_TIMEOUT_OR_INVALID_FRAME = 18,
  LL_WARN_FRAME_EXCHANGE_FRAME_IGNORED_DUPLICATE = 19,
  LL_WARN_FRAME_EXCHANGE_FRAME_REJECTED = 20,
  LL_WARN_FRAME_EXCHANGE_FRAME_IGNORED_CONTROL = 21,
  LL_ERROR_FRAME_EXCHANGE_ATTEMPTS_EXCEEDED = -22,
  LL_ERROR_SEND_FRAME_WRITE_TO_PORT = -23,
  LL_ERROR_READ_FRAME_IGNORED = -24,
  LL_ERROR_VALIDATE_CONTROL_FRAME_NULL = -25,
  APP_ERROR_GENERAL = -1,
  LL_ERROR_FRAME_TOO_SMALL = -2,
  LL_ERROR_BAD_START_FLAG = -3,
  LL_ERROR_BAD_ADDRESS = -4,
  LL_ERROR_BAD_BCC1 = -5,
  LL_ERROR_BAD_END_FLAG = -6,
  LL_WARN_SIG_CONNECTION_TIMED_OUT = -7,
  AL_ERROR_SEND_FILE_NAME_LENGTH = -26,
  AL_ERROR_SEND_FILE_OPEN = -27,
  AL_ERROR_SEND_FILE_CONNECTION = -28,
  AL_ERROR_SEND_FILE_TRANSMISSION = -29,
  AL_INFO_SEND_FILE_TRANSMISSION_DONE = 30,
  AL_INFO_RECEIVE_FILE_WAITING_CONNECTION = 31,
  AL_WARN_RECEIVE_FILE_DATA_PACKET_IGNORE = 32,
  AL_INFO_RECEIVE_FILE_TRANSMISSION_OVER = 33,
  AL_ERROR_RECEIVE_FILE_CONNECTION = -34,
  AL_ERROR_RECEIVE_FILE_WRITE = -35,
  AL_INFO_RECEIVE_FILE_STARTING_TRANSMISSION = 36,
  AL_ERROR_READ_DATA_PACKET = -37,
  AL_ERROR_READ_DATA_PACKET_CONTROL_PACKET = -38,
  AL_ERROR_SEND_CONTROL_PACKET = -39,
  AL_INFO_SEND_CONTROL_PACKET = 40,
  AL_ERROR_READ_CONTROL_PACKET_RECEIVE_PACKET = -41,
  AL_ERROR_PARSE_CONTROL_PACKET_INVALID_PACKET = -42,
  AL_INFO_SEND_FILE_TRANSMISSION_START = 43,
} LogCode;

struct ControlPacket
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
