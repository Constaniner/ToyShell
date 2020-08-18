#include "parse.h"

int main(int argc, char *argv[]) {
    char input_str[MAX_COMMAND_LEN],*home_dir;
    char **command;
    pid_t child_pid;
    int stat_loc;
    int paranum = 0;
    char input_file_name[MAX_NAME_LEN], output_file_name[MAX_NAME_LEN];
    bool bg_flag = false, in_flag = false, file_flag = false;
    int out_flag = 0, pip_flag = 0;
    FILE *script = NULL;

    /*Handle Input Stream*/
    if (argc > 1) {
        file_flag = true;
        if (strcmp(argv[1],"<") == 0 || strcmp(argv[1],"<<") == 0) {
            script = fopen(argv[2], "r");
        } else {
            script = fopen(argv[1], "r");
        }
    }

    while (true) {
        command = (char**)malloc(MAX_JOBS_NUM* sizeof(char *));
        for (int i= 0 ; i<MAX_JOBS_NUM; i++){
            command[i] = (char*)malloc(MAX_COMMAND_LEN*sizeof(char));
        }
        home_dir = malloc(MAX_PATH_LEN * sizeof(char));
        struct passwd *info = getpwuid(getuid());
        strcpy(home_dir,info->pw_dir);

        /*Get Input*/
        if (file_flag == true) {
            char *tmp = fgets(input_str,MAX_COMMAND_LEN, script);
            if (tmp == NULL) {
                fclose(script);
                exit(0);
            }
        } else {
            fgets(input_str,MAX_COMMAND_LEN, stdin);
        }

        /*Divide command*/
        parse_command(input_str, &paranum, command);

        /*Handle empty input*/
        if (command[0] == NULL) {
            free(home_dir);
            //free(input_str);
            free(*command);
            free(command);
            continue;
        }

        /*Determine the format of input*/
        determine_flags(command, &bg_flag, &pip_flag, &in_flag, &out_flag, input_file_name,output_file_name);

        if (strcmp(command[0], "exit") == 0 || strcmp(command[0], "cd") == 0 || strcmp(command[0], "jobs") == 0 ||
            strcmp(command[0], "kill") == 0) {
            builtin_command(command,home_dir);
        } else {
            if (pip_flag == true) {
                do_pipe(0,paranum,command);
            } else {
                child_pid = fork();
                if (child_pid < 0) {
                    free(home_dir);
                    //free(input_str);
                    free(*command);
                    free(command);
                    continue;
                } else if (child_pid == 0) {
                    /*Input Redirection*/
                    if (in_flag == true) {
                        int in_stat = open(input_file_name, O_RDONLY);
                        close(0);
                        dup2(in_stat, 0);
                        close(in_stat);
                    }
                    /*Output Redirection*/
                    if (out_flag == 1) {
                        int out_stat = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        close(1);
                        dup2(out_stat, 1);
                        close(out_stat);
                    } else if (out_flag == 2) {
                        int out_stat = open(output_file_name, O_WRONLY | O_CREAT | O_APPEND, 0666);
                        close(1);
                        dup2(out_stat, 1);
                        close(out_stat);
                    }
                    if (execvp(command[0], command) < 0) {
                        perror(command[0]);
                    }
                }else{
                    if (bg_flag == false) {
                        waitpid(child_pid, &stat_loc, WUNTRACED);
                    } else {
                        continue;
                    }
                }
            }

        }

        free(home_dir);
        //free(input_str);
        //for (int i= 0 ; i<MAX_JOBS_NUM ; i++){
            //free(command[i]);
        //}
        free(*command);
        free(command);
    }
}
