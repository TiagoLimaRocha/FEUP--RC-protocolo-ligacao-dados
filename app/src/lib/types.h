#pragma once

#include <stdint.h>
#include "macros.h"

typedef unsigned char uchar_t;

typedef struct ControlPacket ControlPacket;
typedef struct DataPacket DataPacket;
typedef struct alInfo alInfo;
typedef struct llInfo llInfo;
typedef struct LinkLayer LinkLayer;

/// alControlType enum to determine the control type of the control field for the application layer
typedef enum alControlType
{
  CONTROL_START = 0x02, /**!< Start of transmisison */
  CONTROL_END = 0x03,   /**!< End of transmission */
  CONTROL_DATA = 0x01   /**!< Ongoing Transmission */
} alControlType;

/// llControlType enum to determine the control type of the control field for the link layer
typedef enum llControlType
{
  LL_INF = 0x00,  /**!< Info */
  LL_SET = 0x03,  /**!< Setup */
  LL_DISC = 0x0B, /**!< Disconnect */
  LL_UA = 0x07,   /**!< Unnumbered acknowledgment */
  LL_RR = 0x05,   /**!< Receiver ready / positive ACk */
  LL_REJ = 0x01   /**!< Reject / negative ACK */
} llControlType;

/// LinkType enum to determine te type of link to be made
typedef enum LinkType
{
  TRANSMITTER = 0x00, /**!< Determines transmitter flow */
  RECEIVER = 0x01,    /**!< Determines reveiver flow */
} LinkType;

/// llControlFrameField enum to determine the control frame field to be used
typedef enum llControlFrameField
{
  FD_FIELD = 0x00,   /**!< Start Flag field */
  AF_FIELD = 0x01,   /**!< Address field */
  C_FIELD = 0x02,    /**!< Control field */
  BCC1_FIELD = 0x03, /**!< BCC1 field */
  DATA_FIELD = 0x04, /**!< Data field start field */
} llControlFrameField;

/// LogType enum for representing the type of logging produced by the logger
typedef enum LogType
{
  INFO,  /**!< Information */
  WARN,  /**!< Warnings */
  ERROR, /**!< Errors */
} LogType;

