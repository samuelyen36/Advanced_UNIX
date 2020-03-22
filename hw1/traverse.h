#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h>


void traverse_proc_pid(int socket);	//read the directory(file) name in /proc directory, which stands for the pid of processes
int is_target(char *str);	//check if the string is actually the thing we want(pid>1000).
void print_process_information(char *ps_directory);