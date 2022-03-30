#ifndef TRACEROUTE_ESSENTIALS_H_
#define TRACEROUTE_ESSENTIALS_H_

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

u_int16_t compute_icmp_checksum(const void* buff, int length);
int valid_IPv4_check(char* ip);
int send_packets(struct timeval* send_time,
    int sockfd,
    pid_t pid, uint8_t* seq, struct sockaddr_in recipient);
int receive_packets(int sockfd,
    pid_t pid, u_int8_t ttl,
    struct timeval received_time[3], char** received_ip_addrs);
void print_less_than_3_packets(int received_packets, char** received_ip_addrs);
void print_3_packets(struct timeval received_time[3],
    char** received_ip_addrs, struct timeval send_time);
#endif