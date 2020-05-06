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
    char cmd[MAX_CMD_SIZE],cmd_res[MAX_CMD_SIZE];
    int idx_arg_arr=0;
    int __flag=0;       //for detecting --

    char *so_path=NULL; //if the path to shared lib is given, store at here
    char *base_dir=NULL;    //if the allowed path  is given, store at here
    int d_flag=0,p_flag=0;      //check if -d, -p emerges
    memset(cmd,'\0',MAX_CMD_SIZE);
    memset(cmd_res,'\0',MAX_CMD_SIZE);

    for(int i=0;i<argc;i++){    //store arguments before --
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

    char *arg_require = "p:d:";
    int opt;
    while((opt=getopt(idx_arg_arr,arg_arr,arg_require))!=-1){
        switch(opt)
        {
            case 'p': 
                if(optarg==NULL){
                    printf("value of path needs to be given\n");
                    return 0;       //terminate main
                }
                p_flag=1;
                so_path = malloc(strlen(optarg));
                strcpy(so_path,optarg);
                printf("path to shared library: %s\n", so_path);
                break;  

            case 'd':   
                if(optarg==NULL){
                    printf("value of path needs to be given\n");
                    return 0;   //terminate main
                }
                d_flag=1;
                base_dir = malloc(strlen(optarg));
                strcpy(base_dir,optarg);
                printf("base directory: %s\n", base_dir);
                break;
            case '?':  
                //printf("invalid option: %c\n", optopt);
                printf("\nusage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n-p: set the path to sandbox.so, default = ./sandbox.so\n-d: the base directory that is allowed to access, default = .\n--: seperate the arguments for sandbox and for the executed command\n");
                return 0;   //terminate main
                break;  
        } 
    }


    if(__flag==0){      //program arguments has no -- and no error options, put all argument to cmd(to be execute)
        for(int i=1;i<argc;i++){
            if(strlen(cmd)==0){
                strcpy(cmd,argv[i]);
            }
            else{
                sprintf(cmd,"%s %s",cmd,argv[i]);
            }
        }
    }
    
    if(p_flag==1){  //concate with the location of shared lib
        sprintf(cmd_res,"LD_PRELOAD=\"%s\" %s",so_path,cmd);
    }
    if(p_flag==0){
        sprintf(cmd_res,"LD_PRELOAD=\"./sandbox.so\" %s",cmd);
    }
    if(d_flag==1){  //a specific path is given.
        printf("d_flag is on\n");
        char tmp_dir[64];
        sprintf(tmp_dir,"SELF_DIR=%s",base_dir);
        if(putenv(tmp_dir)==-1){    //put the given directory into environemnt variable
            printf("putenv went wrong\n");
        }
    }

    printf("cmd to be execute: %s\n",cmd_res);

    char *ptr;

    if((ptr=getenv("SELF_DIR"))!=NULL){
        printf("\nyeahyeahyeah, we get env %s\n",ptr);
    }

    ori_system(cmd_res);

    if(d_flag==1){      //delete dir environment variable
        if(putenv("SELF_DIR=")==-1){
            printf("putenv went wrong at deleting\n");
        }
        free(base_dir);
    }
    if(p_flag==1){
        free(so_path);
    }

    return 0;
}