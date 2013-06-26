/**
 * @file   main.c
 * @Author Me (me@example.com)
 * @date   July, 2013
 * @brief  Main function.
 *
 * 
 */
#include "main.h"

/**
 * @mainpage
 * @author <a href="mailto:niccolo.marastoni@studenti.univr.it">Niccolò Marastoni</a>
 * @author <a href="mailto:andrei.munteanu@studenti.univr.it">Andrei Munteanu</a>
 * @section Specifica
 * Il programma dovrà leggere un file di configurazione, contenente:
 * <ol>
 * <li> Il numero di processi di calcolo parallelo.
 * <li> I dati di computazione.
 * </ol>
 * Il file di configurazione avra' la seguente struttura:
 * <ul>
 * <li> La prima riga conterra' un intero, che corrisponde al numero di processi paralleli da creare, NPROC.
 * <li> Le altre righe avranno il seguente formato: <id> <num1> <op> <num2>, dove:
 * <ol>
 * <li> id e' il numero di processo parallelo a cui far eseguire l'operazione, se e' maggiore o uguale a 1. Se e' zero, indica che l'operazione dovra' essere eseguita dal primo processo.
 * libero. Si assuma che id corrisponda sempre ad un valore corretto.
 * <li> num1 e num2 sono due numeri interi, corrispondenti ai dato sui quali eseguire
 * l'operazione.
 * <li> op e' un carattere rappresentante l'operazione da eseguire sui due numeri. Caratteri validi
 * per op sono: '+', '-', '*', '/', rappresentanti rispettivamente la somma, la sottrazione, la
 * moltiplicazione e la divisione.
 * </ol>
 * </ul>
 * Ogni processo eseguirà una routine di questo tipo:
 * <ol>
 * <li> Attesa su un semaforo per eseguire un calcolo.
 * <li> Ricezione dei dati e dell'operazione da eseguire, da parte del processo padre.
 * <ul>
 * <li> Se riceve il comando di terminazione, termina la sua esecuzione. Il comando di terminazione e' indicato dal carattere 'K'.
 * </ul>
 * <li> Esecuzione del calcolo.
 * <li> Invio del risultato al padre, e segnalazione al padre che ha finito di calcolare.
 * <li> Attesa bloccante sullo stesso semaforo al punto 1, per attendere il comando successivo.
 * </ol>
 * Il processo padre comunichera' con i processi figlio tramite memoria condivisa.
 * Il processo padre eseguira' in questo modo:
 * <ol>
 * <li> Setup della simulazione, leggendo dal file di configurazione il numero di processori da simulare, creandone i processi relativi, e creando ed inizializzando le eventuali strutture di supporto quali semafori e memoria condivisa. Inoltre, verra' creato un array in cui memorizzare i risultati delle operazioni eseguite. La lunghezza di tale array sara' ricavata dal numero di operazioni lette dal file di configurazione.
 * <li> Entrata in un ciclo che per ogni operazione da simulare effettua quanto segue:
 * <li> Se il comando ha id diverso da zero, attende che il processo numero id sia libero, salva il risultato dell'eventuale calcolo precedente nell'array dei risultati, e poi interagisce con lui passandogli il comando da simulare. Il processo padre non deve attendere che il processore abbia completato la simulazione dell'operazione passata al figlio. 
 * <li> Se l'istruzione ha id 0, trova il primo processore libero ed in caso interagisce con esso, come al punto 1. Altrimenti attende che almeno un processore sia libero, e poi interagisce con esso come al punto 1.
 * <li> Passati tutti i comandi ai figli, attende che i processi figlio abbiano eseguito tutti i loro calcoli.
 * <li> Salva nell'array dei risultati gli ultimi risultati computati, e fa terminare i processi fi glio passando il comando di terminazione 'K'.
 * <li> Attende che tutti i figli abbiano terminato.
 * <li> Stampa su un file di output tutti i risultati.
 * <li> Libera eventuali risorse.
 * <li> Esce.
 * </ol>
 * 
 */

