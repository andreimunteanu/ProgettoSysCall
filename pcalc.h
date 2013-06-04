#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>

typedef struct operation{
  int num1;
  char op;
  int num2;
  float res;
}operation;

int sem_id;
int mem_id;

int read_integer(int fd);
void init_sh_mem(key_t *mem_key, operation **operations ,int lines);
int is_operator(char c);
void read_remaining_line(int fd, operation *cursor);
void write_line(int fd, operation *cursor);
void copy_operations(int fd, int *proc_id, operation **operations,int lines);