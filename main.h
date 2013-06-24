/**
 * @file   sem_init.c
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Provides functions to initialize and handle an array of semaphores.
 *
 * 
 */
#ifndef MAIN_H
#define MAIN_H

#include "header.h"

int read_integer(int fd);
void init_sh_mem(key_t *mem_key1, key_t* mem_key2, operation **operations ,int **offsets,int lines,int n_proc);
void copy_operations(int fd, int *proc_id, operation **operations,int lines);
void start(int sem_num,int *offset, operation *operations, const int AVAILABLE_WORKERS, const int REMAINING_WORK);
void P(int sem_num);
void V(int sem_num);
void child_P(int sem_num1, int sem_num2);
void child_finish(int sem_num1, int sem_num2 );
void init_sem(key_t *sem_key, int n_proc, int lines);
void wait_results(int sem_num);
void print_to_video(char *str);
char *prompt_user(char *msg);
float *get_results(operation *operations,int lines);
int get_sem_val(int sem_num);
int count_lines(int fd);

#endif 
