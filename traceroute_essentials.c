#include "traceroute_essentials.h"


void print_less_than_3_packets(int received_packets, char** received_ip_addrs, int ttl)
{
    printf("%d. ", ttl);
    for (int i = 0; i < received_packets; i++) {
        if (strncmp(received_ip_addrs[i], "DUPLICATE", 10) != 0) {
            printf("%s ", received_ip_addrs[i]);
        }
    }
    printf("???\n");
}
suseconds_t get_average_time(struct timeval send_time, struct timeval received_time[3])
{
    struct timeval diff;
    suseconds_t average_time = 0;
    for (int i = 0; i < 3; i++) {
        timersub(&received_time[i], &send_time, &diff);
        average_time += diff.tv_usec;
    }
    return average_time / 3000;
}
void print_3_packets(struct timeval received_time[3], char** received_ip_addrs, struct timeval send_time, int ttl)
{
    printf("%d. ", ttl);
    suseconds_t avg_time = get_average_time(send_time, received_time);
    for (int i = 0; i < 3; i++) {
        if (strncmp(received_ip_addrs[i], "DUPLICATE", 10) != 0) {
            printf("%s ", received_ip_addrs[i]);
        }
    }
    printf("%ldms\n", avg_time);
}