/// LogCode enum used to manufacture new logs in the log factory
typedef enum LogCode
{
  APP_ERROR_USAGE = 1,                                  /**!< Error for bad app usage */
  BB_ERROR_CREATE = 2,                                  /**!< Error for Byte Buffer creation */
  BB_ERROR_PUSH = 3,                                    /**!< Error for Byte Buffer stuffing */
  BB_ERROR_REMOVE = 4,                                  /**!< Error for  */
  BB_WARN_PRINT_EMPTY = 5,                              /**!< Warning for Byte Buffer empty print */
  BB_WARN_FREE_EMPTY = 6,                               /**!< Warning for Byte Buffer empty free */
  LL_INFO_CLOSE_DISCONECT = 7,                          /**!< Info for Link Layer disconnection */
  LL_ERROR_CLOSE_DISCONECT = -8,                        /**!< Error for Link Layer disconnection */
  LL_WARN_CLOSE_FRAME_IGNORED = 9,                      /**!< Warning for Link Layer ignored close frame */
  LL_ERROR_WRITE_FAIL = -10,                            /**!< Error for Link Layer write failure */
  LL_WARN_READ_FRAME_IGNORED_SET = 11,                  /**!< Warning for Link Layer ignored settup fram */
  LL_WARN_READ_FRAME_IGNORED_CONTROL = 12,              /**!< Warning for Link Layer inored control frame */
  LL_WARN_READ_FRAME_IGNORED_DUPLICATE = 13,            /**!< Warning Link Layer ignored duplicate frame */
  LL_INFO_OPEN_CONNECT = 14,                            /**!< Info for Link Layer new connection opened */
  LL_INFO_OPEN_RECEIVER_WAITING = 15,                   /**!< Info for Link Layer reciever ready */
  LL_WARN_OPEN_RECEIVER_FRAME_IGNORED = 16,             /**!< Warning for Link Layer reciever frame ignored */
  LL_ERROR_OPEN_RECEIVER_UA_PACKET_SEND = -17,          /**!< Error for Link Layer UA packet not sent */
  LL_WARN_FRAME_EXCHANGE_TIMEOUT_OR_INVALID_FRAME = 18, /**!< Warning for Link Layer invalid frame or timeout during exchange */
  LL_WARN_FRAME_EXCHANGE_FRAME_IGNORED_DUPLICATE = 19,  /**!< Warning for Link Layer ignored duplicate frame during exchange */
  LL_WARN_FRAME_EXCHANGE_FRAME_REJECTED = 20,           /**!< Warning for Link Layer rejected frame during exchange */
  LL_WARN_FRAME_EXCHANGE_FRAME_IGNORED_CONTROL = 21,    /**!< Warning for Link Layer ignored control frame during exchange */
  LL_ERROR_FRAME_EXCHANGE_ATTEMPTS_EXCEEDED = -22,      /**!< Error for Link Layer exchange attempts exceeded */
  LL_ERROR_SEND_FRAME_WRITE_TO_PORT = -23,              /**!< Error for Link Layer could not write to port during sending */
  LL_ERROR_READ_FRAME_IGNORED = -24,                    /**!< Error for Link Layer ignored frame error during read */
  LL_ERROR_VALIDATE_CONTROL_FRAME_NULL = -25,           /**!< Error for Link Layer NULL control frame, cannot validate */
  APP_ERROR_GENERAL = -1,                               /**!< Error for application general failures */
  LL_ERROR_FRAME_TOO_SMALL = -2,                        /**!< Error for Link Layer frame too small */
  LL_ERROR_BAD_START_FLAG = -3,                         /**!< Error for Link Layer bad start flag */
  LL_ERROR_BAD_ADDRESS = -4,                            /**!< Error for Link Layer bad address */
  LL_ERROR_BAD_BCC1 = -5,                               /**!< Error for Link Layer bad BCC1 */
  LL_ERROR_BAD_END_FLAG = -6,                           /**!< Error for Link Layer bad end flag */
  LL_WARN_SIG_CONNECTION_TIMED_OUT = -7,                /**!< Warning for Link Layer signal connection timed out */
  AL_ERROR_SEND_FILE_NAME_LENGTH = -26,                 /**!< Error for Application Layer exceeded name length while sending file */
  AL_ERROR_SEND_FILE_OPEN = -27,                        /**!< Error for Application Layer error opening file while sending */
  AL_ERROR_SEND_FILE_CONNECTION = -28,                  /**!< Error for Application Layer could not connect while sending */
  AL_ERROR_SEND_FILE_TRANSMISSION = -29,                /**!< Error for Application Layer bad transmission while sending */
  AL_INFO_SEND_FILE_TRANSMISSION_DONE = 30,             /**!< Info for Application Layer transmisison done */
  AL_INFO_RECEIVE_FILE_WAITING_CONNECTION = 31,         /**!< Info for Application Layer waiting connection */
  AL_WARN_RECEIVE_FILE_DATA_PACKET_IGNORE = 32,         /**!< Warning for Application Layer ignored data packet */
  AL_INFO_RECEIVE_FILE_TRANSMISSION_OVER = 33,          /**!< Info for Application Layer transmisison over */
  AL_ERROR_RECEIVE_FILE_CONNECTION = -34,               /**!< Error for Application Layer connection error while receiving */
  AL_ERROR_RECEIVE_FILE_WRITE = -35,                    /**!< Error for Application Layer file writting error while receiving */
  AL_INFO_RECEIVE_FILE_STARTING_TRANSMISSION = 36,      /**!< Info for Application Layer starting new transmisison */
  AL_ERROR_READ_DATA_PACKET = -37,                      /**!< Error for Application Layer reading data packet */
  AL_ERROR_READ_DATA_PACKET_CONTROL_PACKET = -38,       /**!< Error for Application Layer reading control packet */
  AL_ERROR_SEND_CONTROL_PACKET = -39,                   /**!< Error for Application Layer sending control packet */
  AL_INFO_SEND_CONTROL_PACKET = 40,                     /**!< Info for Application Layer control packet sent */
  AL_ERROR_READ_CONTROL_PACKET_RECEIVE_PACKET = -41,    /**!< Error for Application Layer receiving control packet */
  AL_ERROR_PARSE_CONTROL_PACKET_INVALID_PACKET = -42,   /**!< Error for Application Layer invalid control packet cannot be parsed */
  AL_INFO_SEND_FILE_TRANSMISSION_START = 43,            /**!< Info for Application Layer starting file sending transmission */
} LogCode;

