#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <inttypes.h>

void read_net_udp_v4();	
void read_net_udp_v6();   
void big_endian_store_udp(char *socket_ipv6, char *readable_ipv6);

