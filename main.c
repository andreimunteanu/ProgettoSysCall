/**
 * @file   sem_init.c
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Provides functions to initialize and handle an array of semaphores.
 *
 * 
 */
#include "main.h"

/**

 */

void main(int argc, char *argv[]){
  register int j,i = 0;
  int fd,n_proc, lines = -1;  // da dichiarare i register
  int available_workers, remaining_work; 
  int *proc_ids;
  float *results;
  operation *operations;
  int *offsets;
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
    /*
     */
    available_workers = 2*n_proc;
    remaining_work = 2*n_proc +1;
    proc_ids = (int *)malloc(lines * sizeof(int));    
	
    init_sh_mem(&mem_key1,&mem_key2, &operations,&offsets,lines,n_proc);
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
 
      start(2*i, offsets + i, operations, available_workers, remaining_work);
     
      exit(0);
    }
  
    for(i= 0; i < lines; ++i){
      id = (proc_ids[i]-1);
      
      if(id == -1){
	P(available_workers);
	V(available_workers);
	j = 0;
	while(get_sem_val((2*j+1)) == 0){
	  j = (j +1)%n_proc;
	}
	id = j;
      }
      
      P(2*id+1);
      *(offsets +id) = i;
      V(2*id);
    }
    
    wait_results(remaining_work);
       
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
      *(offsets +i) = 0;
      V(2*i);
    }
    
    for(i = 0; i < n_proc; ++i)
      wait();
    
    free(proc_ids);
    free(results);

    shmctl(mem_id1, IPC_RMID, 0);
    shmctl(mem_id2, IPC_RMID,0);
    semctl(sem_id, 0, IPC_RMID);
  }
}
