#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
       #include <errno.h>

// addr of output: 0000000000201b80
int main(){
    pid_t pid;

    pid = fork();

    if (pid==0){   //child
        if(ptrace(PTRACE_TRACEME  ,0,0,0)<0){
            exit(0);
        }
        execl("/home/samuelyen/Desktop/UNIX_prog/homework/hw4/q1/traceme2","traceme2",NULL);
    }
    else{   //parent
        int status;
        long res=0;
        char ptr[64];
        long addr = 0x0000000000201b80;
        if(waitpid(pid,&status,0) <0 )    exit(0);
        //assert(WIFSTOPPED(status));
        //ptrace(PTRACE_SETOPTIONS,pid,0,PTRACE_O_EXITKILL);
        //ptrace(PTRACE_CONT,pid,0,0);
        res=ptrace(PTRACE_PEEKDATA,pid,(void *)0x0000000000201b80,0);
        if(res==-1){
            printf("errno: %d\n",errno);
        }
        ptrace(PTRACE_CONT,pid,0,0);
        //printf("%ld",res);
        waitpid(pid,&status,0);

        if (WIFEXITED(status))  printf("exit status = %d\n", WEXITSTATUS(status));
        //printf("child return with status %d",status);
    }
    return 0;
}