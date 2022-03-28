#include "traceroute_essentials.h"

u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

int valid_IPv4_check(char* ip)
{
	int* bytes = (int*)calloc(sizeof(int), 4);
	int idx = -1;
	for (int byte = 0; byte < 4 ; byte++) {
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
int send_packets(struct timeval* send_time,
 				 int sockfd, pid_t pid, uint8_t* seq, struct sockaddr_in recipient)
{	
	gettimeofday(send_time, NULL);
	for (int packet = 1; packet <= 3; packet++) {		
		struct icmp header;
		header.icmp_type = ICMP_ECHO;
		header.icmp_code = 0;
		header.icmp_hun.ih_idseq.icd_id = pid;
		header.icmp_hun.ih_idseq.icd_seq = (*seq)++;
		header.icmp_cksum = 0;
		header.icmp_cksum = compute_icmp_checksum((u_int16_t*)&header,
												sizeof(header));
		
		ssize_t bytes_sent = sendto(sockfd,
									&header,
									sizeof(header),
									0,
									(struct sockaddr*)&recipient,
									sizeof(recipient));
		if (bytes_sent < 0) {
			fprintf(stderr, "sendto error: %s\n", strerror(errno));
			return -1;
		}
	}
	return 1;
}
