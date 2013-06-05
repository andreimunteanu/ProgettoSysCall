#include "pcalc.h"

void P(int sem_num){
  
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = -1;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
    perror("wait");
    exit(1);
  }
}

void V(int sem_num){
  struct sembuf op; 
  op.sem_num = sem_num;
  op.sem_op = 1;
  op.sem_flg = 0;
  if(semop(sem_id, &op, 1) == -1){
   perror("wait");
   exit(1);
  }
}

void child_finish(const int sem_num1, int sem_num2 ){
  struct sembuf *op = (struct sembuf*)malloc(sizeof(struct sembuf)*2);
  op->sem_num = sem_num1;
  op->sem_op = 1;
  op->sem_flg = 0;

  (op+1)->sem_num = sem_num2;
  (op+1)->sem_op = -1;
  (op+1)->sem_flg = 0;
  

  if(semop(sem_id, op, 2) == -1){
    perror("child finish");
    exit(1);
  }
}


void child_P(const int sem_num1, int sem_num2 ){
  struct sembuf *op = (struct sembuf*)malloc(sizeof(struct sembuf)*2);
  op->sem_num = sem_num1;
  op->sem_op = -1;
  op->sem_flg = 0;

  (op+1)->sem_num = sem_num2;
  (op+1)->sem_op = -1;
  (op+1)->sem_flg = 0;
  

  if(semop(sem_id, op, 2) == -1){
    perror("child P");
    exit(1);
  }
}

void init_sem(key_t *sem_key, int n_proc, int lines){
  int i;
  if((*sem_key = ftok("pcalc.c", 2)) == -1){
    perror("ftok");
    exit(1); 
  }
  
  if((sem_id = semget(*sem_key, 2*n_proc + 2, 0666|IPC_CREAT|IPC_EXCL)) == -1){
      perror("semget");
      exit(1);
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
   // printf("%d %d ",arg.array[i], arg.array[i+1]);
  }
  printf("\n");
  
  arg.array[2*n_proc] = n_proc;
  arg.array[2*n_proc + 1] = lines;
  
  if(semctl(sem_id, 0, SETALL, arg) == -1){
   perror("semctl");
   exit(1);
  }
 
  free(arg.array);
}
