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
#define MAX_PATH_SIZE 128

char *empty = "";

void write_to_terminal();

void initial();
int chdir();
int chmod();
int chown();
int creat ();
FILE *fopen();
int link();
int mkdir();
int open(const char *file, int oflag, ...);
int openat (int fd, const char *file, int oflag, ...);   //????, be aware of it., jump first
DIR *opendir();
ssize_t readlink();
int rename();
int remove ();
int rmdir();
int __xstat ();
int __lxstat ();
int symlink();
int unlink();
/*-----------------always reject the following parts--------------*/
int execl (const char *path, const char *arg, ...); 
int execle (const char *path, const char *arg, ...);
int execlp (const char *file, const char *arg, ...);
int execv (const char *path, char *const argv[]);
int execve (const char *path, char *const argv[], char *const envp[]);
int execvp (const char *file, char *const argv[]);
int system (const char *line);
/*/*-----------------always reject the above parts--------------*/

//[sandbox] ??: access to %s is not allowed\n

void write_to_terminal(char *buf){  //write directly to terminal, which means that it wouldn't be redirected
    ssize_t return_write=-1;
    int (*origin_open)(const char *,int,...) = NULL;
    int terminal_fd;
    ssize_t (*origin_write)(int fd, const void *buf, size_t nbytes);
    origin_open = dlsym(RTLD_NEXT, "open");
    origin_write = dlsym(RTLD_NEXT, "write");
    if(terminal_fd = origin_open("/dev/tty",O_RDWR)<0){
        printf("open terminal error\n");
    }
    return_write = origin_write(terminal_fd,buf,strlen(buf));
    return;
    //const char *file, int oflag,
    //int fd, const void *buf, size_t nbytes
}

int check_escape_path(const char *buf1,const char *buf2){       //buf1 is current(absolute) and buf2 can be relative(usually input of function)
    char tmp[128];
    char abs_self_dir[128];
    memset(tmp,'\0',128);
    memset(abs_self_dir,'\0',128);
    char *dir_from_env;
    if((dir_from_env=getenv("SELF_DIR")) != NULL ){ //-d flag is set
        realpath(dir_from_env,abs_self_dir);    //real path of dir_from_env
        //printf("got env dir with its absolute path: %s\n",abs_self_dir);
    }
    realpath(buf2,tmp); //convert relative path to absolute path of accessing address


    if(dir_from_env!=NULL){ //return comparison result of dir_self
        return(strncmp(abs_self_dir,tmp,strlen(abs_self_dir)));
    }
    else{
        return(strncmp(buf1,tmp,strlen(buf1)));
    }
}

int chdir (const char *path){
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    int return_val=-1;
    int (*origin)(const char *) = NULL;
    if(check_escape_path(buf,path)!=0){    //escape
        char tmp[128];
        sprintf(tmp,"[sandbox] chdir: access to %s is not allowed\n",path);
        write_to_terminal(tmp);
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "chdir");
    if(origin != NULL) {
        return_val = origin(path);
    }
    else printf("chdir() error\n");
    return return_val;
}

DIR *opendir (const char *path){
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    DIR* return_val=NULL;
    DIR* (*origin)(const char *) = NULL;
    if(check_escape_path(buf,path)!=0){    //escape
        char err_msg[128];
        memset(err_msg,'\0',128);
        sprintf(err_msg,"[sandbox] opendir: access to %s is not allowed\n",path);
        write_to_terminal(err_msg);
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "opendir");
    if(origin != NULL) {
        return_val = origin(path);
    }
    else printf("opendir() error\n");
    return return_val;
}

int chmod(const char *path, mode_t mode){
    int return_val=-1;
    int (*origin)(const char *,mode_t mode) = NULL;
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    if(check_escape_path(buf,path)!=0){    //escape
        char err_msg[128];
        sprintf(err_msg,"[sandbox] chmod: access to %s is not allowed\n",path);
        write_to_terminal(err_msg);
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "chmod");
    if(origin != NULL) {
        return_val = origin(path,mode);
        //printf( "# chmod(\"%s\") = %d\n", path, return_val);
    }
    else printf("chmod() error\n");
    return return_val;
}

