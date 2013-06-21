#include "pcalc.h"

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
  char buf[64];
  char *conf_file;
  char *filename;
  
  if(argc > 2){
    exit(1);
  }
  else{
    if(argc == 1){
      conf_file = prompt_user("Insert file name: ");
    }
    
    else
      conf_file = argv[1];
    
    if((fd = open(conf_file,O_RDONLY, S_IRUSR)) == -1)
      syserr("fd");
    
    if(argc == 1)
      free(conf_file);
    
    lines = count_lines(fd);    
    n_proc = read_integer(fd);
    sprintf(buf, "Main process is spawning %d child%s for %d operations.\n",n_proc,(n_proc-1)?"ren":"",lines);
    print_to_video(buf);
    available_workers = 2*n_proc;
    remaining_work = 2*n_proc +1;
    proc_ids = (int *)malloc(lines * sizeof(int));    
	
    init_sh_mem(&mem_key1,&mem_key2, &operations,&addresses,lines,n_proc);
    copy_operations(fd, proc_ids,  &operations,lines);
    init_sem(&sem_key, n_proc, lines);

    for(i = 0;i < n_proc;++i){
      if((status = fork()) == -1)
	syserr("fork");

      if(status == 0){
	break;
      }
    }
    
    if(status == 0){
 
      start(sem_id, 2*i,  addresses + i*sizeof(op_addr), available_workers, remaining_work);
     
      exit(0);
    }
    //Stampa operazioni prima di essere assegnate
    printf("Operazioni prima dell'assegnazione\n");
    for(i = 0; i<lines; ++i){
      j = i*sizeof(operation);
      printf("%d %c %d\n",(operations +j)->num1, (operations+j)->op,(operations +j)->num2);
    }

    for(i= 0; i < lines; ++i){
      id = (proc_ids[i]-1);
      
      if(id == -1){
	P(available_workers);
	V(available_workers);
	j = 0;
	while(get_sem_val((2*j+1)) == 0){
	  //printf(" %d non disp \n",j+1);
	  j = (j +1)%n_proc;
	  //printf("provo %d \n",j+1);
	}
	id = j;
      }
      
      P(2*id+1);
      *(addresses + id*sizeof(op_addr)) = operations+i*sizeof(operation);
      V(2*id);
    }
    
    wait_results(remaining_work);
    // Stampa operazioni prima di avere computato
    printf("Array di operazioni dopo avere computato.\n");
    for(i = 0; i<lines; ++i){
      j = i*sizeof(operation);
      printf("%d %c %d\n",(operations +j)->num1, (operations+j)->op,(operations +j)->num2);
    }
    
    results = get_results(operations, lines);
    char *file_name = "res.txt";
    if((fd = open(file_name, O_WRONLY|O_CREAT, 0666)) == -1){
      syserr("open res.txt");
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
    
    for(i = 0; i < n_proc; ++i)
      wait();

    shmctl(mem_id1, IPC_RMID, 0);
    shmctl(mem_id2, IPC_RMID,0);
    semctl(sem_id, 0, IPC_RMID);
  }
}
