#include "pcalc.h"

int read_integer(int fd){
  char c[1], buf[64];
  char *temp = buf;
  
  if(read(fd,c,1) == -1 || *c == '\n'){
    syserr("bad format");
  }
  
  while(*c != '\n' && *c != ' '){
    
    if((*c >= '0' && *c <= '9') )
      *(temp++) = *c;
    else{
      syserr("bad format");
    }
    
    if(read(fd,c,1) == -1){
      syserr("read");
    }
  }
  *temp = '\0';
  return atoi(buf);
}	

void init_sh_mem(key_t *mem_key1, key_t* mem_key2, operation **operations ,int **offsets,int lines,int n_proc){
	
  if((*mem_key1 = ftok("pcalc.c", 1)) == -1){
    syserr("ftok1");
  }

  if((mem_id1 = shmget(*mem_key1,(n_proc*sizeof(int)), 0666|IPC_CREAT|IPC_EXCL)) == -1){
    syserr("shmget1");
  }
  
  if((*offsets = (int*) shmat(mem_id1, NULL, 0)) == (void *)-1){
    syserr("shmat1");
  }
  
  if((*mem_key2 = ftok("pcalc.c", 2)) == -1){
    syserr("ftok2");
  }
  
  if((mem_id2 = shmget(*mem_key2,(lines * sizeof(operation)), 0666|IPC_CREAT|IPC_EXCL)) == -1){
    syserr("shmget2");      
  }
  
  if((*operations =(operation *)shmat(mem_id2,NULL,0)) == (void*)-1){
    syserr("shmat2");
  }

}

int is_operator(char c){
  return (c == '+' || c == '-' || c == '*' || c == '/');
}

void read_remaining_line(int fd, operation *cursor){
  char c[1], buf[64], operator;
  char *aux = buf;
	
  if(read(fd,c,1) == -1 || !is_operator(*c)){
    syserr("read in reamaining line");
  }
  operator = *c;
	
  if(read(fd,c,1) == -1 ) 
    syserr("read");

  if(read(fd,c,1) == -1 || *c == '\n')
    syserr("read");

  while(*c != '\n' && *c != EOF){
		
    if((*c >= '0' && *c <= '9') )
      *(aux++) = *c;
    else
      syserr("format");

    if(read(fd,c,1) == -1)
      syserr("read");
  }
  *aux = '\0';
  cursor->op = operator;
  cursor->num2 = atoi(buf);	
}

void write_line(int fd, operation *cursor){
  cursor->num1 = read_integer(fd);
  read_remaining_line(fd, cursor);	
}

void copy_operations(int fd, int *proc_id, operation **operations,int lines){
  int i = 0;
  char buf[64];
  operation * cursor = *operations;
  //printf("cops %p", cursor);
  while(lines--){
    proc_id[i++] = read_integer(fd);
    write_line(fd, cursor);		
    cursor++;
  }

}
