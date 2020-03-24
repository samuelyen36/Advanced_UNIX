#include "traverse.h"

void traverse_proc_pid(int socket,char *ip_detail){	//traverse through /process/pid/fd, and find the entry which meets the socket number
	//printf("entering traverse\n");
	const char *path_prefix = "/proc";
	DIR *dir_ptr;
	struct dirent *direntp;
	int res=1;
	int printed=0;

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
								if(filtering_string_flag==0){		//do not filter string
									printf("%s",ip_detail);
									printf("%s/",direntp->d_name);		//pid
									print_process_information(tmp_dir);
									print_process_parameter(tmp_dir);
									printf("\n");
									printed=1;
								}
								else{		//do have to check filtering string
									res=judge_format_string(tmp_dir);
									if(res==1){
										printf("%s",ip_detail);
										printf("%s/",direntp->d_name);		//pid
										print_process_information(tmp_dir);
										print_process_parameter(tmp_dir);
										printf("\n");
										printed=1;
									}
								}
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
	if(printed==0 && filtering_string_flag==0){		//no filtering and not printed above(no process matched)
		printf("%s\n",ip_detail);
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

int judge_format_string(char *ps_directory){
	char *file_character=malloc(FILE_LENGTH);
	char tmp[50];
	regex_t reg;
	memset(tmp,'\0',50);
	strcpy(tmp,ps_directory);
	strcat(tmp,"/comm");
	FILE *comm_fp;
	FILE *cmdline_fp;
	char c;
	char *ptr_strstr=NULL;
	int status;
	regmatch_t pmatch[1];

	int z=regcomp(&reg,filtering_string_content,REG_EXTENDED);
	if(z!=0){
		char ebuf[20];
		regerror(z,&reg,ebuf,sizeof(ebuf));
		printf("error compiling regular expression: %s\n",ebuf);
		exit(0);
	}		//regular expression
	memset(file_character,'\0',FILE_LENGTH);
	comm_fp = fopen(tmp,"r");

	while((c=fgetc(comm_fp))!=EOF){
		file_character[strlen(file_character)]=c;
	}

	status = regexec(&reg,file_character,(size_t)1,pmatch,0);
	if(status == REG_NOMATCH){		//RE no match
		//printf("no match\n");
		
	}
	if(status==0){		//RE match
		//printf("matched\n");
		return 1;
	}

	/*ptr_strstr = strstr(file_character,filtering_string_content);
	if(ptr_strstr!=NULL){
		return 1;
	}*/

	ptr_strstr=NULL;
	fclose(comm_fp);
	memset(file_character,'\0',FILE_LENGTH);
	memset(tmp,'\0',50);
	strcpy(tmp,ps_directory);
	strcat(tmp,"/cmdline");

	cmdline_fp = fopen(tmp,"r");
	while((c=fgetc(cmdline_fp))!=EOF){
		file_character[strlen(file_character)]=c;
	}
	status = regexec(&reg,file_character,(size_t)1,pmatch,0);
	if(status == REG_NOMATCH){		//RE no match
		//printf("no match\n");
	}
	if(status==0){		//RE match
		//printf("matched\n");
		return 1;
	}

	/*ptr_strstr = strstr(file_character,filtering_string_content);
	if(ptr_strstr!=NULL){
		return 1;
	}*/
	return 0;
}


void print_process_information(char *ps_directory){		//send /proc/$pid, print the content in /proc/$pid/comm
	char tmp[50];
	strcpy(tmp,ps_directory);
	strcat(tmp,"/comm");		//   /proc/pid/comm
	FILE *comm_fp;
	comm_fp = fopen(tmp,"r");
    
	char c = fgetc(comm_fp); 
    while (c != EOF) 
    { 
        if(c!='\n'){
			printf ("%c", c); 
		}
        c = fgetc(comm_fp); 
    } 

	fclose(comm_fp);
	return;
}

void print_process_parameter(char *ps_directory){		//send /proc/$pid, print the content in /proc/$pid/comm
	char tmp[50];
	strcpy(tmp,ps_directory);
	strcat(tmp,"/cmdline");		//   /proc/pid/comm
	FILE *cmdline_fp;
	cmdline_fp = fopen(tmp,"r");
	char c = fgetc(cmdline_fp);

	while(c!=' '){		//discard the first part, which is the same as /proc/$pid/comm
		c = fgetc(cmdline_fp);
		if(c==EOF){
			return;
		}
	}
	printf(" ");
	while(1){
		c=fgetc(cmdline_fp);
		if(c!=EOF && c!='\n'){
			printf("%c",c);
		}
		else{
			break;
		}
	}
	return;
}