/**
 * @file   slave.c
 * @author <a href="mailto:niccolo.marastoni@studenti.univr.it">Niccolò Marastoni</a>
 * @author <a href="mailto:andrei.munteanu@studenti.univr.it">Andrei Munteanu</a> 
 * @date   July, 2013
 * @brief  Contains the function start that will be called by every child process.
 *
 * 
 */
#include "header.h"

void start(const int sem_num,int * const offset, operation * const operations, const int AVAILABLE_WORKERS, const int REMAINING_WORK){
  operation *my_operation;
  char str[64];

  while(1){   
    P(sem_num); //wait an operation
    P(AVAILABLE_WORKERS); // decrement by one the value of the available workers 
    my_operation = (operations + *offset);// initialize the pointer to the operation
    switch(my_operation->op) {
    case '+': my_operation->res = my_operation->num1 + my_operation->num2; break;
    case '-': my_operation->res = my_operation->num1 - my_operation->num2; break;
    case '*': my_operation->res = my_operation->num1 * my_operation->num2; break;
    case '/': my_operation->res = (float)my_operation->num1 / (float)my_operation->num2; break;
    case 'K': return;
    default : print_to_video("No such operation!\n");
    }
   
    sprintf(str, "Child %d ==> %d %c %d = %f\n\n",1 + sem_num / 2, my_operation->num1, my_operation->op, my_operation->num2, my_operation->res);
    print_to_video(str);
    V(sem_num+1); // available for another operation 
    child_finish(AVAILABLE_WORKERS, REMAINING_WORK); // +1 available workers -1 remaining work
  }
}
