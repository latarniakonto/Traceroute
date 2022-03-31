#include "traceroute_receiving.h"


int is_expected_packet(struct icmp* header, pid_t pid, u_int8_t ttl)
{
    return (header->icmp_hun.ih_idseq.icd_id == pid &&
            header->icmp_hun.ih_idseq.icd_seq < ttl * 3 &&
            header->icmp_hun.ih_idseq.icd_seq >= (ttl * 3) - 3);
}
int receive_packets(int sockfd, pid_t pid, u_int8_t ttl, struct timeval received_time[3], char** received_ip_addrs)
{
    int received_packets = 0;
    while (received_packets < 3) {
        fd_set readfd;
        FD_ZERO(&readfd);
        FD_SET(sockfd, &readfd);
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ready = select(sockfd + 1, &readfd, NULL, NULL, &tv);
        if (ready < 0) {
            fprintf(stderr, "select error: %s\n", strerror(errno));
            return -1;
        } else if (ready == 0) {
            return received_packets;
        } else {
            struct sockaddr_in sender;
            socklen_t sender_len = sizeof(sender);
            u_int8_t buffer[IP_MAXPACKET];

            ssize_t bytes_received = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
            if (bytes_received < 0) {
                fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
                return -1;
            } else {
                char sender_ip_str[20];
                if (inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str)) == NULL) {
                    fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
                    return -1;
                }

                struct ip* ip_header = (struct ip*)buffer;
                ssize_t ip_header_len = 4 * ip_header->ip_hl;
                u_int8_t* icmp_packet = buffer + ip_header_len;
                struct icmp* icmp_header = (struct icmp*)icmp_packet;
                if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED)
                    icmp_header++;

                if (is_expected_packet(icmp_header, pid, ttl)) {
                    gettimeofday(&received_time[received_packets], NULL);
                    int duplicate_ip_addr = 0;
                    for (int i = 0; i < received_packets; i++) {
                        if (strcmp(sender_ip_str, received_ip_addrs[i]) == 0) {
                            duplicate_ip_addr = 1;
                        }
                    }
                    // Sometimes recvfrom() would override memory
                    // when running ping and this program in parallel
                    char prevent_memory_override_str[20];
                    for (int i = 0; i < 20; i++) {
                        prevent_memory_override_str[i] = sender_ip_str[i];
                    }
                    if (duplicate_ip_addr > 0) {
                        received_ip_addrs[received_packets++] = "DUPLICATE";
                    } else {
                        received_ip_addrs[received_packets++] = prevent_memory_override_str;
                    }
                }
            }
        }
    }
    return received_packets;
}
