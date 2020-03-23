#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include "traverse.h"


void read_net_tcp_v4();	//print the entry that matches the socket number in /proc/net/tcp
void read_net_tcp_v6();
void big_endian_store(char *socket_ipv6, char *readable_ipv6);