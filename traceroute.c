#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);
}

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
		return EXIT_FAILURE;
	}
	uint8_t counter = 0;
	for (int ttl = 1; ttl <= 30; ttl++) {
		// printf("ttl: %d\n", ttl);
		if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0) {
			fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		for (int packet = 1; packet <= 3; packet++) {
			struct icmp header;
			header.icmp_type = ICMP_ECHO;
			header.icmp_code = 0;
			header.icmp_hun.ih_idseq.icd_id = getpid();
			header.icmp_hun.ih_idseq.icd_seq = counter++;
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
				return EXIT_FAILURE;
			}
		}		
		int received_packets = 0;
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
					if (icmp_header->icmp_hun.ih_idseq.icd_id == getpid() &&
						icmp_header->icmp_hun.ih_idseq.icd_seq < ttl * 3 &&
						icmp_header->icmp_hun.ih_idseq.icd_seq >= (ttl * 3) - 3) {
						received_packets++;
						// printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);
						}
					
					// printf("%d\n", icmp_header->icmp_type);
				}			
			}
			if (received_packets == 3)
				break;
		}
	}

    //walidacja argumentu/adresu ip do tracerouta
    //for(i=1;i<=30;i++)
    //send_icmp(i);send_icmp(i);send_icmp(i);
    //recvfrom(socket);recvfrom(socket);recvfrom(socket);
    //przetwarzanie odpowiedzi z routerów
    //formatowanie odpowiedzi z routerów
    //wypisywanie odpowiedzi z routerów
    //break, gdy otrzymujemy odpowiedź od komputera docelowego


	// for (;;) {
		
	// 	struct sockaddr_in 	sender;	
	// 	socklen_t 			sender_len = sizeof(sender);
	// 	u_int8_t 			buffer[IP_MAXPACKET];

	// 	ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
	// 	if (packet_len < 0) {
	// 		fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
	// 		return EXIT_FAILURE;
	// 	}

	// 	char sender_ip_str[20]; 
	// 	inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		// printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);

	// 	struct ip* 			ip_header = (struct ip*) buffer;
	// 	ssize_t				ip_header_len = 4 * ip_header->ip_hl;

	// 	printf ("IP header: "); 
	// 	print_as_bytes (buffer, ip_header_len);
	// 	printf("\n");

	// 	printf ("IP data:   ");
	// 	print_as_bytes (buffer + ip_header_len, packet_len - ip_header_len);
	// 	printf("\n\n");
	// }

	return EXIT_SUCCESS;
}