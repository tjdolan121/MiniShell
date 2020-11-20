// COMPILE WITH gcc main.c Helpers.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "Helpers.h"

int main(void) {
    int is_bg;
    int pipe_position;
    int args_len;
    char *args[64];
    size_t size;

    puts("Welcome to MyShell");
    while (1) {
        is_bg = 0;

        // ======================== Get line ======================== //

        char *line = NULL;
        size = 0;
        printf("$ ");
        getline(&line, &size, stdin);

        // ======================== Make Array ===================== //

        args_len = string_2_array(line, " \n", args);

        // ================== Early Exit ================= //

        switch (args_len) {
            case -1:
                puts("Too many arguments");
                free(line);
                continue;
            case 0:
                free(line);
                continue;
        }

        if (!strcmp(args[0], "DONE")) {
            free(line);
            return 0;
        }

        if (!strcmp(args[0], "cd")) {
          if (args[1]) {
            chdir(args[1]);
            free(line);
            continue;
          }
          else {
            chdir(getenv("HOME"));
            free(line);
            continue;
          }
        }

        // ================== Process the array ============ //

        is_bg = check_bg(args, args_len);
        pipe_position = check_pipe(args, args_len);
        remove_ampersand_if_present(args, &args_len, is_bg);
        remove_pipe_if_present(args, pipe_position);

        // ================= Run the commands ================ //

        if (!pipe_position) {
          execute_single_job(args, is_bg);
          free(line);
        }


        else {
          char **const first_cmd = args;
          char **const second_cmd = args + pipe_position + 1;
          execute_piped_job(first_cmd, second_cmd, is_bg);
          free(line);
        }
    }
    return 0;
  }
