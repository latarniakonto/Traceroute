#include "traceroute_essentials.h"
#include "traceroute_receiving.h"
#include "traceroute_sending.h"


int main(int argc, char** argv)
{
    if (argc == 1) {
        fprintf(stderr, "arguments error: wrong number of arguments\n");
        return EXIT_FAILURE;
    }
    char* ipv4_addr = argv[1];    

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    pid_t pid = getpid();
    u_int8_t counter = 0;

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    int convert_IPv4_addr = inet_pton(AF_INET, ipv4_addr, &recipient.sin_addr);
    if (convert_IPv4_addr <= 0) {
        if (convert_IPv4_addr == 0) {
            fprintf(stderr, "inet_pton error: ip adress is in wrong format\n");
        } else {
            fprintf(stderr, "inet_pton error: %s\n", strerror(errno));
        }
        return -1;
    }

    for (int ttl = 1; ttl <= 30; ttl++) {
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0) {
            fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        struct timeval send_time;
        if (!send_packets(&send_time, sockfd, pid, &counter, recipient)) {
            return EXIT_FAILURE;
        }

        char* received_ip_addrs[3];
        struct timeval received_time[3];
        int* received_packets = receive_packets(sockfd, pid, ttl, received_time, received_ip_addrs);

        if (received_packets == NULL)
        {
            return EXIT_FAILURE;
        } else if (received_packets[0] == 3) {
            print_3_packets(received_time, received_ip_addrs, send_time, ttl);
        } else if (received_packets[0] > 0) {
            print_less_than_3_packets(received_packets[0], received_ip_addrs, ttl);
        } else if (received_packets[0] == 0) {
            printf("%d. *\n", ttl);
        }

        if (received_packets[1] == 1) {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}
