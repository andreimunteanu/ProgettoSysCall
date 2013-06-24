#include "header.h"

void print_to_video(char *str){
  if(write(1,str,strlen(str)) < 0)
    syserr("print_to_video");
  fflush(stdout);
}

void print_to_file(int fd, char *string, int len){
  int n;

  if((n = write(fd, string, len)) == -1){
    syserr("write");
  }
}

char *prompt_user(char *msg){
  register int i = 0;
  char *filename = malloc(64*sizeof(char));
  char buf[1];
  print_to_video(msg);
  while((read(2, buf, 1) > 0) && *buf != '\n'){
    filename[i++] = buf[0];
  }
  filename[i] = '\0';
  i = 0;  
  return filename;
}

void syserr(char *str){
  write(3,str,strlen(str));
  exit(1);
}

float *get_results(operation *operations,int lines){
  register int i;
  float *temp = (float*)malloc(lines*sizeof(float));
  for(i = 0; i < lines ; ++i){
    *(temp+i) = (operations+i)->res;
  }
  return temp;
}

int get_sem_val(int sem_num){
  int ret;
  char buf[64];
  union semun {
    int val;
    struct semid_ds * buffer;
    unsigned short * array;
  } arg;
  ret =semctl(sem_id, sem_num, GETVAL, arg);
  sprintf(buf, "Child (%d) is %s\n", (sem_num+1)/2, ret?"FREE":"BUSY");
  print_to_video(buf);
  return ret;
}

int count_lines(int fd){
  int chars, n, lines;
  char buf[1];
  chars = 0;
  lines = -1;
  while((n = read(fd,buf, 1)) > 0){
    chars++;
    if(*buf == '\n')
      lines++;
  }	
  
  lseek(fd, -chars, SEEK_END);
  return lines;
}
