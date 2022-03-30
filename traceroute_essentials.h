#ifndef TRACEROUTE_ESSENTIALS_H_
#define TRACEROUTE_ESSENTIALS_H_

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>


int valid_IPv4_check(char* ip);
void print_less_than_3_packets(int received_packets, char** received_ip_addrs);
void print_3_packets(struct timeval received_time[3],
    char** received_ip_addrs, struct timeval send_time);
int final_router(char* ipv4_addr,
    int received_packets, char** received_ip_addrs);
#endif