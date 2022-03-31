#include "traceroute_sending.h"


u_int16_t compute_icmp_checksum(const void* buff, int length)
{
    u_int32_t sum;
    const u_int16_t* ptr = buff;
    assert(length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16) + (sum & 0xffff);
    return (u_int16_t)(~(sum + (sum >> 16)));
}
int send_packets(struct timeval* send_time, int sockfd, pid_t pid, uint8_t* seq, struct sockaddr_in recipient)
{
    gettimeofday(send_time, NULL);
    for (int packet = 0; packet < 3; packet++) {
        struct icmp header;
        header.icmp_type = ICMP_ECHO;
        header.icmp_code = 0;
        header.icmp_hun.ih_idseq.icd_id = pid;
        header.icmp_hun.ih_idseq.icd_seq = (*seq)++;
        header.icmp_cksum = 0;
        header.icmp_cksum = compute_icmp_checksum((u_int16_t*)&header, sizeof(header));

        ssize_t bytes_sent = sendto(sockfd, &header, sizeof(header), 0, (struct sockaddr*)&recipient, sizeof(recipient));
        if (bytes_sent < 0) {
            fprintf(stderr, "sendto error: %s\n", strerror(errno));
            return 0;
        }
    }
    return 1;
}