int chown(const char *file, uid_t owner, gid_t group){        //think about how to handle the directory
    int return_val=-1;
    int (*origin)(const char *, uid_t owner, gid_t group) = NULL;
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    if(check_escape_path(buf,file)!=0){    //escape
        char err_msg[128];
        memset(err_msg,'\0',128);
        sprintf(err_msg,"[sandbox] chown: access to %s is not allowed\n",file);
        write_to_terminal(err_msg);
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "chown");
    if(origin != NULL) {
        return_val = origin(file,owner,group);
    }
    else printf("chown() error\n");
    return return_val;
}

int creat (const char *file, mode_t mode){
    int return_val=-1;
    int (*origin)(const char *,mode_t mode) = NULL;
    if(strncmp(file,"./",2)==0 || strncmp(file,"../",3)==0 || file[0]=='/' || strcmp(file,".")==0 || strcmp(file,"..")==0){    //file is related to directory
        char buf[64];
        getcwd(buf, sizeof(buf));   //get current absolute path
        if(check_escape_path(buf,file)!=0){    //escape
            char err_msg[128];
            memset(err_msg,'\0',128);
            sprintf(err_msg,"[sandbox] creat: access to %s is not allowed\n",file);
            write_to_terminal(err_msg);
            return return_val;
        }
    }
    origin = dlsym(RTLD_NEXT, "creat");
    if(origin != NULL) {
        return_val = origin(file,mode);
        //printf( "# creat(\"%s\") = %d\n", file, return_val);
    }
    else printf("creat() error\n");
    return return_val;

}

FILE *fopen(const char * __filename, const char * __modes){
    FILE *return_val=NULL;
    FILE* (*origin)(const char *,const char* __modes) = NULL;
    if(strncmp(__filename,"./",2)==0 || strncmp(__filename,"../",3)==0 || __filename[0]=='/' || strcmp(__filename,".")==0 || strcmp(__filename,"..")==0){
        char buf[64];
        getcwd(buf, sizeof(buf));   //get current absolute path
        if(check_escape_path(buf,__filename)!=0){    //escape
            char *err_msg = "[sandbox] fopen: access to";
            char out_buf[128];
            sprintf(out_buf,"%s %s is not allowed\n",err_msg,__filename);
            write_to_terminal(out_buf);
            //write(STDERR_FILENO,out_buf,strlen(out_buf));        
            return return_val;
        }
    }
    origin = dlsym(RTLD_NEXT, "fopen");
    if(origin != NULL) {
        return_val = origin(__filename,__modes);
        //printf( "# fopen(\"%s\") \n", __filename);
    }
    else printf("fopen() error\n");
    return return_val;
}

int link(const char *oldpath, const char *newpath) {
    int (*origin)(const char *, const char *) = NULL;
    int return_val = -1;
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    
    if(strncmp(oldpath,"./",2)==0 || strncmp(oldpath,"../",3)==0 || oldpath[0]=='/' || strcmp(oldpath,".")==0 || strcmp(oldpath,"..")==0){
        if(check_escape_path(buf,oldpath)!=0 ){
                char *err_msg = "[sandbox] link: access to: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s is not allowed\n",err_msg,oldpath);
                write_to_terminal(out_buf);
                //write(STDERR_FILENO,out_buf,strlen(out_buf));        
                return return_val;
        }
    }
    if(strncmp(newpath,"./",2)==0 || strncmp(newpath,"../",3)==0 || newpath[0]=='/' || strcmp(newpath,".")==0 || strcmp(newpath,"..")==0 ){
        if(check_escape_path(buf,newpath)!=0 ){
                char *err_msg = "[sandbox] link: access to: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s is not allowed\n",err_msg,newpath);
                write_to_terminal(out_buf);
                //write(STDERR_FILENO,out_buf,strlen(out_buf));        
                return return_val;
        }
    }
    origin = dlsym(RTLD_NEXT, "link");
    if(origin != NULL) {
        return_val = origin(oldpath, newpath);
    }
    else write(STDOUT_FILENO,"link error\n",strlen("link error\n"));
    return return_val;
}

int mkdir(const char *path, mode_t mode){
    int (*origin)(const char *, mode_t) = NULL;
    int return_val = -1;
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    
    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/' || strcmp(path,".")==0 || strcmp(path,"..")==0){
        if(check_escape_path(buf,path)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] mkdir: access to %s is not allowed\n",path);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "mkdir");
    if(origin != NULL) {
        return_val = origin(path, mode);
    }
    else write(STDOUT_FILENO,"mkdir error\n",strlen("mkdir error\n"));
    return return_val;
}

