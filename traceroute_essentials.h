#ifndef TRACEROUTE_ESSENTIALS_H_
#define TRACEROUTE_ESSENTIALS_H_

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>

u_int16_t compute_icmp_checksum(const void* buff, int length);
int valid_IPv4_check(char* ip);
int send_packets(struct timeval* send_time,
 				 int sockfd, pid_t pid, uint8_t* seq, struct sockaddr_in recipient);
#endif