#ifndef TRACEROUTE_SENDING_ESSENTIALS_H_
#define TRACEROUTE_SENDING_ESSENTIALS_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

int send_packets(struct timeval* send_time,
    int sockfd,
    pid_t pid, uint8_t* seq, struct sockaddr_in recipient);
#endif
