/**
 * @file   sem_init.c
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Provides functions to initialize and handle an array of semaphores.
 *
 * 
 */
#include "header.h"

/**

 */

void start(int sem_num,int *offset, operation *operations, const int AVAILABLE_WORKERS, const int REMAINING_WORK){
  operation *my_operation;
  char str[64];

  while(1){   
    P(sem_num);
    P(AVAILABLE_WORKERS);
    my_operation = (operations + *offset);
    switch(my_operation->op) {
    case '+': my_operation->res = my_operation->num1 + my_operation->num2; break;
    case '-': my_operation->res = my_operation->num1 - my_operation->num2; break;
    case '*': my_operation->res = my_operation->num1 * my_operation->num2; break;
    case '/': my_operation->res = (float)my_operation->num1 / (float)my_operation->num2; break;
    case 'K': return;
    default : printf("No such operation!\n");
    }
   
    sprintf(str, "Child %d ==> %d %c %d = %f\n\n",1 + sem_num / 2, my_operation->num1, my_operation->op, my_operation->num2, my_operation->res);
    print_to_video(str);
    V(sem_num+1);
    child_finish(AVAILABLE_WORKERS, REMAINING_WORK);
  }
}
