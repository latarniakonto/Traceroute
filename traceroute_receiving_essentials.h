#ifndef TRACEROUTE_RECEIVING_ESSENTIALS_H_
#define TRACEROUTE_RECEIVING_ESSENTIALS_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <netinet/ip.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

int expected_packet(struct icmp* header, pid_t pid, u_int8_t ttl);
int receive_packets(int sockfd,
    pid_t pid, u_int8_t ttl,
    struct timeval received_time[3], char** received_ip_addrs);
#endif