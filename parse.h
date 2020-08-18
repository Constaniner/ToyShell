#include <pwd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_JOBS_NUM 20
#define MAX_NAME_LEN 100
#define MAX_PATH_LEN 1000
#define MAX_COMMAND_LEN 500
#define STR_DELIMITERS " \t\r\n\a"


/*Function to divide command*/
char ** parse_command(char* input_str,int* paranum,char** command);

/*Function to determine flags*/
void determine_flags(char** command,bool* bg_flag,int* pip_flag,bool* in_flag, int* out_flag,char*ipt_file,char* opt_file);

/*Function to implement some built-in commands*/
void builtin_command(char** command,char* home_dir);

/*Function to deal with pipe*/
void do_pipe(int left, int right,char**command);