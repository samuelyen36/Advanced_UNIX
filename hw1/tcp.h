#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>
//#include "tcp.c"

void read_net_tcp(int socket);	//print the entry that matches the socket number in /proc/net/tcp
