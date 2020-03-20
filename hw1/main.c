#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>

void traverse_proc_pid();	//read the directory(file) name in /proc directory, which stands for the pid of processes
int is_target(char *str);	//check if the string is actually the thing we want(pid>1000).
void read_net_tcp(int socket);	//print the entry that matches the socket number in /proc/net/tcp


char *convert_ipv4(char *socket_address);

int main(int argc, char **argv){
	int opt=0;
	int long_index=0;
	char *filtering_stirng=NULL;
	static struct option long_options[] = {
    	{"tcp", no_argument, 0, 't' },
        {"udp", no_argument, 0, 'u' },
        {0,0,0,0}
    };

	while ((opt = getopt_long(argc, argv,"tu",long_options, &long_index )) != -1) {	//handling -t/-u/--tcp/--udp
        switch (opt) {
            case 't' : 
			 	printf("List only tcp options\n");
                break;
            case 'u' :
			 	printf("list only UDP options\n");
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

	for(int i=1; i<argc; i++){
		if(argv[i][0]!='-'){	//filtering string, which is not an option
			filtering_stirng = malloc(sizeof(argv[i])+1);
			strcpy(filtering_stirng,argv[i]);
			printf("get the filtering string %s\n",filtering_stirng);
			break;
		}
	}

	//traverse_proc_pid();
	read_net_tcp(1234);

	if(!filtering_stirng){
		free(filtering_stirng);
	}
	return 0;
}

void traverse_proc_pid(void){
	printf("entering traverse\n");
	const char *path_prefix = "/proc";
	DIR *dir_ptr;
	struct dirent *direntp;

	dir_ptr = opendir(path_prefix);
	if(dir_ptr == NULL){
		printf("Ls: can not open %s",path_prefix);
	}
	else {
		while(direntp = readdir(dir_ptr)){
			printf("----------------------------\n");
			printf("%d\t%s\n",(int)direntp->d_ino,direntp->d_name);		//enter /proc and get the pid directory
			if(is_target(direntp->d_name)){	//it's a pid directory
				char tmp[100];		//   /proc/$pid/fd
				strcpy(tmp,"/proc/");
				strcat(tmp,direntp->d_name);
				strcat(tmp,"/fd");

				DIR *pid_dir_ptr;
				struct dirent *pid_direntp;
				pid_dir_ptr = opendir(tmp);
				if(pid_dir_ptr == NULL){
					printf("Ls: can not open %s",tmp);
					exit(0);
				}
				else{			//go through .../fd/
					while(pid_direntp = readdir(pid_dir_ptr)){
						char link_pdw[100];
						char buf[100];		//used for readlink buffer
						strcpy(link_pdw,tmp);
						strcat(link_pdw,"/");
						strcat(link_pdw,pid_direntp->d_name);		//got the directory of symbolic link
						printf("%s\n",link_pdw);
						//printf("%d\t%s\n",(int)pid_direntp->d_ino,pid_direntp->d_name);		//enter /proc and get the pid directory

						readlink(link_pdw,buf,sizeof(buf));		//read the symbolic link
						//printf("%s\n",buf);
						if(strncmp("socket",buf,6)==0){		//check if the first six character is "socket"
							printf("%s\n",buf);
						}
					}
				}
				printf("----------------------------\n");
			}
		}
		closedir(dir_ptr);
	}
}

int is_target(char *str){	//is number&&>1000 => return 1  / else=> return 0
	for(int i=0; i<strlen(str); i++){
		if(str[i]>'9' || str[i]<'0'){	//The string has a character which is not between 0<9
			if(str[i]!='\0'){
				return 0;
			}
		}
	}
	int num=atoi(str);
	if(num>1000){
		return 1;
	}
	else{
		return 1;		//temporary return both 1, modified later if needed
	}
}

void read_net_tcp(int socket){
	FILE *fp;
	char inode_str[100]="\0";
	char trash[100];
	char local_ip[10],des_ip[10];
	char local_port[5],des_port[5];
	char tmp[100];
	char c;
	int tcp_inode;
	fp = fopen("/proc/net/tcp","r");
	fscanf(fp,"%s %s %s %s %s %s %s %s %s %s %s %s",trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash);		//first row, which is comment;
	while(c=fgetc(fp) != '\n'){
			continue;
	}
	while(1){
		for(int i=0;i<6;i++){
			c=fgetc(fp);
			if(c==EOF){
				return;
			}
		}
		memset(local_ip,'\0',10);		//get local ip and local port
		for(int i=0;i<8;i++){
			local_ip[i]=fgetc(fp);	
		}
		c=fgetc(fp); //seperating:
		memset(local_port,'\0',5);
		for(int i=0;i<4;i++){
			local_port[i]=fgetc(fp);
		}

		c=fgetc(fp); //seperating space

		memset(des_ip,'\0',10);
		for(int i=0;i<8;i++){
			des_ip[i]=fgetc(fp);	
		}
		memset(des_port,'\0',5);
		for(int i=0;i<4;i++){
			des_port[i]=fgetc(fp);
		}


		for(int i=0;i<58;i++){
			c=fgetc(fp);
			if(c==EOF){
				return;
			}
		}

		fscanf(fp,"%d",&tcp_inode);
		printf("%s\t%s\t%d\n",local_ip,des_ip,tcp_inode);
		while(c=fgetc(fp) != '\n'){
			continue;
		}
	}

	
	//printf("%s",trash);
	/*while(fscanf(fp,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",trash,trash,trash,trash,trash,trash,trash,trash,trash,inode_str,trash,trash,trash,trash,trash,trash,trash)){
		if(atoi(inode_str)==0){
			printf("going to break with inode_str contains %s\n",inode_str);
			break;
		}
		printf("%d\n",atoi(inode_str));
	}*/
	return;
}


char *convert_ipv4(char *socket_address){
}