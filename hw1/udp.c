/*#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>*/
#include "udp.h"
#include "traverse.h"

void read_net_udp_v4(){
	FILE *fp;
	char inode_str[100]="\0";
	char trash[100];
	char local[20],des[20];
	char local_ip[9],des_ip[9];	//8char
	char local_port[5],des_port[5];		//4char
	char buf[100];
	char c;
	unsigned int int_local_ip, int_local_port, int_des_ip, int_des_port;
	char readable_local_ip[20], readable_des_ip[20];
	struct in_addr ipv4_local,ipv4_des;
	printf("List of UDP connections:\nProtocol%5sLocal Address%20sForeign Address%20sPID/Program name and arguments\n"," "," "," ");
	fp = fopen("/proc/net/udp","r");
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

		int_local_ip = (unsigned int)strtol(local_ip, NULL, 16);
    	int_local_port = (unsigned int)strtol(local_port, NULL, 16);

		int_des_ip = (unsigned int)strtol(des_ip, NULL, 16);
    	int_des_port = (unsigned int)strtol(des_port, NULL, 16);


		ipv4_local.s_addr = int_local_ip;
    	inet_ntop(AF_INET, &ipv4_local, readable_local_ip, INET_ADDRSTRLEN);
		ipv4_local.s_addr = int_des_ip;
    	inet_ntop(AF_INET, &ipv4_local, readable_des_ip, INET_ADDRSTRLEN);

		//printf("\t%s\t%d\n",readable_local_ip, int_local_port);		//result
		//printf("%s%10s%s","TCP"," ",readable_local_ip);	//TCP   local_ip
		if(int_local_port==0){
			sprintf(buf,"%s%10s%s:%s","UDP"," ",readable_local_ip,"*");
			printf("%-45s",buf);
			//printf("%s%10s%s:%s","TCP"," ",readable_local_ip,"*");
		}
		else{
			sprintf(buf,"%s%10s%s:%d","UDP"," ",readable_local_ip,int_local_port);
			printf("%-45s",buf);
			//printf("%s%10s%s:%d","TCP"," ",readable_local_ip,int_local_port);
		}

		if(int_des_port==0){
			sprintf(buf,"%s:%s",readable_des_ip,"*");
			printf("%-45s",buf);
			//printf("%s:%s",readable_des_ip,"*");
		}
		else{
			sprintf(buf,"%s:%d",readable_des_ip,int_des_port);
			printf("%-45s",buf);
			//printf("%s:%d",readable_des_ip,int_des_port);
		}

		if(atoi(inode_str)!=0){
			traverse_proc_pid(atoi(inode_str));
		}

		printf("\n");
		while(c=fgetc(fp) != '\n'){
			continue;
		}
	}
	
	return;
}

void read_net_udp_v6(){
	FILE *fp;
	char inode_str[100]="\0";
	char trash[100];
	char local[50],des[50];
	char local_ip[33],des_ip[33];	//33char
	char local_port[5],des_port[5];		//4char
	char buf[100];
	char c;
	unsigned int int_local_ip, int_local_port, int_des_ip, int_des_port;
	char readable_local_ip[40], readable_des_ip[40];
	struct in6_addr ipv6_local,ipv6_des;
	printf("List of UDP6 connections:\nProtocol%5sLocal Address%20sForeign Address%20sPID/Program name and arguments\n"," "," "," ");
	fp = fopen("/proc/net/udp6","r");
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
		memset(readable_local_ip,'\0',40);
		memset(readable_des_ip,'\0',40);

		for(int i=0;i<32;i++){
			local_ip[i]=local[i];
			des_ip[i]=des[i];
		}
		for(int i=0;i<4;i++){
			local_port[i] = local[i+33];
			des_port[i] = des[i+33];
		}

    	int_local_port = (unsigned int)strtol(local_port, NULL, 16);
		int_des_port = (unsigned int)strtol(des_port, NULL, 16);


		//ipv6_local.s6_addr = int_local_ip;
		big_endian_store_udp(local_ip, readable_local_ip);
		big_endian_store_udp(des_ip,readable_des_ip);

		if(int_local_port==0){
			sprintf(buf,"%s%8s%s:%s","UDP6"," ",readable_local_ip,"*");
			printf("%-45s",buf);
			//printf("%s%8s%s:%-34s\t","TCP6"," ",readable_local_ip,"*");
		}
		else{
			sprintf(buf,"%s%8s%s:%d","UDP6"," ",readable_local_ip,int_local_port);
			printf("%-45s",buf);
			//printf("%s%8s%s:%-34d\t","TCP6"," ",readable_local_ip,int_local_port);
		}

		if(int_des_port==0){
			sprintf(buf,"%s:%s",readable_des_ip,"*");
			printf("%-45s",buf);
			//printf("%s:%-34s\t",readable_des_ip,"*");
		}
		else{
			sprintf(buf,"%s:%d",readable_des_ip,int_des_port);
			printf("%-45s",buf);
			//printf("%s:%-34d\t",readable_des_ip,int_des_port);
		}

		if(atoi(inode_str)!=0){
			traverse_proc_pid(atoi(inode_str));
		}
		printf("\n");
		while(c=fgetc(fp) != '\n'){
			continue;
		}
	}
	
	return;

}

void big_endian_store_udp(char *socket_ipv6, char *readable_ipv6){
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

