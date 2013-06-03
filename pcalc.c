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

void child_finish(){
  //più avanti

}

void init_sem(key_t *sem_key, int n_proc, int lines){
  int i;
  if((*sem_key = ftok("pcalc.c", 2)) == -1){
    perror("ftok");
    exit(1); 
  }
  
  if((sem_id = semget(*sem_key, n_proc + 2, 0666|IPC_CREAT|IPC_EXCL)) == -1){
      perror("semget");
      exit(1);
  }
  
  union semun{
   int val;
   struct semid_ds * buffer;
   unsigned short * array;
  }arg;
  
  arg.array = (u_short *)malloc((n_proc + 2) * sizeof(u_short));
  
  for(i = 0;i < n_proc;++i){
   arg.array[i] = 0;
  }
  
  arg.array[n_proc] = n_proc;
  arg.array[n_proc + 1] = lines;
  
  if(semctl(sem_id, 0, SETALL, arg) == -1){
   perror("semctl");
   exit(1);
  }
  free(arg.array);
}

void main(int argc, char *argv[]){
  int fd,i,n,chars,lines,n_proc;  
  int *proc_id;
  pid_t *procs;
  char buf[1], super_buf[256];
  operation *operations;
  key_t mem_key, sem_key;
	
  chars = i = 0;
  lines = -1;
  if(argc == 1){
    char *str = "Inserisci nome file di configurazione: ";
    write(1, str, strlen(str));
    fflush(stdout);
  }
  
  else if(argc == 2){		
    if((fd = open(argv[1],O_RDONLY, S_IRUSR)) == -1);
    
    while((n = read(fd,buf, 1)) > 0){//controllo per la sintassi
      chars++;
      if(*buf == '\n')
	lines++;
    }	
    lseek(fd, -chars, SEEK_END);
		
    n_proc = read_integer(fd);
    procs = (pid_t *)malloc(n_proc * sizeof(pid_t));
    proc_id = (int *)malloc(lines * sizeof(int));
	
    init_sh_mem(&mem_key, &operations,lines);
    copy_operations(fd, proc_id,  &operations,lines);

    for(i = 0; i<lines*sizeof(operation); i += sizeof(operation))
      printf("%d %c %d\n",(operations +i)->num1, (operations+i)->op,(operations +i)->num2);    

    init_sem(&mem_key, n_proc, lines);
    
    union semun{
      int val;
      struct semid_ds * buffer;
      unsigned short * array;
    }arg;
    
    semctl(sem_id, 0, GETALL, arg);
    arg.array = (u_short *)malloc((n_proc + 2) * sizeof(u_short));
 
    for(i = 0;i < n_proc + 2;++i){
     printf("%d ", arg.array[i] );
    }      
    printf("\n");
      
    shmctl(mem_id, IPC_RMID, 0);
    semctl(sem_id, 0, IPC_RMID);
  }
  else
    exit(1);
}
