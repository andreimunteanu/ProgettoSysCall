/**
 * @file   sem_init.c
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Provides functions to initialize and handle an array of semaphores.
 * 
 *  
 */

#include "header.h"

void P(const int sem_num){
  
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
    syserr("P");
  }
}

void V(int sem_num){
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
   syserr("V");
  }
}

void wait_results(const int sem_num){
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = 0;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
   syserr("wait results");
  }
}

void child_finish(const int sem_num1,const int sem_num2 ){
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

int get_sem_val(const int sem_num){
  int ret;
  char buf[64];
  union semun {
    int val;
    struct semid_ds * buffer;
    unsigned short * array;
  } arg;
  ret =semctl(sem_id, sem_num, GETVAL, arg);
  sprintf(buf, "Child (%d) is %s\n", (sem_num+1)/2, ret?"FREE":"BUSY");
  print_to_video(buf);
  return ret;
}


void init_sem(key_t *sem_key, const int n_proc, const int lines){
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
