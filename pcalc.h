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

typedef operation* op_addr;

int sem_id;
int mem_id;

int read_integer(int fd);
void init_sh_mem(key_t *mem_key, operation **operations ,op_addr** addresses,int lines,int n_proc);
void copy_operations(int fd, int *proc_id, operation **operations,int lines);
void start(int sem_id, const int sem_num, op_addr *address, const int AVAILABLE_WORKERS, const int REMAINING_WORK);
void P(int sem_num);
void V(int sem_num);
void child_P(const int sem_num1, int sem_num2);
void child_finish(const int sem_num1, int sem_num2 );
void init_sem(key_t *sem_key, int n_proc, int lines);
