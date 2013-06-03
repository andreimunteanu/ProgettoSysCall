#include "pcalc.h"

int read_integer(int fd){
  char c[1], buf[64];
  char *temp = buf;
  
  if(read(fd,c,1) == -1 || *c == '\n'){
    perror("read");
    printf("codifica1\n");
    exit(1);
  }
  
  while(*c != '\n' && *c != ' '){
    
    if((*c >= '0' && *c <= '9') )
      *(temp++) = *c;
    else{
      printf("codifica2\n");
      exit(1);
    }
    
    if(read(fd,c,1) == -1){
      perror("read");
      exit(1);
    }
  }
  *temp = '\0';
  return atoi(buf);
}	

void init_sh_mem(key_t *mem_key, operation **operations ,int lines){
	
  if((*mem_key = ftok("pcalc.c", 1)) == -1){
    perror("ftok");
    exit(1);
  }

  if((mem_id = shmget(*mem_key, lines * sizeof(operation), 0666|IPC_CREAT|IPC_EXCL)) == -1){
    perror("shmget");      
    exit(1);
  }
  
  if((*operations = (operation*) shmat(mem_id, NULL, 0)) == (void *)-1){
    perror("shmat");
    exit(1);
  }	
}

int is_operator(char c){
  return (c == '+' || c == '-' || c == '*' || c == '/');
}

void read_remaining_line(int fd, operation *cursor){
  char c[1], buf[64], operator;
  char *aux = buf;
	
  if(read(fd,c,1) == -1 || !is_operator(*c)){
    printf("bitch\n");
    exit(1);
  }
  operator = *c;
	
  if(read(fd,c,1) == -1 ) // da mettere a novanta
    exit(1);

  if(read(fd,c,1) == -1 || *c == '\n')
    exit(1);

  while(*c != '\n' && *c != EOF){
		
    if((*c >= '0' && *c <= '9') )
      *(aux++) = *c;
    else
      exit(1);

    if(read(fd,c,1) == -1)
      exit(1);
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
  while(lines--){
    proc_id[i++] = read_integer(fd);
    write_line(fd, cursor);		
    //printf("%d %c %d\n",cursor->num1, cursor->op,cursor->num2);
    cursor += sizeof(operation);
  }

}
