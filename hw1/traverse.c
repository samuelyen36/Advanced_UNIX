#include "traverse.h"

void traverse_proc_pid(int socket){	//traverse through /process/pid/fd, and find the entry which meets the socket number
	//printf("entering traverse\n");
	const char *path_prefix = "/proc";
	DIR *dir_ptr;
	struct dirent *direntp;

	dir_ptr = opendir(path_prefix);
	if(dir_ptr == NULL){
		printf("Ls: can not open %s",path_prefix);
	}
	else {
		while(direntp = readdir(dir_ptr)){
			//printf("----------------------------\n");
			//printf("%d\t%s\n",(int)direntp->d_ino,direntp->d_name);		//enter /proc and get the pid directory
			if(is_target(direntp->d_name)){	//it's a pid directory
				char tmp[100];		//   /proc/$pid/fd
				strcpy(tmp,"/proc/");
				strcat(tmp,direntp->d_name);		//pid here
				strcat(tmp,"/fd");

				DIR *pid_dir_ptr;
				struct dirent *pid_direntp;
				pid_dir_ptr = opendir(tmp);	//   /proc/$pid/fd
				if(pid_dir_ptr == NULL){		//may occur permission denied
					//printf("Ls: can not open %s",tmp);
					//exit(0);
				}
				else{			//go through .../fd/
					while(pid_direntp = readdir(pid_dir_ptr)){
						//printf("enter readdir\n");
						char link_pdw[100];
						char buf[100];		//used for readlink buffer
						char num[10];
						strcpy(link_pdw,tmp);
						strcat(link_pdw,"/");
						strcat(link_pdw,pid_direntp->d_name);		//got the directory of symbolic link
						//printf("%s\n",link_pdw);
						//printf("%d\t%s\n",(int)pid_direntp->d_ino,pid_direntp->d_name);		//enter /proc and get the pid directory

						readlink(link_pdw,buf,sizeof(buf));		//read the symbolic link
						//printf("%s\n",buf);
						if(strncmp("socket",buf,6)==0){		//check if the first six character of soft link information is "socket"
							//printf("%s\n",buf);		//store info like socket:[733319]sr/share/code....
							//printf("%d\t",atoi(buf));
							memset(num,'\0',10);
							for(int i=8;;i++){
								if(buf[i]>='0' && buf[i]<='9'){
									num[i-8] = buf[i];
									continue;
								}
								else{
									break;
								}
							}
							//printf(" %s\t",buf);
							if(atoi(num)==socket){   /*do the check on socket number here*/
								char tmp_dir[50]="/proc/";
								strcat(tmp_dir,direntp->d_name);
								printf("%s/",direntp->d_name);
								print_process_information(tmp_dir);
							}
						}
					}
					closedir(pid_dir_ptr);
				}
				//printf("----------------------------\n");
			}
		}
		closedir(dir_ptr);
	}
}

int is_target(char *str){	//is number&&>1000 => return 1  / else=> return 0
	for(int i=0; i<strlen(str); i++){
		if(str[i]>'9' || str[i]<'0'){	//The string has a character which is not between 0<9
			if(str[i]!='\0'){	//if char is not a number and is not a space, then return 0, which stands for false
				return 0;
			}
		}
	}
	return 1;
}

void print_process_information(char *ps_directory){		//send /proc/$pid, print the content in /proc/$pid/comm
	strcat(ps_directory,"/comm");		//   /proc/pid/comm
	FILE *comm_fp;
	comm_fp = fopen(ps_directory,"r");
    
	char c = fgetc(comm_fp); 
    while (c != EOF) 
    { 
        if(c!='\n'){
			printf ("%c", c); 
		}
        c = fgetc(comm_fp); 
    } 

	//printf("\n");
	fclose(comm_fp);
	return;
}