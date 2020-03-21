/*#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>*/
#include "tcp.h"

void read_net_tcp_v4(int socket){
	FILE *fp;
	char inode_str[100]="\0";
	char trash[100];
	char local[20],des[20];
	char local_ip[9],des_ip[9];	//8char
	char local_port[5],des_port[5];		//4char
	char c;
	int tcp_inode;
	unsigned int int_local_ip, int_local_port, int_des_ip, int_des_port;
	char readable_local_ip[20], readable_des_ip[20];
	struct in_addr ipv4_local,ipv4_des;
	fp = fopen("/proc/net/tcp","r");
	fscanf(fp,"%s %s %s %s %s %s %s %s %s %s %s %s",trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash);		//first row, which is comment;
	while(c=fgetc(fp) != '\n'){
			continue;
	}
	while(1){	//loca ip/port      remove ip/port    inode
		c = fgetc(fp);		//we don't need the first character in each row, so use it to test if EOF occurs
		if(c==EOF){	
			break;
		}
		fscanf(fp,"%s %s %s %s %s %s %s %s %s %s",trash,local,des,trash,trash,trash,trash,trash,trash,inode_str);
		memset(local_ip,'\0',9);
		memset(local_port,'\0',5);
		memset(des_ip,'\0',9);
		memset(des_port,'\0',5);

		for(int i=0;i<8;i++){
			local_ip[i]=local[i];
			des_ip[i]=des[i];
		}
		for(int i=0;i<4;i++){
			local_port[i] = local[i+9];
			des_port[i] = des[i+9];
		}

		printf("%s\t%s\t%s\t%s\t%s",local_ip,local_port,des_ip,des_port,inode_str);
		int_local_ip = (unsigned int)strtol(local_ip, NULL, 16);
    	int_local_port = (unsigned int)strtol(local_port, NULL, 16);

		ipv4_local.s_addr = int_local_ip;
    	inet_ntop(AF_INET, &ipv4_local, readable_local_ip, INET_ADDRSTRLEN);
		printf("\t%s\t%d\n",readable_local_ip, int_local_port);		//result
		


		while(c=fgetc(fp) != '\n'){
			continue;
		}
	}
	
	return;
}

void read_net_tcp_v6(int socket){
	FILE *fp;
	char inode_str[100]="\0";
	char trash[100];
	char local[50],des[50];
	char local_ip[33],des_ip[33];	//40char
	char local_port[5],des_port[5];		//4char
	char c;
	int tcp_inode;
	unsigned int int_local_ip, int_local_port, int_des_ip, int_des_port;
	char readable_local_ip[30], readable_des_ip[30];
	struct in6_addr ipv6_local,ipv6_des;
	fp = fopen("/proc/net/tcp6","r");
	fscanf(fp,"%s %s %s %s %s %s %s %s %s %s %s %s",trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash);		//first row, which is comment;
	while(c=fgetc(fp) != '\n'){
			continue;
	}
	while(1){	//loca ip/port      remove ip/port    inode
		c = fgetc(fp);		//we don't need the first character in each row, so use it to test if EOF occurs
		if(c==EOF){	
			break;
		}
		fscanf(fp,"%s %s %s %s %s %s %s %s %s %s",trash,local,des,trash,trash,trash,trash,trash,trash,inode_str);
		memset(local_ip,'\0',33);
		memset(local_port,'\0',5);
		memset(des_ip,'\0',33);
		memset(des_port,'\0',5);

		for(int i=0;i<32;i++){
			local_ip[i]=local[i];
			des_ip[i]=des[i];
		}
		for(int i=0;i<4;i++){
			local_port[i] = local[i+33];
			des_port[i] = des[i+33];
		}

		//printf("%s\t%s\t%s\n",local,des,inode_str);
		printf("%s\t%s\t%s\t%s\t%s",local_ip,local_port,des_ip,des_port,inode_str);
		//int_local_ip = (unsigned int)strtol(local_ip, NULL, 16);
    	int_local_port = (unsigned int)strtol(local_port, NULL, 16);



		//ipv6_local.s6_addr = int_local_ip;
		big_endian_store(local_ip, readable_local_ip);
    	//inet_ntop(AF_INET6, &ipv6_local, readable_local_ip, INET_ADDRSTRLEN);
		printf("\treadable format: %s\t%d\n",readable_local_ip, int_local_port);		//result
		
		while(c=fgetc(fp) != '\n'){
			continue;
		}
	}
	
	return;

}

void big_endian_store(char *socket_ipv6, char *readable_ipv6){
	struct sockaddr_in6 rem_info;
	struct in6_addr sin6_addr;
	for (int i = 0 ; i < 16 ; i++) {
		sscanf(socket_ipv6+2*(15 - i), "%2" SCNx8, &rem_info.sin6_addr.s6_addr[i]);
	}
	for (int i = 0 ; i < 4 ; i++) {
		for (int j = 0;j < 4;j++) {
			sin6_addr.s6_addr[i*4+j] = rem_info.sin6_addr.s6_addr[4*(3 - i) + j];
		}
	}
	inet_ntop(AF_INET6, &(sin6_addr), readable_ipv6, INET6_ADDRSTRLEN);
	return;
}