int open(const char *file, int oflag, ...){
    int (*origin)(const char *, int, ...) = NULL;
    int return_val = -1;
    va_list args;
    va_start(args, oflag);
    mode_t mode = va_arg(args, mode_t);
    va_end(args);
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    
    if(strncmp(file,"./",2)==0 || strncmp(file,"../",3)==0 || file[0]=='/' || strcmp(file,".")==0 || strcmp(file,"..")==0){
        if(check_escape_path(buf,file)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] open: access to %s is not allowed\n",file);
                write_to_terminal(err_msg);
                return return_val;
        }
    }
    origin = dlsym(RTLD_NEXT, "open");
    if(origin != NULL) {
        if(mode > 0777) {
            return_val = origin(file, oflag);
        }
        else {
            return_val = origin(file, oflag, mode);
        }
    }
    else write(STDOUT_FILENO,"open error\n",strlen("open error\n"));
    return return_val;

}

ssize_t readlink(const char *path, char *buf, size_t len){
    ssize_t (*origin)(const char *, char *, size_t) = NULL;
    ssize_t return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
        
    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/' || strcmp(path,".")==0 || strcmp(path,"..")==0){
        if(check_escape_path(buf_curpath,path)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] readlink: access to %s is not allowed\n",path);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "readlink");
    if(origin != NULL) {
        return_val = origin(path,buf,len);
    }
    else write(STDOUT_FILENO,"readlink error\n",strlen("readlink error\n"));
    return return_val;
}

int rename(const char *old, const char *new){
    int (*origin)(const char *, const char *) = NULL;
    int return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(old,"./",2)==0 || strncmp(old,"../",3)==0 || old[0]=='/' || strcmp(old,".")==0 || strcmp(old,"..")==0){
        if(check_escape_path(buf_curpath,old)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] rename: access to %s is not allowed\n",old);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    if(strncmp(new,"./",2)==0 || strncmp(new,"../",3)==0 || new[0]=='/' || strcmp(new,".")==0 || strcmp(new,"..")==0){
        if(check_escape_path(buf_curpath,new)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] rename: access to %s is not allowed\n",new);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "rename");
    if(origin != NULL) {
        return_val = origin(old,new);
    }
    else write(STDOUT_FILENO,"rename error\n",strlen("rename error\n"));
    return return_val;
}

int remove(const char *path){
    int (*origin)(const char *) = NULL;
    int return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/' || strcmp(path,".")==0 || strcmp(path,"..")==0){
        if(check_escape_path(buf_curpath,path)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] remove: access to %s is not allowed\n",path);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "remove");
    if(origin != NULL) {
        return_val = origin(path);
    }
    else write(STDOUT_FILENO,"remove error\n",strlen("remove error\n"));
    return return_val;

}


int rmdir(const char *path){
    int (*origin)(const char *) = NULL;
    int return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/' || strcmp(path,".")==0 || strcmp(path,"..")==0){
        if(check_escape_path(buf_curpath,path)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] rmdir: access to %s is not allowed\n",path);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "rmdir");
    if(origin != NULL) {
        return_val = origin(path);
    }
    else write(STDOUT_FILENO,"rmdir error\n",strlen("rmdir error\n"));
    return return_val;
}

int __xstat (int vers, const char *name, struct stat *buf){
    int (*origin)(int, const char *, struct stat *) = NULL;
    int return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path

    if(strncmp(name,"./",2)==0 || strncmp(name,"../",3)==0 || name[0]=='/' || strcmp(name,".")==0 || strcmp(name,"..")==0 ){
        if(check_escape_path(buf_curpath,name)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] __xstat: access to %s is not allowed\n",name);
                write_to_terminal(err_msg);
                return return_val;
        }
    }
    
    origin = dlsym(RTLD_NEXT, "__xstat");
    if(origin != NULL) {
        return_val = origin(vers, name, buf);
    }
    else write(STDOUT_FILENO,"stat error\n",strlen("stat error\n"));
    return return_val;
}

int __lxstat (int vers, const char *name, struct stat *buf){
    int (*origin)(int, const char *, struct stat *) = NULL;
    int return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path

    if(strncmp(name,"./",2)==0 || strncmp(name,"../",3)==0 || name[0]=='/' || strcmp(name,".")==0 || strcmp(name,"..")==0){
        if(check_escape_path(buf_curpath,name)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] __lxstat: access to %s is not allowed\n",name);
                write_to_terminal(err_msg);
                return return_val;
        }
    }
    
    origin = dlsym(RTLD_NEXT, "__lxstat");
    if(origin != NULL) {
        return_val = origin(vers, name, buf);
    }
    else write(STDOUT_FILENO,"stat error\n",strlen("stat error\n"));
    return return_val;
}

