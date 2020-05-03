#define _GNU_SOURCE
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

FILE *_fp;
char *empty = "";
int (*origin_fprintf)(FILE *, const char *, ...) = NULL;


void initial();
int chdir();
DIR *opendir();
int chmod();
int __chown();
int __creat ();

void initial(void){
    const char* env_var = getenv("MONITOR_OUTPUT");
    FILE* (*origin)(char const *, char const *) = NULL;
    origin = dlsym(RTLD_NEXT, "fopen");
    if(origin != NULL && env_var != NULL) {
        _fp = origin(env_var, "wt");
    }
    else _fp = stderr;
    origin_fprintf = dlsym(RTLD_NEXT, "fprintf");
    printf("addr to output device: %p\n",_fp);
    //origin_fprintf(_fp,"rewrite fprintf done\n");
    
    //printf("rewrite fprintf done: %p\n",stderr);
}

int check_escape_path(const char *buf1,const char *buf2){       //buf1 is current(absolute) and buf2 can be revalent(usually input of function)
    char tmp[64];
    realpath(buf2,tmp);
    return(strncmp(buf1,tmp,strlen(buf1)));
}

int chdir (const char *path)
{
    initial();
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    int return_val=-1;
    int (*origin)(const char *) = NULL;
    if(check_escape_path(buf,path)!=0){    //escape
        char *err_msg = "chdir: try to escape from sandbox\n";
        //write(STDERR_FILENO,err_msg,strlen(err_msg));
        fprintf(stderr,"chdir: nonono, try to escape from sandbox\n");
        //origin_fprintf(_fp,"chdir: nonono, try to escape from sandbox\n");
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "chdir");
    if(origin != NULL) {
        return_val = origin(path);
        printf( "# chdir(\"%s\") = %d\n", path, return_val);
    }
    else printf("chdir() error\n");
    printf("run our own version successfully\n");
    return return_val;
}

DIR *opendir (const char *path)
{
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    DIR* return_val=NULL;
    DIR* (*origin)(const char *) = NULL;
    if(check_escape_path(buf,path)!=0){    //escape
        char *err_msg = "opendir: try to escape from sandbox\n";
        write(STDERR_FILENO,err_msg,strlen(err_msg));
        //printf("opendir: nonono, try to escape from sandbox\n");
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "opendir");
    if(origin != NULL) {
        return_val = origin(path);
        printf( "# opendir(\"%s\") = %p\n", path, return_val);
    }
    else printf("opendir() error\n");
    printf("run our own version of opendir successfully\n");
    return return_val;
}

int chmod(const char *path, mode_t mode){
    int return_val=-1;
    int (*origin)(const char *,mode_t mode) = NULL;
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    if(check_escape_path(buf,path)!=0){    //escape
        char *err_msg = "chmod: try to escape from sandbox\n";
        write(STDERR_FILENO,err_msg,strlen(err_msg));
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "chmod");
    if(origin != NULL) {
        return_val = origin(path,mode);
        printf( "# chmod(\"%s\") = %d\n", path, return_val);
    }
    else printf("chdir() error\n");
    printf("run our own version of chmod successfully\n");
    return return_val;
}

int chown(const char *file, uid_t owner, gid_t group){        //think about how to handle the directory
    int return_val=-1;
    int (*origin)(const char *, uid_t owner, gid_t group) = NULL;
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    if(check_escape_path(buf,file)!=0){    //escape
        char *err_msg = "chmod: try to escape from sandbox\n";
        write(STDERR_FILENO,err_msg,strlen(err_msg));        
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "chown");
    if(origin != NULL) {
        return_val = origin(file,owner,group);
        printf( "# chown(\"%s\") = %d\n", file, return_val);
    }
    else printf("chown() error\n");
    //printf("run our own version of chown successfully\n");
    return return_val;
}

int creat (const char *file, mode_t mode){
    int return_val=-1;
    int (*origin)(const char *,mode_t mode) = NULL;
    if(strncmp(file,"./",2)==0 || strncmp(file,"../",3)==0 || file[0]=='/'){
        char buf[64];
        getcwd(buf, sizeof(buf));   //get current absolute path
        if(check_escape_path(buf,file)!=0){    //escape
            char *err_msg = "creat: try to escape from sandbox\n";
            write(STDERR_FILENO,err_msg,strlen(err_msg));        
            return return_val;
        }
    }
    origin = dlsym(RTLD_NEXT, "creat");
    if(origin != NULL) {
        return_val = origin(file,mode);
        printf( "# creat(\"%s\") = %d\n", file, return_val);
    }
    else printf("creat() error\n");
    printf("run our own version of creat successfully\n");
    return return_val;

}


int main(){
    return 0;
}