/** @struct ControlPacket
 *  @brief This structure is used to sinalize the beggining and end of file transfer
 * 
 *  @var ControlPacket::type 
 *  Member 'type' Byte that contains the type of the control packet (start, end or data)
 *  @var ControlPacket::nameLength 
 *  Member 'nameLength' Byte that contains the length of the value
 *  @var ControlPacket::size 
 *  Member 'size' contains the size of the packet
 *  @var ControlPacket::sizeLength 
 *  Member 'sizeLength' contains the length of the size
 */
struct ControlPacket
{
  alControlType type;
  char *name;
  int8_t nameLength;
  uint32_t size;
  uint8_t sizeLength;
};

/** @struct DataPacket
 *  @brief This structure contains fragments of the file to transmit
 * 
 *  @var DataPacket::type 
 *  Member 'type' Byte that contains the type of the control field (1 for data)
 *  @var DataPacket::sequenceNo
 *  Member 'sequenceNo' contains the sequence number (mod 255)
 *  @var DataPacket::size 
 *  Member 'size' contains the number of Bytes in the data field
 *  @var DataPacket::data
 *  Member 'data' contains the data of the packet in Bytes
 */
struct DataPacket
{
  alControlType type;
  int8_t sequenceNo;
  uint16_t size;
  char *data;
};

/** @struct alInfo
 *  @brief This structure is used to store information about the application layer during the transmission process
 * 
 *  @var alInfo::baudrate 
 *  Member 'baudrate' contains the rate at which information is transferred in a communication channel (b/s) 
 *  @var alInfo::timeout
 *  Member 'timeout' contains the timeout in seconds
 *  @var alInfo::retries 
 *  Member 'retries' contains the number of retries done
 *  @var alInfo::avgBitsPerSecond
 *  Member 'avgBitsPerSecond' contains the average bit transfer per each second
 *  @var alInfo::dataPacketCount
 *  Member 'dataPacketCount' contains the number of data packets sent
 *  @var alInfo::fileSize 
 *  Member 'fileSize' contains the total size of the file in Bytes
 *  @var alInfo::framesTotal
 *  Member 'framesTotal' contains the total amount of frames parsed
 *  @var alInfo::framesLost
 *  Member 'framesLost' contains the total amount of frames lost
 *  @var alInfo::transmissionDurationSecs
 *  Member 'transmissionDurationSecs' contains the total duration of the transmission process in seconds
 */
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

/** @struct llInfo
 *  @brief This structure is used to store information about the link layer during the transmission process
 * 
 *  @var llInfo::framesTotal 
 *  Member 'framesTotal' contains the total amount of frames in the link layer
 *  @var llInfo::framesLost
 *  Member 'framesLost' contains the amount of lost frames in the link layer
 */
struct llInfo
{
  unsigned int framesTotal;
  unsigned int framesLost;
};

/** @struct LinkLayer
 *  @brief This structure is used to sinalize the beggining and end of file transfer
 * 
 *  @var LinkLayer::port 
 *  Member 'port' contains the serial port used
 *  @var LinkLayer::baudrate
 *  Member 'baudrate' speed of the communication channel
 *  @var LinkLayer::timeout
 *  Member 'timeout' max time to receive response 
 *  @var LinkLayer::transmissionsNo
 *  Member 'transmissionsNo' max frame size
 * 
 */
struct LinkLayer
{
  char port[MAX_PORT_LENGTH];
  int baudrate;
  unsigned int sequenceNo;
  unsigned int timeout;
  unsigned int transmissionsNo;
};
