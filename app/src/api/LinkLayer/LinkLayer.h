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
void ll_abort(int fd);
void ll_setup(int timeout, int max_retries, int baudrate);
int ll_open(int port, LinkType type);
int ll_close(int fd);
int ll_write(int fd, char *buffer, int length);
int ll_read(int fd, char **buffer);
llInfo ll_info();

// HELPERS
int ll_open_transmitter(int fd);
int ll_open_receiver(int fd);
char ll_get_address_field(LinkType lnk, llControlType type);
bool ll_is_control_command(llControlType type);
int ll_read_frame(int fd, ByteBuffer *frame);
int ll_validate_control_frame(ByteBuffer *frame);
void ll_printControlType(llControlType type);
void ll_build_control_frame(ByteBuffer *frame, llControlType type);
void ll_build_data_frame(ByteBuffer *frame, char *buffer, int length);
bool ll_is_frame_control_type(ByteBuffer *frame, llControlType type);
int ll_send_frame(int fd, ByteBuffer *frame);
int ll_frame_exchange(int fd, ByteBuffer *frame, llControlType reply);
int ll_send_control_frame(int fd, llControlType type);
void ll_build_data_frame(ByteBuffer *frame, char *buffer, int length);
void ll_log_frame(ByteBuffer *frame, const char *type);
const char *ll_get_control_type_str(llControlType type);
int open_serial_port(int port, LinkType type);
void close_serial_port(int fd);