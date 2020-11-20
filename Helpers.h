int string_2_array(char *input_string, char *delimiter, char **output_array);
int check_bg(char **input_array, int array_len);
int check_pipe(char **args, int args_len);
int remove_ampersand_if_present(char **input_array, int *args_len, int is_bg);
int remove_pipe_if_present(char **input_array, int pipe_position);
int execute_single_job(char **args, int is_bg);
int execute_piped_job(char **first_cmd, char **second_cmd, int is_bg);
