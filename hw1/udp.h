#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <inttypes.h>
//#include "tcp.c"

void read_net_udp_v4(int socket);	//print the entry that matches the socket number in /proc/net/udp
void read_net_udp_v6(int socket);   //print the entry that matches the socket number in /proc/net/udp6
void big_endian_store_udp(char *socket_ipv6, char *readable_ipv6);