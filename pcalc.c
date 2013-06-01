#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>

typedef struct operation{
	int n1;
	char op;
	int n2;
}operation;

int read_integer(int fd){
	char c[1], buf[64];
	char *temp = buf;
	
	if(read(fd,c,1) == -1 || *c == '\n')
			exit(1);

	while(*c != '\n' || *c != ' '){
		
		if((*c >= '0' && *c <= '9') )
			*(temp++) = *c;
		else
			exit(1);

		if(read(fd,c,1) == -1)
			exit(1);
	}
	*temp = '\0';
	return atoi(buf);
}	

void init_sh_mem(key_t *mem_key, int *mem_id, operation **operations ,int lines){
	if((*mem_key = ftok("calc.c", 1)) == -1)
      exit(2013);

    if((*mem_id = shmget(*mem_key, lines * sizeof(operation), 0666|IPC_CREAT|IPC_EXCL)) == -1) 
      exit(6);

    if((*operations = (operation*) shmat(*mem_id, NULL, 0)) == (void *)-1)
      exit(18);
}

int is_operator(char c){
	return (c == '+' || c == '-' || c == '*' || c == '/');
}

char read_remaining_line(int fd, operation *cursor){
	char c[1], buf[64], operator;
	char *aux = buf;
	
	if(read(fd,c,1) == -1 || !is_operator(*c))
			exit(1);
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
	cursor->n2 = atoi(buf);
	
	return *c;
}

char write_line(int fd, operation *cursor){
	cursor->n1 = read_integer(fd);
	return read_remaining_line(fd, cursor);	
}
void copy_operations(int fd, int *proc_id, operation **operations){
	char c, buf[64];
	int i = 0;
	operation * cursor = *operations;
	c = ' ';
	while(c){
		
		proc_id[i++] = read_integer(fd);
		c = write_line(fd, cursor);
		
		cursor += sizeof(operation);
	}

}

void main(int argc, char *argv[]){
  int fd,i,n,chars,lines,n_proc;  
  int *proc_id;
  char buf[1], super_buf[256];
  pid_t *procs;
  operation *operations;
  key_t mem_key, sem_key;
  int mem_id;

	chars = i = 0;
	lines = 1;
  if(argc == 1){
    char *str = "Inserisci nome file di configurazione: ";
    write(1, str, strlen(str));
    fflush(stdout);
    //scanf
  }
  else if(argc == 2){
    if((fd = open(argv[1],O_RDONLY, S_IRUSR)) == -1);
      //      syserr("open");
    
    while((n = read(fd,buf, 1)) > 0){//controllo per la sintassi
      chars++;
      if(*buf == '\n')
				lines++;
    }	
		lseek(fd, -chars, SEEK_END);

		n_proc = read_integer(fd);
		
		procs = (pid_t *)malloc(n_proc * sizeof(pid_t));
    proc_id = (int *)malloc(lines * sizeof(int));

		init_sh_mem(&mem_key, &mem_id, &operations,lines);

		
		
    
   /* if((sem_key = ftok(argv[0], 2)) == -1)
      exit(2099);*/

	}
	else
    exit(1);
}
