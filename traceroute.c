#include "traceroute_essentials.h"


int main (int argc, char** argv)
{	
	if (argc == 1) {
		fprintf(stderr, "arguments error: wrong number of arguments\n");
		return EXIT_FAILURE;
	}	
	char* ipv4_addr = argv[1];
	if (valid_IPv4_check(ipv4_addr) == -1) {
		fprintf(stderr, "arguments error: argument is not a valid IPv4\n");
		return EXIT_FAILURE;
	}
	
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	pid_t pid = getpid();
	uint8_t counter = 0;

	struct sockaddr_in recipient;
	bzero(&recipient, sizeof(recipient));
	recipient.sin_family = AF_INET;
	int convert_IPv4_addr = inet_pton(AF_INET, ipv4_addr, &recipient.sin_addr);
	if (convert_IPv4_addr <= 0) {
		if (convert_IPv4_addr == 0) {
			fprintf(stderr, "inet_pton error: ip adress is in wrong format\n");
		}
		else {
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
		if (send_packets(&send_time, sockfd, pid, &counter, recipient) < 0) {
			return EXIT_FAILURE;
		}
		
		int received_packets = 0;
		char* received_ip_addrs[3];
		int timeout = 1;
		suseconds_t average_time = 0;
		struct timeval received_time;
		struct timeval diff;
		for (;;) {
			fd_set readfd;
			FD_ZERO(&readfd);
			FD_SET(sockfd, &readfd);
			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			int ready = select(sockfd + 1, &readfd, NULL, NULL, &tv);
			if (ready < 0) {
				fprintf(stderr, "select error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
			else if (ready == 0) {				
				timeout = 0;
				break;
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
						return EXIT_FAILURE;
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
						gettimeofday(&received_time, NULL);
						timersub(&received_time, &send_time, &diff);
						average_time += diff.tv_usec;						
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
			if (received_packets == 3)
				break;
		}
		if (timeout == 0) {
			printf("*\n");
		}
		else {
			if (received_packets == 3) {
				average_time /= 3000;
				char ip_addr_cat[64] = "";
				for (int i = 0; i < received_packets; i++) {
					strcat(ip_addr_cat, received_ip_addrs[i]);					
				}
				printf ("%s %ldms\n", ip_addr_cat, average_time);
			}
			else {
				char ip_addr_cat[64] = "";
				for (int i = 0; i < received_packets; i++) {
					strcat(ip_addr_cat, received_ip_addrs[i]);					
				}
				printf ("%s ???\n", ip_addr_cat);
			}
			int at_final_router = -1;
			for(int i = 0; i < received_packets; i++) {				
				if (strcmp(received_ip_addrs[i], ipv4_addr) == 0)
					at_final_router = 1;
			}
			if (at_final_router > 0)
				break;
		}
	}

	return EXIT_SUCCESS;
}