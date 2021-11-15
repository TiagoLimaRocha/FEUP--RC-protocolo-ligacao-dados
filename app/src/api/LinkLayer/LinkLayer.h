#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include "../../lib/types.h"
#include "../../lib/macros.h"
#include "../../lib/utils/utils.h"
#include "../../lib/ByteBuffer/ByteBuffer.h"

// FUNCTIONAL

/**
* @brief Terminates the connection with the serial port
*
* @param fd: serial port file descriptor 
*
*/
void ll_abort(int fd);

/**
* @brief Saves the configuration for the transfer process 
*
* @param timeout: timeout to close connection
* @param max_retries: number of retries until connection closes
* @param baudrate: speed of the communication  
*
*/
void ll_setup(int timeout, int max_retries, int baudrate);

/**
* @brief Opens the connection to the serial port and begins the setup protocol for file transmission
*
* @param port: serial port number
* @param type: if the app transmits or receives data
*
* @return int APP_ERROR_GENERAL in case of error or the serial port descriptor in case of success
*
*/
int ll_open(int port, LinkType type);

/**
* @brief Begins the disconnection protocol and closes the serial port
*
* @param fd: serial port file descriptor 
*
* @return int APP_ERROR_GENERAL in case of error or the serial port descriptor in case of success
*
*/
int ll_close(int fd);

/**
* @brief Sends a frame through the serial port
*
* @param fd: serial port file descriptor 
* @param buffer: file partition to send
* @param length: size of the file partition
*
* @return  returns APP_ERROR_GENERAL in case of error or the size of the frame written in case of success
*
*/
int ll_write(int fd, char *buffer, int length);

/**
* @brief Reads an incoming frame from the serial port
*
* @param fd: serial port file descriptor 
* @param buffer: buffer to store the data of the frame
*
* @return int APP_ERROR_GENERAL in case of error or the size of the frame read in case of success
*/
int ll_read(int fd, char **buffer);

/**
* @brief Gets the number of dropped frames and total frames received
*
* @return llInfo struct
*/
llInfo ll_info();

// HELPERS

/**
 * @brief Builds the control frame for starting the setup protocol, sends it and waits for the UA from the receiver
 * 
 * @param fd: serial port file descriptor 
 * 
 * @return int APP_ERROR_GENERAL in case of error or the serial port descriptor in case of success
 */
int ll_open_transmitter(int fd);

/**
 * @brief Waits for a control packet to establish a connection, verifies it, and returns an UA
 * 
 * @param fd: serial port file descriptor  
 * 
 * @return int APP_ERROR_GENERAL in case of error or the serial port descriptor in case of success
 */
int ll_open_receiver(int fd);

/**
 * @brief Returns what the address flag should be, depending on the type of control flag and if it's the receiver or transmitter
 * 
 * @param lnk: Receiver/Transmitter
 * @param type: Control Flag of the message
 * 
 * @return LL_AF2 (commands sent by receiver/responses by the transmitter) or LL_AF1 (commands sent by transmitter/responses by the receiver)
 */
char ll_get_address_field(LinkType lnk, llControlType type);

/**
 * @brief Verifies if control flag is one of the ones used by the control frames
 * 
 * @param type: Control flag of the message 
 * 
 * @return true if its LL_INF or LL_DISC or LL_SET
 * @return false if not
 */
bool ll_is_control_command(llControlType type);

/**
 * @brief Reads and returns a frame read from the serial port
 * 
 * @param fd: serial port file descriptor 
 * @param frame: struct that stores the frame information (flags,data,etc)
 * 
 * @return int Returns EXIT_SUCCESS or APP_ERROR_GENERAL
 */
int ll_read_frame(int fd, ByteBuffer *frame);

/**
 * @brief Validates the control flags of a flames
 * 
 * @param frame: frame to analyse 
 * 
 * @return int In case of success EXIT_SUCCESS else one of the following: APP_ERROR_GENERAL, LL_ERROR_FRAME_TOO_SMALL, LL_ERROR_BAD_START_FLAG, LL_ERROR_BAD_ADDRESS, LL_ERROR_BAD_BCC1, LL_ERROR_BAD_END_FLAG
 */
int ll_validate_control_frame(ByteBuffer *frame);

/**
 * @brief Builds and saves control frame to send
 * 
 * @param frame: where the new frame is saved
 * @param type: The C Flag of the frame 
 */
void ll_build_control_frame(ByteBuffer *frame, llControlType type);

/**
 * @brief Builds and saves a data frame to send
 * 
 * @param frame: where the new frame is saved
 * @param buffer: data to send in the frame 
 * @param length: length of data to send 
 */
void ll_build_data_frame(ByteBuffer *frame, char *buffer, int length);

/**
 * @brief Verifies if frame is of a certain type
 * 
 * @param frame: frame to check
 * @param type: C Flag desired to verify
 *  
 * @return true if the both C flags are the same
 * @return false if not
 */
bool ll_is_frame_control_type(ByteBuffer *frame, llControlType type);

/**
 * @brief Writes to the serial port the frame to send
 * 
 * @param fd: serial port file descriptor  
 * @param frame: the data packet
 * 
 * @return int If success EXIT_SUCCESS else APP_ERROR_GENERAL
 */
int ll_send_frame(int fd, ByteBuffer *frame);

/**
 * @brief Sends a data frame through the serial port and waits for the response
 * 
 * @param fd: serial port file descriptor  
 * @param frame: The initial frame to send 
 * @param reply: The type of response expected 
 * 
 * @return int APP_ERROR_GENERAL in case of insuccess or 1 in case of Success
 */
int ll_frame_exchange(int fd, ByteBuffer *frame, llControlType reply);

/**
 * @brief Builds and sends a control frame
 * 
 * @param fd: serial port file descriptor  
 * @param type: type of control message 
 * @return int EXIT_SUCCESS or APP_ERROR_GENERAL
 */
int ll_send_control_frame(int fd, llControlType type);



/**
 * @brief Debug function to print the data of the frame
 * 
 * @param frame: info of the frame 
 * @param type: type of the frame 
 */
void ll_log_frame(ByteBuffer *frame, const char *type);

/**
 * @brief Returns a string of the type of Control Flag (Ex.: "DISC")
 * 
 * @param type: C flag 
 * @return const char* Converted String
 */
const char *ll_get_control_type_str(llControlType type);

/**
 * @brief Opens the serial port for writing and reading and changes the settings of the terminal 
 * 
 * @param port: number of the serial port 
 * @param type: receiver/transmitter
 *  
 * @return int returns the file descriptor of the serial port 
 */
int open_serial_port(int port, LinkType type);

/**
 * @brief Closes the serial port and resets the terminal
 * 
 * @param fd: serial port file descriptor  
 */
void close_serial_port(int fd);