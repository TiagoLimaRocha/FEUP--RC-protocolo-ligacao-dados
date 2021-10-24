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

void al_setup(int timeout, int baudrate, int maxRetries, int fragSize);
int al_send_file(const char *filename, int port);
int al_receive_file(const char *filename, int port);
int al_get_file_info(const char *filename, FILE *fptr);
void al_print_info();
void al_update_info(struct timeval *startTimer);
alInfo al_info();

void print_control_packet(ControlPacket *packet);
int read_control_packet(int fd, ControlPacket *packet);
int parse_control_packet(char *packetBuffer, int size, ControlPacket *cp);
int send_control_packet(int fd, alControlType type);
void build_control_packet(alControlType type, ByteBuffer *packet);

int read_data_packet(int fd, DataPacket *packet, char *buffer);
int send_data_packet(int fd, DataPacket *packet);