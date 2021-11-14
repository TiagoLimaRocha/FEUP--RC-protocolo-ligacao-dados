#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "../../lib/types.h"
#include "../../lib/macros.h"
#include "../../lib/utils/utils.h"
#include "../../lib/ByteBuffer/ByteBuffer.h"
#include "../LinkLayer/LinkLayer.h"

/** @defgroup Application Layer
 * @{
 *
 * Functions for the application layer to transfer files
 */

/**
 * @brief Starts the application layer and sets up all the proper option values.
 * @param timeout timeout duration in seconds
 * @param baudrate rate at which information is transferred in a communication channel (b/s) 
 * @param maxRetries transmisison attempts 
 * @param fragSize data fragment size
 */
void al_setup(int timeout, int baudrate, int maxRetries, int fragSize);

/**
 * @brief Opens a connections and proceeds with the file transfering flow. 
 *        Closes when file finished transfering.
 *        It first reads the file and then sends a control packet to sinalize 
 *        the beggining of the transfer. Then for each data fragment a data packet is
 *        built which includes the filename, sequence number, fragment size, and fragment
 *        data using the ll_write method from the Link Layer. When all fragments are sent, 
 *        another control packet is sent to mark the transfer ending.
 * @param filename name/path of the file to transfer
 * @param port port in which the transfer takes place
 * @return Return 0 if success
 */
int al_send_file(const char *filename, int port);

/**
 * @brief Opens a connections and proceeds with the file receiving flow. 
 *        Closes when file is received.
 *        Uses the ll_read method from Link Layer to read the transmitted data packets.
 *        While it receives new packets, the funciton stores the data fragments in a file
 *        until it receives the control packet marking the end of transmission. This method
 *        validates the packet sequence numbers and, at the end, if it has received the 
 *        proper amount of bytes.
 * @param filename name/path of the file to transfer
 * @param port port in which the transfer takes place
 * @return Return 0 if success
 */
int al_receive_file(const char *filename, int port);

/**
 * @brief Gets the file name and size by seeking to a certain position of the file pointer on STREAM, 
 *        returning the current position of STREAM and rewinding the file pointer to the beginning 
 *        of STREAM. 
 * @param filename name/path of the file to transfer
 * @param fptr file pointer
 * @return Return 0 if success
 */
int al_get_file_info(const char *filename, FILE *fptr);

/**
 * @brief Prints the file information to the standard output
 */
void al_print_info();

/**
 * @brief Updates the file information structure everytime a file is sent or received 
 * @param startTimer structure to contain all the game sprites
 */
void al_update_info(struct timeval *startTimer);

/**
 * @brief Gets the application layer information
 * @return Returns the alInfo structure
 */
alInfo al_info();

/**
 * @brief Prints the control packet to the standard output
 * @param packet control packet structure to be printed out
 */
void print_control_packet(ControlPacket *packet);

/**
 * @brief Reads the control packet at the beggining and ending of file reception 
 * @param fd file descriptor of the opened receiving port
 * @param packet control packet
 * @return Return 0 if success
 */
int read_control_packet(int fd, ControlPacket *packet);

/**
 * @brief Parses and interprets the control packet. 
 *        Reads the raw byte and fills each field of the control packet struct with the
 *        corresponding data 
 * @param packetBuffer raw control packet Byte
 * @param size total memory allocated to the packet buffer after reading the link layer
 * @param packet control packet
 * @return Return 0 if success
 */
int parse_control_packet(char *packetBuffer, int size, ControlPacket *packet);

/**
 * @brief Sends control packet to the specified port using ll_write
 *        method from the Link Layer
 * @param fd file descriptor of the opened port
 * @param type trype of control packet to be sent (start, end or data)
 * @return Return 0 if success
 */
int send_control_packet(int fd, alControlType type);

/**
 * @brief Builds a control packet using the TLV coding format
 * @param type indicates control type parameter (start, end or data for ongoing)
 * @param packet packet to be byte stuffed
 */
void build_control_packet(alControlType type, ByteBuffer *packet);

/**
 * @brief Reads data packet from the specified port using ll_read method
 *        from te Link Layer and builds the data packet with the information
 *        stored in the buffer
 * @param fd file descriptor of the opened port
 * @param packet data packet
 * @param buffer used to store the read Byte from the Link Layer
 * @return Return 0 if success
 */
int read_data_packet(int fd, DataPacket *packet, char *buffer);

/**
 * @brief Feeds in the proper data to the C, N, L2 and L1 fields of the data packet
 *        and sends data packet to a specified port using ll_write method from the Link Layer
 * @param fd file descriptor of the opened port
 * @param packet data packet
 * @return Return 0 if success
 */
int send_data_packet(int fd, DataPacket *packet);