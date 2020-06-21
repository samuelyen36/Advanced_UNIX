#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
//-lcapstone
int main(int argc, char *argv[]) {
    // open file
    // read file
    FILE *fp;
    fp = fopen("./no_more_traps.txt", "r");
    if(fp == NULL) printf("can't open file\n");
    pid_t child;
	int count = 0;
	if((child = fork()) < 0) printf("fork");
	if(child == 0) {
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) printf("ptrace");
		execl("/home/samuelyen/Desktop/UNIX_prog/homework/hw4/q5/no_more_traps","no_more_traps",NULL);
	} 
	else {
		int status;
		if(waitpid(child, &status, 0) < 0) printf("waitpid");
		assert(WIFSTOPPED(status));
        	ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
		while(WIFSTOPPED(status)) {
            	if(ptrace(PTRACE_CONT, child, 0, 0) < 0) printf("ptrace(PTRACE_CONT)");
           	if(waitpid(child, &status, 0) < 0 )  printf("wait");
            	struct user_regs_struct regs;
            	if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0){
			printf("ptrace(GETREGS)");
            	}
      	    	/* set registers */
            	regs.rip = regs.rip - 1;
            	/* restore break point */
            	unsigned long code;
            	code = ptrace(PTRACE_PEEKTEXT, child, regs.rip, 0); //get the code at target address (breakpoint address)
            	// read file
            	char buffer[3];
            	fread(buffer, 2, 1, fp);
            	unsigned char tmp;
            	if (buffer[1] >= '0' && buffer[1] <= '9') {
                	tmp = buffer[1] - '0';       //0~9
            	}
            	if (buffer[1] >= 'a') {
                	tmp = buffer[1] - 'a' + 10;                                        //a~f
            	}
            	if (buffer[0] >= '0' && buffer[0] <= '9'){
                	tmp += (buffer[0] - '0') * 16;    //0-9
            	}
            	if (buffer[0] >= 'a'){
                	tmp += (buffer[0] - 'a' + 10) * 16;                                //a~f
            	} 
            	//printf("%x\n",tmp);
            	if(ptrace(PTRACE_POKETEXT, child, regs.rip, (code & 0xffffffffffffff00) | tmp) != 0){
				    printf("ptrace(POKETEXT)");
            	}
            	else{
                	//printf("poketext success\n");
            	}
            	if(ptrace(PTRACE_SETREGS, child, 0, &regs) != 0) printf("ptrace(SETREGS)");
        }
	perror("done");
        
}
	fclose(fp);
	return 0;
    //fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
}
