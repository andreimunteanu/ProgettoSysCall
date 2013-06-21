#include "pcalc.h"

void start( const int sem_num,int *offset, op_addr *address, const int AVAILABLE_WORKERS, const int REMAINING_WORK){
  operation my_operation;
  char str[64];

  while(1){   
    P(sem_num);
    P(AVAILABLE_WORKERS);
    my_operation = **address;
    switch(my_operation.op) {
    case '+': (**address).res = my_operation.num1 + my_operation.num2; break;
    case '-': (**address).res = my_operation.num1 - my_operation.num2; break;
    case '*': (**address).res = my_operation.num1 * my_operation.num2; break;
    case '/': (**address).res = (float)my_operation.num1 / (float)my_operation.num2; break;
    case 'K': return;
    default : printf("No such operation!\n");
    }
   
    sprintf(str, "Child %d ==> %d %c %d = %f\n\n",1 + sem_num / 2, my_operation.num1, my_operation.op, my_operation.num2, (**address).res);
    print_to_video(str);
    V(sem_num+1);
    child_finish(AVAILABLE_WORKERS, REMAINING_WORK);
  }
}
