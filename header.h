#ifndef HEADER_H
#define HEADER_H

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
int mem_id1;
int mem_id2;

void syserr(char *str);
void print_to_file(int fd, char *string, int len);
#endif
