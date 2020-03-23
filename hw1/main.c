#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>

#include "tcp.h"
#include "udp.h"
#include "traverse.h"
//void read_net_tcp(int socket);	//print the entry that matches the socket number in /proc/net/tcp

extern char filtering_string_content[50];
extern int filtering_string_flag;

//char *convert_ipv4(char *socket_address);

int main(int argc, char **argv){
	int opt=0;
	int long_index=0;
	short tcp_opt=0,udp_opt=0;
	static struct option long_options[] = {
    	{"tcp", no_argument, 0, 't' },
        {"udp", no_argument, 0, 'u' },
        {0,0,0,0}
    };

	while ((opt = getopt_long(argc, argv,"tu",long_options, &long_index )) != -1) {	//handling -t/-u/--tcp/--udp
        switch (opt) {
            case 't' : 
			 	printf("List only tcp options\n");
				tcp_opt=1;
                break;
            case 'u' :
			 	printf("list only UDP options\n");
				udp_opt=1;
                break;
			case '?':
				printf("unknown_options, exit from the program\n");
				return 0;
				break;
            default:  
				printf("\n");
				break;
        }
    }

	filtering_string_flag=0;
	for(int i=1; i<argc; i++){
		if(argv[i][0]!='-'){	//filtering string, which is not an option
			strcpy(filtering_string_content,argv[i]);
			printf("get the filtering string %s\n",filtering_string_content);
			filtering_string_flag=1;
			break;
		}
	}

	if(udp_opt==1 && tcp_opt==0){		//only udp
		read_net_udp_v4();
		read_net_udp_v6();
	}
	else if(udp_opt==0 && tcp_opt==1){		//only tcp
		read_net_tcp_v4();
		read_net_tcp_v6();
	}
	else{		//both of them   00/11
		read_net_tcp_v4();
		read_net_tcp_v6();
		read_net_udp_v4();
		read_net_udp_v6();
	}


	/*read_net_tcp_v4();
	read_net_udp_v4();*/


	return 0;
}
