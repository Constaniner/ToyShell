#include "parse.h"


char ** parse_command(char* input_str,int* paranum,char** command){
    char* parsed_command;
    int i = 0;

    parsed_command = strtok(input_str,STR_DELIMITERS);
    while (parsed_command != NULL) {
        strcpy(command[i],parsed_command);
        i++;
        parsed_command = strtok(NULL,STR_DELIMITERS);
    }
    command[i] = NULL;
    *paranum = i;
    return command;
}

void determine_flags(char** command,bool* bg_flag,int* pip_flag,bool* in_flag, int* out_flag,char*ipt_file,char* opt_file){
    int i = 0;
    while (command[i] != NULL) {
        if (strcmp(command[i],"&") == 0) {
            command[i] = NULL;
            *bg_flag = true;
            i++;
        } else if (strcmp(command[i],"|") == 0) {
            *pip_flag = true;
            i++;
        } else if (strcmp(command[i],"<") == 0 || strcmp(command[i],"<<") == 0) {
            strcpy(ipt_file,command[i+1]);
            command[i] = NULL;
            command[i+1] = NULL;
            *in_flag = true;
            i+= 2;
        } else if (strcmp(command[i],">") == 0) {
            strcpy(opt_file,command[i+1]);
            command[i] = NULL;
            command[i+1] = NULL;
            *out_flag = 1;
            i+= 2;
        } else if (strcmp(command[i],">>") == 0) {
            strcpy(opt_file,command[i+1]);
            command[i] = NULL;
            command[i+1] = NULL;
            *out_flag = 2;
            i+= 2;
        } else {
            i++;
        }
    }
}


void builtin_command(char** command,char* home_dir){
    if (strcmp(command[0],"exit") == 0) {
        exit(0);
    }
    if (strcmp(command[0],"cd") == 0) {
        char* cd_path;
        if (command[1] == NULL) {
            command[1] = "..";
        }
        if (strncmp(command[1],"~",1) == 0) {
            cd_path = malloc((strlen(home_dir)+strlen(command[1]))* sizeof(char*));
            strcpy(cd_path,home_dir);
            strcpy(cd_path+strlen(home_dir),command[1]+1);
        } else {
            cd_path = malloc(strlen(command[1]));
            strcpy(cd_path,command[1]);
        }
        if (chdir(cd_path) != 0) {
            perror("cd ERROR");
        }
        free(cd_path);
    }

    if (strcmp(command[0],"jobs") == 0) {
        //TODO: Implement
        exit(0);
    }
    if (strcmp(command[0],"kill") == 0) {
        //TODO: Implement
        exit(0);
    }
}

void do_pipe(int left, int right,char**command) {
    if (left >= right) return ;
    int pos = -1;
    /*Find the position of pipe*/
    for (int i = left;i<right;i++) {
        pos = i;
        if (strcmp(command[i],"|") == 0) {
            command[i] = NULL;
            break;
        }
    }

    int fds[2];
    pipe(fds);
    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork ERROR");
    } else if (pid == 0) {
        close(fds[0]);
        dup2(fds[1], 1);
        close(fds[1]);

        //TODO: Input Redirection
        /*char* comm[MAX_COMMAND_LEN];
        for (int i = 0;i < pos;i++) {
            comm[i] = command[i+left];
        }
        comm[pos] = NULL;*/


        pid_t child_pid = fork();
        if (child_pid < 0) {
            perror("Fork ERROR");
        } else if (child_pid == 0) {
            execvp(command[left], command+left);
        }else{
            int stat_loc;
            waitpid(child_pid, &stat_loc,0);
        }
    } else {
        int status;
        waitpid(pid, &status,0);

        if (pos+1 < right){
            close(fds[1]);
            dup2(fds[0], 0);
            close(fds[0]);
            do_pipe(pos+1, right,command);
        }
    }
}

