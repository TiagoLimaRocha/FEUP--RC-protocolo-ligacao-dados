#pragma once

// Options
#define HT_CAPACITY 50000
#define BUFFER 6042

#define DEFAULT_MAX_TRANSMISSION_ATTEMPS 3
#define DEFAULT_TIMEOUT_DURATION 3
#define DEFAULT_BAUDRATE 38400
#define MAX_FRAGMENT_SIZE 0xFFFF

// Application Layer
#define MAX_FRAGMENT_SIZE 0xFFFF
#define MAX_BAUDRATE 460800
#define DATA_HEADER_SIZE 4
#define MAX_FILE_NAME 256

#define CP_CFIELD 0x00
#define SEQ_FIELD 0x01
#define L2_FIELD 0x02
#define L1_FIELD 0x03

#define TLV_SIZE_T 0x00
#define TLV_NAME_T 0x01
#define CP_MIN_SIZE 7

#define AL_LOG_INFORMATION

// Link Layer
#define LL_FLAG 0x7E    // Flag for beggining and ending of frame
#define LL_ESC 0x7D     // Escape character for byte stuffing
#define LL_ESC_MOD 0x20 // Stuffing byte
#define LL_AF1 0x03     // Transmitter commands, Receiver replys
#define LL_AF2 0x01     // Transmitter replys, Receiver commands

#define FERPROB 20

#define _POSIX_SOURCE 1 // POSIX compliant source

#define INF_FRAME_SIZE 6
#define CONTROL_FRAME_SIZE 5
#define MAX_PORT_LENGTH 20

#define MAX_TRANSMISSION_ATTEMPS 3
#define TIMEOUT_DURATION 3

// Port name prefix
#ifdef __linux__
#define PORT_NAME "/dev/ttyS"
#elif _WIN32
#define PORT_NAME "COM"
#else
#define PORT_NAME "/dev/ttyS"
#endif