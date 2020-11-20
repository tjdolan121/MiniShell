#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "Helpers.h"

int string_2_array(char *input_string, char *delimiter, char **output_array) {
    char *token;
    int args_len = 0;
    token = strtok(input_string, delimiter);
    while (token != NULL) {
        *output_array = token;
        token = strtok(NULL, delimiter);
        args_len++;
        if (args_len > 63) {
            return -1;
        }
        output_array++;
    }
    *output_array = NULL;
    return args_len;
}

int check_bg(char **input_array, int array_len) {
    input_array += array_len - 1;
    if (!strcmp(*input_array, "&")) {
      return 1;
    }
    else {
      return 0;
    }
}

int check_pipe(char **args, int args_len) {
  for (int i = 0; i < args_len; i++) {
    if (strcmp(*args, "|")) {
        args++;
    }
    else {
      *args = NULL;
      return i;
    }
  }
  return 0;
}

int remove_ampersand_if_present(char **input_array, int *args_len, int is_bg) {
  if (is_bg) {
    input_array += *args_len - 1;
    *input_array = NULL;
    *args_len--;
  }
  return 0;
}

int remove_pipe_if_present(char **input_array, int pipe_position) {
  if (pipe_position) {
    input_array += pipe_position;
    *input_array = NULL;
  }
  return 0;
}

int execute_single_job(char **args, int is_bg) {
  int cstatus;
  pid_t pid = fork();
  if (pid == (pid_t) 0) {
      if (execvp(args[0], args) == -1) {
          perror("execvp: error");
      }
      exit(1);
  } else {
      if (pid == (pid_t)(-1)) {
          perror("Fork Failed");
          exit(1);
      }
  }
  if (!is_bg) {
    pid_t c = wait(&cstatus);
    printf("Child %ld exited with status = %d\n", (long) c, cstatus);
  }
  return 0;
}

int execute_piped_job(char **first_cmd, char **second_cmd, int is_bg) {
  int cstatus;
  pid_t pid;
  int g[2];

  pid = fork();
  if (pid == -1) {
    puts("First fork failed");
    exit(1);
  }
  else if (pid == 0) {
    pid_t pid2;
    if (pipe(g) < 0) {
      puts("Pipe Failed in Child");
      exit(1);
    }
    else {
      pid2 = fork();
      if (pid2 == -1) {
        puts("Second fork failed");
        exit(1);
      }
      else if (pid2 == 0) {
        dup2(g[0], 0); // Read == Stdin
        close(g[1]);
        if (execvp(second_cmd[0], second_cmd) == -1) {
          perror("execvp: error");
          exit(1);
        }
      }
      else {
        int status;
        dup2(g[1], 1); // Write == Stdout
        close(g[0]);
        if (execvp(first_cmd[0], first_cmd) == -1) {
          perror("execvp: error");
        }
      }
    }
  }
  // In the parent
  else {
    if (!is_bg) {
      pid_t c = wait(&cstatus);
      printf("Child %ld exited with status = %d\n", (long) c, cstatus);
    }
  }
  return 0;
}
