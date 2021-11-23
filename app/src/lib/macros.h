#pragma once

/** @defgroup macros
 * @{
 *
 * Constants for programming the many layers of the API.
 */

// Options
#define HT_CAPACITY 50000 /**< @brief Hash map max memory allocation size in Bytes */
#define BUFFER 6042       /**< @brief String buffer max memory allocation size in Bytes */

#define DEFAULT_MAX_TRANSMISSION_ATTEMPS 3 /**!< @brief Default value for maximum transmisison attempts  */
#define DEFAULT_TIMEOUT_DURATION 3         /**!< @brief Default value for timeout duration in seconds  */
#define DEFAULT_BAUDRATE 38400             /**!< @brief Default rate at which information is transferred in a communication channel (b/s) */

// Application Layer
#define MAX_FRAGMENT_SIZE 0xFFFF /**!< @brief Maximum value for the chunk fragment size to be transfered */
#define MAX_BAUDRATE 460800      /**!< @brief Maximum rate at which information is transferred in a communication channel (b/s)  */
#define DATA_HEADER_SIZE 4       /**!< @brief Size in Bytes of the header of the package */
#define MAX_FILE_NAME 256        /**!< @brief Maximum length of the filename string in chars (Bytes) */

#define CP_CFIELD 0x00 /**!< @brief Control package control field */
#define SEQ_FIELD 0x01 /**!< @brief Sequence number field */
#define L2_FIELD 0x02  /**!< @brief TLV Byte for length in Bytes of the value field */
#define L1_FIELD 0x03  /**!< @brief TLV Byte for length in Bytes of the value field */

#define TLV_SIZE_T 0x00 /**!< @brief TLV Byte for file size */
#define TLV_NAME_T 0x01 /**!< @brief TLV Byte for file name */
#define CP_MIN_SIZE 7   /**!< @brief Minimum size in Bytes of the Control Packet */

#define AL_LOG_INFORMATION /**!< @brief Feature flag to log App layer info */
// #define AL_PRINT_CPACKETS /**!< @brief Feature flag to log control packets read */

// Link Layer
#define LL_FLAG 0x7E    /**!< @brief Flag for beggining and ending of frame  */
#define LL_ESC 0x7D     /**!< @brief PPP escape mechanism character for byte stuffing */
#define LL_ESC_MOD 0x20 /**!< @brief Stuffing byte  */
#define LL_AF1 0x03     /**!< @brief Transmitter commands, Receiver replys  */
#define LL_AF2 0x01     /**!< @brief Transmitter replys, Receiver commands  */

#define FERPROB 20 /**!< @brief File error predefined probability for testing purposes */
// #define FER        /**!< @brief Feature flag for FER testing */
// #define LL_LOG_FRAMES /**!< @brief Feature flag for logging the frames in the link layer */
// #define LL_LOG_BUFFER /**!< @brief Feature flag for logging the buffer  in ll_read_frame method */

#define _POSIX_SOURCE 1 /**!< @brief POSIX compliant source  */

#define INF_FRAME_SIZE 6     /**!< @brief Size in Bytes of the I frames */
#define CONTROL_FRAME_SIZE 5 /**!< @brief Size in Bytes of the control frame */
#define MAX_PORT_LENGTH 20   /**!< @brief Maximum length in Bytes of the port buffer */

#define MAX_TRANSMISSION_ATTEMPS 3 /**!< @brief Maximum number of transmission attempts before timeout */
#define TIMEOUT_DURATION 3         /**!< @brief Timeout duration in seconds */

// Port name prefix
#ifdef __linux__
#define PORT_NAME "/dev/ttyS" /**!< @brief Port description for linux systems */
#elif _WIN32
#define PORT_NAME "COM" /**!< @brief Port description for windows systems */
#else
#define PORT_NAME "/dev/ttyS"
#endif