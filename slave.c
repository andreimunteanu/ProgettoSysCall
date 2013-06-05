#include "pcalc.h"

void start(int sem_id, const int sem_num, op_addr *address, const int AVAILABLE_WORKERS, const int REMAINING_WORK){
  operation my_operation;
  char c;
  
  printf("start address = %p\n",*address);
  while(1){
    child_P(sem_num ,sem_num+1);
    
    my_operation = **address;
    printf("\tmy_operation %d %c %d\n",my_operation.num1,my_operation.op, my_operation.num2);
    
    printf("\tfiglio: %d,op add %p \n",sem_num,*address);       
    
    printf("op %c ", my_operation.op);
    switch(my_operation.op) {
    case '+': my_operation.res = my_operation.num1 + my_operation.num2; break;
    case '-': my_operation.res = my_operation.num1 - my_operation.num2; break;
    case '*': my_operation.res = my_operation.num1 * my_operation.num2; break;
    case '/': my_operation.res = (float)my_operation.num1 / (float)my_operation.num2; break;
    case 'K': return;
    default : printf("No such operation!\n");
    }
    printf("res %f\n",my_operation.res);
    V(sem_num+1);
    child_finish(AVAILABLE_WORKERS, REMAINING_WORK);
    //return;
  }
}
