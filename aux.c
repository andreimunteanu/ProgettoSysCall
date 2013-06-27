/**
 * @file   aux.c
 * @author <a href="mailto:niccolo.marastoni@studenti.univr.it">Niccolò Marastoni</a>
 * @author <a href="mailto:andrei.munteanu@studenti.univr.it">Andrei Munteanu</a>
 * @date   July, 2013
 * @brief  Contains functions which allows to interact with the sdin,sdout and stderr.
 *
 */

#include "header.h"

void print_to_video(const char *str){
  if(write(1,str,strlen(str)) < 0)
    syserr("print_to_video");
  fflush(stdout);
}

void print_to_file(const int fd, const char *string, const int len){
  int n;

  if((n = write(fd, string, len)) == -1){
    syserr("write");
  }
}

char *prompt_user( const char *msg){
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

void syserr(const char *str){
  write(3,str,strlen(str));
  exit(1);
}

