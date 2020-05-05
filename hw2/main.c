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
//int openat();   //????, be aware of it., jump first
DIR *opendir();
ssize_t readlink();
int rename();
int rmdir();
int __xstat ();
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
    realpath(buf2,tmp); //convert relative path to absolute path
    return(strncmp(buf1,tmp,strlen(buf1)));
}

int chdir (const char *path){
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
    return return_val;
}

DIR *opendir (const char *path){
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    DIR* return_val=NULL;
    DIR* (*origin)(const char *) = NULL;
    if(check_escape_path(buf,path)!=0){    //escape
        char *err_msg = "opendir: try to escape from sandbox\n";
        write_to_terminal(err_msg);
        //write(STDERR_FILENO,err_msg,strlen(err_msg));
        //printf("opendir: nonono, try to escape from sandbox\n");
        return return_val;
    }
    origin = dlsym(RTLD_NEXT, "opendir");
    if(origin != NULL) {
        return_val = origin(path);
        printf( "# opendir(\"%s\") = %p\n", path, return_val);
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
    if(strncmp(file,"./",2)==0 || strncmp(file,"../",3)==0 || file[0]=='/'){    //file is related to directory
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
    return return_val;

}

FILE *fopen(const char * __filename, const char * __modes){
    FILE *return_val=NULL;
    FILE* (*origin)(const char *,const char* __modes) = NULL;
    if(strncmp(__filename,"./",2)==0 || strncmp(__filename,"../",3)==0 || __filename[0]=='/'){
        char buf[64];
        getcwd(buf, sizeof(buf));   //get current absolute path
        if(check_escape_path(buf,__filename)!=0){    //escape
            char *err_msg = "fopen: try to escape from sandbox: ";
            char out_buf[128];
            sprintf(out_buf,"%s %s\n",err_msg,__filename);
            write_to_terminal(out_buf);
            //write(STDERR_FILENO,out_buf,strlen(out_buf));        
            return return_val;
        }
    }
    origin = dlsym(RTLD_NEXT, "fopen");
    if(origin != NULL) {
        return_val = origin(__filename,__modes);
        printf( "# fopen(\"%s\") \n", __filename);
    }
    else printf("fopen() error\n");
    return return_val;
}

int link(const char *oldpath, const char *newpath) {
    int (*origin)(const char *, const char *) = NULL;
    int return_val = -1;
    char buf[64];
    getcwd(buf, sizeof(buf));   //get current absolute path
    
    if(strncmp(oldpath,"./",2)==0 || strncmp(oldpath,"../",3)==0 || oldpath[0]=='/'){
        if(check_escape_path(buf,oldpath)!=0 ){
                char *err_msg = "link: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,oldpath);
                write_to_terminal(out_buf);
                //write(STDERR_FILENO,out_buf,strlen(out_buf));        
                return return_val;
        }
    }
    if(strncmp(newpath,"./",2)==0 || strncmp(newpath,"../",3)==0 || newpath[0]=='/'){
        if(check_escape_path(buf,newpath)!=0 ){
                char *err_msg = "link: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,newpath);
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
    
    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/'){
        if(check_escape_path(buf,path)!=0 ){
                char *err_msg = "mkdir: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,path);
                write_to_terminal(out_buf);
                //write(STDERR_FILENO,out_buf,strlen(out_buf));        
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
    
    if(strncmp(file,"./",2)==0 || strncmp(file,"../",3)==0 || file[0]=='/'){
        if(check_escape_path(buf,file)!=0 ){
                char *err_msg = "open: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,file);
                write_to_terminal(out_buf);
                //write(STDERR_FILENO,out_buf,strlen(out_buf));        
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

    char buf_curpath[64];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
        
    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/'){
        if(check_escape_path(buf_curpath,path)!=0 ){
                char *err_msg = "readlink: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,path);
                write_to_terminal(out_buf);
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

    char buf_curpath[64];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(old,"./",2)==0 || strncmp(old,"../",3)==0 || old[0]=='/'){
        if(check_escape_path(buf_curpath,old)!=0 ){
                char *err_msg = "rename: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,old);
                write_to_terminal(out_buf);
                return return_val;
        }
    }

    if(strncmp(new,"./",2)==0 || strncmp(new,"../",3)==0 || new[0]=='/'){
        if(check_escape_path(buf_curpath,new)!=0 ){
                char *err_msg = "rename: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,new);
                write_to_terminal(out_buf);
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

int rmdir(const char *path){
    int (*origin)(const char *) = NULL;
    int return_val = -1;

    char buf_curpath[64];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(path,"./",2)==0 || strncmp(path,"../",3)==0 || path[0]=='/'){
        if(check_escape_path(buf_curpath,path)!=0 ){
                char *err_msg = "rmdir: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,path);
                write_to_terminal(out_buf);
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

    char buf_curpath[64];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(name,"./",2)==0 || strncmp(name,"../",3)==0 || name[0]=='/'){
        if(check_escape_path(buf_curpath,name)!=0 ){
                char *err_msg = "stat: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,name);
                write_to_terminal(out_buf);
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

    char buf_curpath[64];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(from,"./",2)==0 || strncmp(from,"../",3)==0 || from[0]=='/'){
        if(check_escape_path(buf_curpath,from)!=0 ){
                char *err_msg = "symlink: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,from);
                write_to_terminal(out_buf);
                return return_val;
        }
    }

    if(strncmp(to,"./",2)==0 || strncmp(to,"../",3)==0 || to[0]=='/'){
        if(check_escape_path(buf_curpath,to)!=0 ){
                char *err_msg = "symlink: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,to);
                write_to_terminal(out_buf);
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

    char buf_curpath[64];
    getcwd(buf_curpath, sizeof(buf_curpath));   //get current absolute path
    
    if(strncmp(name,"./",2)==0 || strncmp(name,"../",3)==0 || name[0]=='/'){
        if(check_escape_path(buf_curpath,name)!=0 ){
                char *err_msg = "unlink: try to escape from sandbox: ";
                char out_buf[128];
                sprintf(out_buf,"%s %s\n",err_msg,name);
                write_to_terminal(out_buf);
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
    char *err_msg = "[sandbox]: sandbox always reject execl\n";
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execle (const char *path, const char *arg, ...){
    char *err_msg = "[sandbox]: sandbox always reject execle\n";
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execlp (const char *file, const char *arg, ...){
    char *err_msg = "[sandbox]: sandbox always reject execlp\n";
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execv (const char *path, char *const argv[]){
    char *err_msg = "[sandbox]: sandbox always reject execv\n";
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execve (const char *path, char *const argv[], char *const envp[]){
    char *err_msg = "[sandbox]: sandbox always reject execve\n";
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int execvp (const char *file, char *const argv[]){
    char *err_msg = "[sandbox]: sandbox always reject execvp\n";
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}

int system (const char *line){
    char *err_msg = "[sandbox]: sandbox always reject system\n";
    write_to_terminal(err_msg);
    return -1;  //reject and fail this command
}