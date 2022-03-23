#include <netinet/ip.h>
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

int main (int argc, char** argv)
{	
    //walidacja argumentu/adresu ip do tracerouta
    //for(i=1;i<=30;i++)
    //send_icmp(i);send_icmp(i);send_icmp(i);
    //recvfrom(socket);recvfrom(socket);recvfrom(socket);
    //przetwarzanie odpowiedzi z routerów
    //formatowanie odpowiedzi z routerów
    //wypisywanie odpowiedzi z routerów
    //break, gdy otrzymujemy odpowiedź od komputera docelowego

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	for (;;) {
		
		struct sockaddr_in 	sender;	
		socklen_t 			sender_len = sizeof(sender);
		u_int8_t 			buffer[IP_MAXPACKET];

		ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
		if (packet_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			return EXIT_FAILURE;
		}

		char sender_ip_str[20]; 
		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);

		struct ip* 			ip_header = (struct ip*) buffer;
		ssize_t				ip_header_len = 4 * ip_header->ip_hl;

		printf ("IP header: "); 
		print_as_bytes (buffer, ip_header_len);
		printf("\n");

		printf ("IP data:   ");
		print_as_bytes (buffer + ip_header_len, packet_len - ip_header_len);
		printf("\n\n");
	}

	return EXIT_SUCCESS;
}