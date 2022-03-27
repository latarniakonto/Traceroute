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