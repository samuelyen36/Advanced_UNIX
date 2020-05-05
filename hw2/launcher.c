#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#define MAX_CMD_SIZE 128

/* gcc launcher.c -ldl -o launcher */

int ori_system(const char * cmd){
    int return_val=-1;
    int (*origin)(const char *) = NULL;

    origin = dlsym(RTLD_NEXT, "system");
    if(origin != NULL) {
        return_val = origin(cmd);
    }
    else printf("system() error in launcher\n");
}


int main(int argc, char *argv[]){
    char *arg_arr[argc];    //store string up to number of argc (in order to parse before --)
    char cmd[MAX_CMD_SIZE];
    int idx_arg_arr=0;
    int __flag=0;

    char *so_path=NULL;
    char *base_dir=NULL;
    memset(cmd,'\0',MAX_CMD_SIZE);

    

    for(int i=0;i<argc;i++){    //discard i=0 which is filename
        if(strcmp("--", argv[i])==0){        //parse before --
        __flag=1;
        continue;
        }
        if(__flag==0){
            arg_arr[idx_arg_arr]=argv[i];
            idx_arg_arr++;
        }
        if(__flag==1){
            if(strlen(cmd)==0){
                strcpy(cmd,argv[i]);
            }
            else{
                sprintf(cmd,"%s %s",cmd,argv[i]);
            }
        }
    }
    printf("cmd to be execute is:%s\n\n\n",cmd);


    char *arg_require = "p:d:";
    int opt;
    while((opt=getopt(idx_arg_arr,arg_arr,arg_require))!=-1){
        switch(opt)
        {  
            case 'p': 
                if(optarg==NULL){
                    printf("value of path needs to be given\n");
                    return 0;
                }
                printf("path to shared library: %s\n", optarg);
                strcpy(so_path,optarg);
                break;  

            case 'd':   
                if(optarg==NULL){
                    printf("value of path needs to be given\n");
                    return 0;
                }
                printf("base directory: %s\n", optarg); 
                strcpy(base_dir,optarg);
                break;
            case '?':  
                printf("unknown option: %c\n", optopt); 
                return 0;
                break;  
        } 
    }

    //ori_system(cmd);
    return 0;
}