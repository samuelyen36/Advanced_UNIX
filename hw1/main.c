#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h>

void traverse_proc_pid();

int main(int argc, char **argv){
	int opt=0;
	int long_index=0;
	char *filtering_stirng;
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

	traverse_proc_pid();

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
			printf("%d\t%s\n",(int)direntp->d_ino,direntp->d_name);
		}
		closedir(dir_ptr);
	}
}