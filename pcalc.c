#include "pcalc.h"

float *get_results(operation *operations,int lines){
  int i;
  float *temp = (float*)malloc(lines*sizeof(float));
  for(i = 0; i < lines ; ++i){
    *(temp+i) = (operations+i*sizeof(operation))->res;
  }
  return temp;
}

int get_sem_val(int sem_num){
  union semun {
    int val;
    struct semid_ds * buffer;
    unsigned short * array;
  } arg;
  
  semctl(sem_id, sem_num, GETVAL, arg);
  return arg.val;
}

void print_to_file(int fd, char *string, int len){
  int n;

  if((n = write(fd, string, len)) == -1){
    perror("write");
    exit(1);
  }
}

int count_lines(int fd){
  int chars, n, lines;
  char buf[1];
  chars = 0;
  lines = -1;
  while((n = read(fd,buf, 1)) > 0){//controllo per la sintassi
    chars++;
    if(*buf == '\n')
      lines++;
  }	
  
  lseek(fd, -chars, SEEK_END);
  return lines;
}

char *prompt_user(char *msg){
  int i = 0;
  char *filename = malloc(64*sizeof(char));
  char buf[1];
  //char *str = "Inserisci nome file di configurazione: ";
  write(1, msg, strlen(msg));
  fflush(stdout);
  while((read(2, buf, 1) > 0) && *buf != '\n'){
    filename[i++] = buf[0];
  }
  filename[i] = '\0';
  i = 0;	
  return filename;
}

void main(int argc, char *argv[]){
  int fd,n_proc,j,i = 0, lines = -1;  // da dichiarare i register
  int available_workers, remaining_work; 
  int *proc_ids;
  float *results;
  operation *operations;
  op_addr *addresses;
  key_t mem_key1,mem_key2, sem_key;
  pid_t status;
  int id;
  char *conf_file;
  char *filename;
  
  if(argc > 2){
    exit(1);
  }
  else{
    if(argc == 1){
      conf_file = prompt_user("insert file name: ");
    }
    
    else
      conf_file = argv[1];
    
    if((fd = open(conf_file,O_RDONLY, S_IRUSR)) == -1){
      perror("open conf");
      exit(1);
    }
    
    if(argc == 1)
      free(conf_file);

    lines = count_lines(fd);    
    n_proc = read_integer(fd);
    available_workers = 2*n_proc;
    remaining_work = 2*n_proc +1;
    proc_ids = (int *)malloc(lines * sizeof(int));    
	
    init_sh_mem(&mem_key1,&mem_key2, &operations,&addresses,lines,n_proc);
    
    //printf("in ops %p\n",operations);
    copy_operations(fd, proc_ids,  &operations,lines);
    //printf("last %p \n", addresses + (n_proc-1)*sizeof(op_addr));
    
       
    for(i = 0; i<lines; ++i){
      j = i*sizeof(operation);
      //printf("%d %c %d\n",(operations +j)->num1, (operations+j)->op,(operations +j)->num2);
    }

    init_sem(&sem_key, n_proc, lines);
    
    union semun{
      int val;
      struct semid_ds * buffer;
      unsigned short * array;
    }arg;
    arg.array = (u_short *)malloc((2*n_proc + 2) * sizeof(u_short)); 
    semctl(sem_id, 0, GETALL, arg);
   
    /*
      for(i = 0;i < 2*n_proc + 2;++i){
      printf("%d ", arg.array[i] );
      }      
      printf("\n");*/

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
      /* if((addresses = (op_addr*) shmat(mem_id, NULL, 0)) == (void *)-1){
	 perror("shmat");
	 exit(1);
	 }
	 addresses = addresses;*/
      //printf();
      start(sem_id, 2*i,  addresses + i*sizeof(op_addr), available_workers, remaining_work);
      //      printf("Esco \n");
      exit(0);
    }
    //    printf("tocca al babbo\n");      
    for(i= 0; i < lines; ++i){
      id = (proc_ids[i]-1);
      
      if(id == -1){
	P(available_workers);
	V(available_workers);
	j = 0;
	while(get_sem_val(j=(j+1) % n_proc) == 0){
	  printf(" %d non disp \n",j);
	}
	id = j;
      }
      
      P(2*id+1);
      *(addresses + id*sizeof(op_addr)) = operations+i*sizeof(operation);
      V(2*id);
    }
    
    wait_results(remaining_work);
    //    printf("i figli hanno finito, ora li coppo\n");
    
    results = get_results(operations, lines);
    char *file_name = "res.txt";
    if((fd = open(file_name, O_WRONLY|O_CREAT, 0666)) == -1){
      perror("open res");
      exit(1);
    }
    for(i = 0; i < lines; ++i){
      char string[64];
      sprintf(string, "%f\n", *(results+i));
      print_to_file(fd, string, strlen(string));
    }

    operations[0].op = 'K';
    
    for(i = 0; i < n_proc; ++i){
      *(addresses + i*sizeof(op_addr)) = operations;
      V(2*i);
    }
    
    printf("");
    for(i = 0; i < n_proc; ++i)
      wait();
    //printf("Ciao papa, muore.\n");
    shmctl(mem_id1, IPC_RMID, 0);
    shmctl(mem_id2, IPC_RMID,0);
    semctl(sem_id, 0, IPC_RMID);
  }
}
