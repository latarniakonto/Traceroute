#include "traceroute_essentials.h"


int valid_IPv4_check(char* ip)
{
    int* bytes = (int*)calloc(sizeof(int), 4);
    int idx = -1;
    for (int byte = 0; byte < 4; byte++) {
        int steps_counter = 0;
        while (ip[++idx] != '\0' && ip[idx] != '.') {
            int digit = ip[idx] - '0';
            if (digit > 9 || digit < 0) {
                free(bytes);
                return -1;
            }
            if (steps_counter == 0 && digit == 0) {
                free(bytes);
                return -1;
            }
            bytes[byte] *= 10;
            bytes[byte] += digit;
            steps_counter++;
        }
        if (steps_counter > 3 || bytes[byte] > 255 || bytes[byte] < 0) {
            free(bytes);
            return -1;
        }
    }
    free(bytes);
    return 1;
}

void print_less_than_3_packets(int received_packets, char** received_ip_addrs)
{
    for (int i = 0; i < received_packets; i++) {
        if (strcmp(received_ip_addrs[i], "DUPLICAT") != 0) {
            printf("%s ", received_ip_addrs[i]);
        }
    }
    printf("???\n");
}
suseconds_t get_average_time(struct timeval send_time,
    struct timeval received_time[3])
{
    struct timeval diff;
    suseconds_t average_time = 0;
    for (int i = 0; i < 3; i++) {
        timersub(&received_time[i], &send_time, &diff);
        average_time += diff.tv_usec;
    }
    return average_time / 3000;
}
void print_3_packets(struct timeval received_time[3],
    char** received_ip_addrs, struct timeval send_time)
{

    suseconds_t avg_time = get_average_time(send_time, received_time);
    for (int i = 0; i < 3; i++) {
        if (strcmp(received_ip_addrs[i], "DUPLICAT") != 0) {
            printf("%s ", received_ip_addrs[i]);
        }
    }
    printf("%ldms\n", avg_time);
}

int final_router(char* ipv4_addr,
    int received_packets, char** received_ip_addrs)
{
    for (int i = 0; i < received_packets; i++) {
        if (strcmp(received_ip_addrs[i], ipv4_addr) == 0)
            return 1;
    }
    return -1;
}