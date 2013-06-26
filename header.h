/**
 * @file   header.h
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Definition of the struct operation and global variables.
 *
 * 
 */
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

/**
 * Operation struct
 */
typedef struct operation{
  int num1;
  char op;
  int num2;
  float res;
}operation;


int sem_id;
int mem_id1;
int mem_id2;


/**
 * @brief Prints on the stderr a string and exits whit "exit code" 1.
 * @param str the string to print.
 */

void syserr(const char *str);

/**
 * @brief Writes in specific file the string.
 * @param fd file descriptor of the file.
 * @param string the string to write.
 * @param len length of the string.
 *
 */
void print_to_file( const int fd, const char *string,const  int len);
#endif
