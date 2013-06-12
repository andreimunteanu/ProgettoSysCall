#include "pcalc.h"

void start(int sem_id, const int sem_num, op_addr *address, const int AVAILABLE_WORKERS, const int REMAINING_WORK){
  operation my_operation;
  char c;
  

  while(1){
    // child_P(sem_num ,AVAILABLE_WORKERS);
    P(sem_num);
    P(AVAILABLE_WORKERS);
    //printf("start address = %p\n",*address);    
    my_operation = **address;
    //printf("\t %d my_operation %d %c %d\n",sem_num,my_operation.num1,my_operation.op, my_operation.num2);
    // printf("\t %c %d \n",(**address).op,(**address).num1);
    //printf("\tfiglio: %d,op add %p \n",sem_num,*address);       
    
    //printf("op %c ", my_operation.op);
    switch(my_operation.op) {
    case '+': (**address).res = my_operation.num1 + my_operation.num2; break;
    case '-': (**address).res = my_operation.num1 - my_operation.num2; break;
    case '*': (**address).res = my_operation.num1 * my_operation.num2; break;
    case '/': (**address).res = (float)my_operation.num1 / (float)my_operation.num2; break;
    case 'K': /*printf("Muoro");*/ return;
    default : printf("No such operation!\n");
    }
    //printf("res %f\n",my_operation.res);
    V(sem_num+1);
    child_finish(AVAILABLE_WORKERS, REMAINING_WORK);
    //return;
  }
}
