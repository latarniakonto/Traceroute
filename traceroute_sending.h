#ifndef TRACEROUTE_SENDING_H_
#define TRACEROUTE_SENDING_H_
#include <assert.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


int send_packets(struct timeval* send_time, int sockfd, pid_t pid, uint8_t* seq, struct sockaddr_in recipient);
#endif
