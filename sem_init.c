/**
 * @file   sem_init.c
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Provides functions to initialize and handle an array of semaphores.
 *
 * 
 */

#include "header.h"

/**
 * @brief Decrements by one the semaphore's value.
 * @param Specifies the semaphore in the array.
 *
 * Decrements by one the semaphore specified by sem_num.
 */

void P(int sem_num){
  
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
    syserr("P");
  }
}

/**
 * @brief Increments by one the semaphore's value.
 * @param Specifies the semaphore in the array.
 *
 * Increments by one the semaphore specified by sem_num.
 */

void V(int sem_num){
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
   syserr("V");
  }
}

/**
 * @brief Checks if the semaphore's value is zero.
 * @param Specifies the semaphore in the array.
 * 
 * The father process checks how much work has to be done. 
 */

void wait_results(int sem_num){
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = 0;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
   syserr("wait results");
  }
}

/**
 * @brief Increments and decrements two specific semaphores.
 * @param Specifies the first semaphore in the array.
 * @param Specifies the second semaphore in the array.
 *
 * Used by che child processes once they finished the operation
 * to increment by one the value of the "available workers semaphore" and
 * decrement by one the value of the "remaining work semaphore".
 */

void child_finish(int sem_num1, int sem_num2 ){
  struct sembuf *op = (struct sembuf*)malloc(sizeof(struct sembuf)*2);
  op->sem_num = sem_num1;
  op->sem_op = 1;
  op->sem_flg = 0;

  (op+1)->sem_num = sem_num2;
  (op+1)->sem_op = -1;
  (op+1)->sem_flg = 0;
  

  if(semop(sem_id, op, 2) == -1){
    syserr("child finish");
  }
  free(op);
}

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

void init_sem(key_t *sem_key, int n_proc, int lines){
  register int i;
  if((*sem_key = ftok("main.c", 2)) == -1){
    syserr("ftok"); 
  }
  
  if((sem_id = semget(*sem_key, 2*n_proc + 2, 0666|IPC_CREAT|IPC_EXCL)) == -1){
    syserr("semget");
  }
  union semun{
   int val;
   struct semid_ds * buffer;
   unsigned short * array;
  }arg;
  
  arg.array = (u_short *)malloc((2*n_proc + 2) * sizeof(u_short));
  
  for(i = 0;i < n_proc; ++i){
   arg.array[2*i] = 0;
   arg.array[2*i +1] = 1;
  }
  
  arg.array[2*n_proc] = n_proc;
  arg.array[2*n_proc + 1] = lines;
  
  if(semctl(sem_id, 0, SETALL, arg) == -1){
    syserr("semctl");
  }
 
  free(arg.array);
}