void main(int argc, char *argv[]){
  register int j,i = 0;
  int fd,n_proc, lines = -1;
  int available_workers, remaining_work; /*Positions of the counting semaphores.*/ 
  int *proc_ids; /*Array of the ids of the workers.*/
  float *results; /*Array of results.*/
  operation *operations; /*Array of operations.*/
  int *offsets; /*Array of offsets.*/
  key_t mem_key1,mem_key2, sem_key; 
  pid_t status;
  int id;
  char buf[64];
  char *conf_file; /*String that contains the configuration file name.*/
  char *filename;  /*String that contains the results file name.*/
  
  if(argc > 2){
    exit(1);
  }
  else{
    if(argc == 1){
      conf_file = prompt_user("Insert file name: ");
    }
    
    else
      conf_file = argv[1];
    
    if((fd = open(conf_file,O_RDONLY, S_IRUSR)) == -1)
      syserr("fd");
    
    if(argc == 1)
      free(conf_file);
    
    lines = count_lines(fd);     /*Initializes the number of operations.*/
    n_proc = read_integer(fd);    /*Initializes the number of child processes.*/
    sprintf(buf, "Main process is spawning %d child%s for %d operations.\n",n_proc,(n_proc-1)?"ren":"",lines);
    print_to_video(buf);

    /*Initializes the positions of the counting semaphores(available workers and remaining work )*/
    available_workers = 2*n_proc;  
    remaining_work = 2*n_proc +1;

    proc_ids = (int *)malloc(lines * sizeof(int));    
    /*Initializes array of operations and offsets.*/
    init_sh_mem(&mem_key1,&mem_key2, &operations,&offsets,lines,n_proc);
    /*Stores the read information.*/
    copy_operations(fd, proc_ids,  &operations,lines);
    /*Initializes the array of semaphores.*/
    init_sem(&sem_key, n_proc, lines);
    
    for(i = 0;i < n_proc;++i){
      if((status = fork()) == -1)
	syserr("fork");

      if(status == 0){
	break;
      }
    }
    
    if(status == 0){
      
      start(2*i, offsets + i, operations, available_workers, remaining_work);
     
      exit(0);
    }
  
    /*Assigns the operations.*/
    for(i= 0; i < lines; ++i){
      id = (proc_ids[i]-1);
      
      if(id == -1){/*Finds the first free child process.*/
	P(available_workers);/*Waits if none available.*/
	V(available_workers);
	j = 0;
	while(get_sem_val((2*j+1)) == 0){/*Looks for a free child process.*/
	  j = (j +1)%n_proc;
	}
	id = j;
      }
      
      P(2*id+1);/*If busy waits.*/
      *(offsets +id) = i;/*Assigns the right offset for the child.*/
      V(2*id);/*Wakes up the process.*/
    }
    
    wait_results(remaining_work);/*Waits until all the operations are done.*/
       
    results = get_results(operations, lines);/*Stores the results int the array.*/
    char *file_name = "res.txt";
    if((fd = open(file_name, O_WRONLY|O_CREAT, 0666)) == -1){
      syserr("open res.txt");
    }
    for(i = 0; i < lines; ++i){
      char string[64];
      sprintf(string, "%f\n", *(results+i));
      print_to_file(fd, string, strlen(string));/*Stores the results in the file.*/
    }

    operations[0].op = 'K';/*Assigns the operation Kill.*/
    
    for(i = 0; i < n_proc; ++i){/*Kill every child process.*/
      *(offsets +i) = 0;/*Assigns the operation.*/
      V(2*i);/*Wakes up the child process.*/
    }
    
    for(i = 0; i < n_proc; ++i)
      wait();
    
    free(proc_ids);
    free(results);

    shmctl(mem_id1, IPC_RMID, 0);
    shmctl(mem_id2, IPC_RMID,0);
    semctl(sem_id, 0, IPC_RMID);
  }
}
