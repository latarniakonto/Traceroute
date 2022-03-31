#ifndef TRACEROUTE_RECEIVING_H_
#define TRACEROUTE_RECEIVING_H_
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


int is_expected_packet(struct icmp* header, pid_t pid, u_int8_t ttl);
int receive_packets(int sockfd, pid_t pid, u_int8_t ttl, struct timeval received_time[3], char** received_ip_addrs);
#endif
