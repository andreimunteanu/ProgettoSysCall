/**
 * @file   mem_init.c
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Provides functions in order to: read and parse the configuration file,
 *         initialize and handle the shared memory.
 * 
 */
#include "header.h"

int count_lines(const int fd){
  register chars, lines;
  int n;
  char buf[1];
  chars = 0;
  lines = -1;
  while((n = read(fd,buf, 1)) > 0){
    ++chars;
    if(*buf == '\n')
      ++lines;
  }	
  
  lseek(fd, -chars, SEEK_END);
  return lines;
}

int read_integer(const int fd){
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

void init_sh_mem(key_t *mem_key1, key_t* mem_key2, operation **operations ,int **offsets,const int lines,const int n_proc){
	
  if((*mem_key1 = ftok("main.c", 1)) == -1){
    syserr("ftok1");
  }

  if((mem_id1 = shmget(*mem_key1,(n_proc*sizeof(int)), 0666|IPC_CREAT|IPC_EXCL)) == -1){
    syserr("shmget1");
  }
  
  if((*offsets = (int*) shmat(mem_id1, NULL, 0)) == (void *)-1){
    syserr("shmat1");
  }
  
  if((*mem_key2 = ftok("main.c", 2)) == -1){
    syserr("ftok2");
  }
  
  if((mem_id2 = shmget(*mem_key2,(lines * sizeof(operation)), 0666|IPC_CREAT|IPC_EXCL)) == -1){
    syserr("shmget2");      
  }
  
  if((*operations =(operation *)shmat(mem_id2,NULL,0)) == (void*)-1){
    syserr("shmat2");
  }

}

/**
 * @brief Checks if the character passed is an operator.
 * @param The character to check.
 * @return true (not zero) if c is an operator( +,-,*,/)  
 *         otherwise false (zero).
 */

int is_operator(char c){
  return (c == '+' || c == '-' || c == '*' || c == '/');
}

/**
 * @brief Reads fro the configuration file op and num2.
 * @param fd file descriptor of the configuration file.
 * @param cursors pointer to the array of operations.
 *  
 * Stores int the array of operations the readed arguments.
 */

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

/**
 * @brief Stores one operation in the array of operations.
 * @param fd file descriptor of the configuration file.
 * @param cursor pointer to the array of operations.
 *
 */

void write_line(int fd, operation *cursor){
  cursor->num1 = read_integer(fd);
  read_remaining_line(fd, cursor);	
}


void copy_operations( const int fd, int * const proc_id, operation **operations,int lines){
  register int i = 0;
  operation * cursor = *operations;
  while(lines--){
    proc_id[i++] = read_integer(fd);
    write_line(fd, cursor);		
    ++cursor;
  }

}
