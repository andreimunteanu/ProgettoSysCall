/**
 * @file   main.h
 * @author <a href="mailto:niccolo.marastoni@studenti.univr.it">Niccolò Marastoni</a>
 * @author <a href="mailto:andrei.munteanu@studenti.univr.it">Andrei Munteanu</a>
 * @date   July, 2013
 * @brief  Provides functions to initialize and handle an array of semaphores.
 *
 * 
 */
#ifndef MAIN_H
#define MAIN_H

#include "header.h"

/**
 * @brief Counts the number of operations of the configuration file.
 * @param fd file descriptor of the configuration file.
 * @return the number of operations.
 */
int count_lines( const int fd);

/**
 * @brief Reads an integer from the configuration file. 
 * @param fd the file descriptor of the configuration file.
 * @return the integer read.
 * 
 */
int read_integer(const int fd);

/**
 * @brief Stores all the operations from the configuration file. 
 * @param fd file descriptor of the configuration file.
 * @param operations pointer to the array of operations.
 * @param proc_id array of the child processes' ids.
 * @param line number of operations.
 */
void copy_operations(const int fd, int * const proc_id, operation **operations,int lines);

/**
 * @brief Initializes two shared memory zones.
 * @param mem_key1 pointer to the first shared memory key.
 * @param mem_key2 pointer to the second shared memory key.
 * @param operations pointer to the first shared memory.
 * @param offsets pointer to the second share memery.
 * @param line number of operations.
 * @param n_proc number of child processes.
 *
 * The first shared memory contains an array of operations readed from the configuration file.
 * The second shared memory contains an array of offsets used by the child in the array of
 * of operations.
 * 
 */
void init_sh_mem(key_t *mem_key1, key_t* mem_key2, operation **operations ,int **offsets,const int lines,const int n_proc);

/**
 * @brief The child process calls this fuction in order to  interact with the father process.
 * @param sem_num identifies the child process' semaphore in the array of semaphores.
 * @param offset pointer to the array of offsets.
 * @param operations pointer to the array of operations.
 * @param AVAILABLE_WORKERS position of the counting semaphore (available workers) int the array of semaphores.
 * @param REAMINING_WORK  poistion of the couting semaphore (remaining work) in the array of semaphores.
 *
 */
void start(const int sem_num,int * const offset, operation * const operations, const int AVAILABLE_WORKERS, const int REMAINING_WORK);

/**
 * @brief Decrements by one the semaphore's value.
 * @param Specifies the semaphore in the array.
 *
 * Decrements by one the semaphore specified by sem_num.
 */
void P(const int sem_num);

/**
 * @brief Increments by one the semaphore's value.
 * @param Specifies the semaphore in the array.
 *
 * Increments by one the semaphore specified by sem_num.
 */
void V(const int sem_num);

/**
 * @brief Increments and decrements two specific semaphores.
 * @param Specifies the first semaphore in the array.
 * @param Specifies the second semaphore in the array.
 *
 * Used by che child processes once they finished the operation
 * to increment by one the value of the "available workers semaphore" and
 * decrement by one the value of the "remaining work semaphore".
 */
void child_finish(const int sem_num1,const int sem_num2 );

/**
 * @brief Checks if the semaphore's value is zero.
 * @param Specifies the semaphore in the array.
 * 
 * The father process checks how much work has to be done. 
 */
void wait_results(const int sem_num);

/**
 * @brief Reads the value of a specific semephore.
 * @param sem_num identifies the semaphore in the array of semaphores.
 * @return The value of the semaphore.
 *
 */
int get_sem_val(const int sem_num);

/**
 * @brief Initializes an array of semaphores.
 * @param Key value of the array. 
 * @param Number of child processes.
 * @param Number of operation wich must be done.
 * 
 * Initializes 2*n_proc+2 semaphores. 
 * Each child process has 2 binary semaphores: the first initialized to zero
 * and the second to 1. The last two are counting semaphores, initialized 
 * to n_proc and lines. With the first one the father process checks how many
 * workers are available and with the other how much work is left. 
 */
void init_sem(key_t *sem_key,const int n_proc,const int lines);

/**
 * @brief Prints on the stdout a string.
 * @param str the string to print
 *
 */
void print_to_video( const char *str);

/**
 * @brief Reads from the stdin the name of the configuration file.
 * @param msg message printed on the sdout before reading.
 * @return strings which contains the name of the configuration file.
 */
char *prompt_user(const char *msg);

/**
 * @brief Creates an array of the results of the operations.
 * @param operations pointer to the array of operations
 * @param lines number of operations.
 * @return pointer to the array of results.
 *
 */
float *get_results(operation * const operations,const int lines);

#endif
