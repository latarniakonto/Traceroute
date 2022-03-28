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
 				 int sockfd,
				 pid_t pid, uint8_t* seq, struct sockaddr_in recipient)
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
int receive_packets(int sockfd,
					pid_t pid, u_int8_t ttl, 
					struct timeval received_time[3], char** received_ip_addrs)
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
		}
		else if (ready == 0) {
			return 0;
		}
		else {
			struct sockaddr_in sender;
			socklen_t sender_len = sizeof(sender);
			u_int8_t buffer[IP_MAXPACKET];

			ssize_t bytes_recevied = recvfrom(sockfd,
										buffer,
										IP_MAXPACKET,
										MSG_DONTWAIT,
										(struct sockaddr*)&sender, &sender_len);
			if (bytes_recevied < 0) {
				break;
			}else {
				char sender_ip_str[20];
				inet_ntop(AF_INET,
						&(sender.sin_addr),
						sender_ip_str,
						sizeof(sender_ip_str));
						
				if (sender_ip_str == NULL) {
					fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
					return -1;
				}

				struct ip* ip_header = (struct ip*)buffer;
				ssize_t ip_header_len = 4 * ip_header->ip_hl;
				u_int8_t* icmp_packet = buffer + ip_header_len;
				struct icmp* icmp_header = (struct icmp*)icmp_packet;
				if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED)
					icmp_header++;
				if (icmp_header->icmp_hun.ih_idseq.icd_id == pid &&
					icmp_header->icmp_hun.ih_idseq.icd_seq < ttl * 3 &&
					icmp_header->icmp_hun.ih_idseq.icd_seq >= (ttl * 3) - 3) {
					gettimeofday(&received_time[received_packets], NULL);
					int duplicate_ip_addr = -1;						
					for (int i = 0; i < received_packets; i++) {
						if (strcmp(sender_ip_str, 
									received_ip_addrs[i]) == 0) {
							duplicate_ip_addr = 1;
						}
					}
					if (duplicate_ip_addr > 0) {							
						received_ip_addrs[received_packets++] = "";
					}
					else {
						received_ip_addrs[received_packets++] = sender_ip_str;
					}
				}
			}
		}
	}
	return received_packets;
}

void print_less_than_3_packets(int received_packets, char** received_ip_addrs)
{
	char ip_addr_cat[64] = "";
	for (int i = 0; i < received_packets; i++) {
		strcat(ip_addr_cat, received_ip_addrs[i]);
	}
	printf ("%s ???\n", ip_addr_cat);
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
	char ip_addr_cat[64] = "";
	suseconds_t avg_time = get_average_time(send_time, received_time);
	for (int i = 0; i < 3; i++) {
		strcat(ip_addr_cat, received_ip_addrs[i]);
	}
	printf ("%s %ldms\n", ip_addr_cat, avg_time);
}