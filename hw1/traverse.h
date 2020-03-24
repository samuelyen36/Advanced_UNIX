#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h>
#include <regex.h>

#define FILE_LENGTH 500

char filtering_string_content[50];
int filtering_string_flag;

void traverse_proc_pid(int socket,char *ip_detail);	//read the directory(file) name in /proc directory, which stands for the pid of processes
int is_target(char *str);	//check if the string is actually the thing.
int judge_format_string(char *ps_directory);
void print_process_information(char *ps_directory);     //   /proc/$pid/comm
void print_process_parameter(char *ps_directory);   //proc/$pid/cmdline