int symlink(const char *from, const char *to){
    int (*origin)(const char *, const char *) = NULL;
    int return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(from,"./",2)==0 || strncmp(from,"../",3)==0 || from[0]=='/' || strcmp(from,".")==0 || strcmp(from,"..")==0 ){
        if(check_escape_path(buf_curpath,from)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] symlink: access to %s is not allowed\n",from);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    if(strncmp(to,"./",2)==0 || strncmp(to,"../",3)==0 || to[0]=='/' || strcmp(to,".")==0 || strcmp(to,"..")==0){
        if(check_escape_path(buf_curpath,to)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] symlink: access to %s is not allowed\n",to);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "symlink");
    if(origin != NULL) {
        return_val = origin(from,to);
    }
    else write(STDOUT_FILENO,"symlink error\n",strlen("symlink error\n"));
    return return_val;

}

int unlink(const char *name){
    int (*origin)(const char *) = NULL;
    int return_val = -1;

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(name,"./",2)==0 || strncmp(name,"../",3)==0 || name[0]=='/' || strcmp(name,".")==0 || strcmp(name,"..")==0){
        if(check_escape_path(buf_curpath,name)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] unlink: access to %s is not allowed\n",name);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "unlink");
    if(origin != NULL) {
        return_val = origin(name);
    }
    else write(STDOUT_FILENO,"unlink error\n",strlen("unlink error\n"));
    return return_val;
}

int execl (const char *path, const char *arg, ...){
    char err_msg[128];
    sprintf(err_msg,"[sandbox] execl(%s): not allowed\n",path);
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execle (const char *path, const char *arg, ...){
    char err_msg[128];
    sprintf(err_msg,"[sandbox] execle(%s): not allowed\n",path);
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execlp (const char *file, const char *arg, ...){
    char err_msg[128];
    sprintf(err_msg,"[sandbox] execlp(%s): not allowed\n",file);
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execv (const char *path, char *const argv[]){
    char err_msg[128];
    sprintf(err_msg,"[sandbox] execv(%s): not allowed\n",path);
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execve (const char *path, char *const argv[], char *const envp[]){
    char err_msg[128];
    sprintf(err_msg,"[sandbox] execve(%s): not allowed\n",path);
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execvp (const char *file, char *const argv[]){
    char err_msg[128];
    sprintf(err_msg,"[sandbox] execvp(%s): not allowed\n",file);
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int system (const char *line){
    char err_msg[128];
    sprintf(err_msg,"[sandbox] system(%s): not allowed\n",line);
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}


int openat (int fd, const char *file, int oflag, ...){
    mode_t mode;
    va_list all_arg;
    va_start(all_arg,oflag);
    mode = va_arg(all_arg,mode_t);
    va_end(all_arg);
    int return_val=-1;
    ssize_t (*origin_readlink)(const char *, char *, size_t) = NULL;
    int (*origin)(int fd, const char *file, int oflag, ...) = NULL;
    int len=-1;
    
    origin_readlink = dlsym(RTLD_NEXT, "readlink");
    /*if(origin != NULL) {
        return_val = origin(path,buf,len);
    }*/

    char path[128];

    if(fd!=AT_FDCWD){
        char tmp[64];
        int pid = getpid();
        sprintf(tmp,"/proc/%d/fd/%d",pid,fd);
        if((len=origin_readlink(tmp,path,sizeof(path)-1))!=-1){
            if(path[strlen(path)-1] != '/'){
                path[strlen(path)]='/';
                path[strlen(path)+1]='\0';
            }
        }
    }
    strcat(path,file);  //concate with input(file)

    char buf_curpath[MAX_PATH_SIZE];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path

    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/' || strcmp(path,".")==0 || strcmp(path,"..")==0){
        if(check_escape_path(buf_curpath,path)!=0 ){
                char err_msg[128];
                memset(err_msg,'\0',128);
                sprintf(err_msg,"[sandbox] openat: access to %s is not allowed\n",path);
                write_to_terminal(err_msg);
                return return_val;
        }
    }

    origin = dlsym(RTLD_NEXT, "openat");
    if(origin != NULL) {
        return_val = origin(fd,file,oflag,mode);
    }
    else write(STDOUT_FILENO,"openat error\n",strlen("openat error\n"));

    return return_val;
}