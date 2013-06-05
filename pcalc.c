#include "pcalc.h"

void main(int argc, char *argv[]){
  int fd,n_proc,n,i = 0, chars = 0, lines = -1;  
  int *proc_id;
  pid_t *procs;
  char buf[1], super_buf[256];
  operation *operations;
  op_addr *addresses;
  key_t mem_key, sem_key;
  pid_t status;
  
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
    int available_workers = 2*n_proc;
    int remaining_work = 2*n_proc +1;
    procs = (pid_t *)malloc(n_proc * sizeof(pid_t));
    proc_id = (int *)malloc(lines * sizeof(int));    
	
    init_sh_mem(&mem_key, &operations,&addresses,lines,n_proc);
    copy_operations(fd, proc_id,  &operations,lines);

    for(i = 0; i<lines*sizeof(operation); i += sizeof(operation))
      printf("%d %c %d\n",(operations +i)->num1, (operations+i)->op,(operations +i)->num2);    

    init_sem(&mem_key, n_proc, lines);
    
    union semun{
      int val;
      struct semid_ds * buffer;
      unsigned short * array;
    }arg;
    arg.array = (u_short *)malloc((2*n_proc + 2) * sizeof(u_short)); 
    semctl(sem_id, 0, GETALL, arg);
   
 
    for(i = 0;i < 2*n_proc + 2;++i){
     printf("%d ", arg.array[i] );
    }      
    printf("\n");

    for(i = 0;i < n_proc;++i){
      if((status = fork()) == -1){
	perror("fork");
	exit(1);
      }

      if(status == 0){
	break;
      }
    }
    
    if(status == 0){
      start(sem_id, 2*i, addresses + i*sizeof(op_addr), available_workers, remaining_work);
      exit(0);
    }
             
    for(i= 0; i < lines; ++i){
      // P(proc_id[i]);// P speciale NO WAIT da riscrivere
      P(2*(proc_id[i]-1)+1);
      
      *(addresses + (proc_id[i] - 1)*sizeof(op_addr)) = operations+i*sizeof(operation);
      
      V(2*(proc_id[i]-1)+1);
      V(2*(proc_id[i]-1));
    }

    for(i = 0; i < n_proc; ++i)
      wait();
    printf("Ciao papa, muore.\n");
    shmctl(mem_id, IPC_RMID, 0);
    semctl(sem_id, 0, IPC_RMID);
  }
  else
    exit(1);
}
