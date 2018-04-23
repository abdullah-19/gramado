/*
 * Gramado Microkernel - The main file for the Microkernel module in the 
 * kernel base. 
 * (c) Copyright 2015 Fred Nora.
 *
 * File: k\microkernel.c 
 * 
 * Classes:
 *     system.io.cpu (K4)
 *     system.io.dma (K3)
 *
 * Obs: O microkernel lida especialmente com canais de transmiss�o
 *      entre a RAM e os dispositivos. Seu ambiente natural favorece
 *      o escalonamento de threads e processos. Atua como uma ponte, 
 *      decidindo quem vai ou volta da mem�ria ram.
 *      Quando o microkernel escalona uma thread, ele determina quando
 * � a hora dela ir pra mem�ria ram para que o processador atue sobre ela.
 *      O cpu tambem trabalha as vezes levando informa��o dos dispositivos
 * para a ram e da ram para os dispositivos. Um trabalho semelhante � feito
 * pelo dma, atrav�s de seus canais.
 *      � aceit�vel a presen�a de c�digo em assembly nessa camada. Principalmente
 * relativos a configura��o de cpu e dma.      
 *
 * Descri��o:
 *    Arquivo principal do microkernel do kernel.
 *    MB - M�dulos inclu�dos no Kernel Base.
 *    O microkernel trata do sistema de multitarefas, compreendendo 
 * principalmente IPC, process, threads, Irqs.
 *    The microkernel also serves as a socket for plugging in these 
 * extensions and coordinating their collaboration (ipc).
 *
 * In this file:
 *   + init_microkernel - Inicia o microkernel.
 *   + ...
 *
 * @todo:
 *     Os servi�os oferecidos por esse m�dulo aparecer�o aqui 
 * come�ando por 'sys_'.
 *
 * microkernelMicrokernel(); � o construtor.
 *
 * Vers�o 1.0, 2015.
 */

 
#include <kernel.h>
//#include "mkidata.h"  //MicroKernel Internal Data.

//unsigned char  MicrokernelName[] = "MICROKERNEL LAYER";



#define LOCALSTUFF1 0
int localsstuff1;




/*
 * microkernelTestLimit:
 *     Rotina de testes variados de limites nos recursos
 * do microkernel. Como a cria��o de trheads e processos.
 * Obs: Essa rotina deve acionada pelo procedimento de 
 * janela do sistema. Em user mode os programas de teste criar�o 
 * suas pr�prias rotinas. 
 *
 */
void microkernelTestLimit()
{
    //�ndice gen�rico.
	int i;
	struct process_d *p;
	struct thread_d *t;
	
	
	
	printf("microkernelTestLimit: Testando limites dos componentes do microkernel...\n");  
    refresh_screen();
	
   //
   // Criando todos os processos poss�veis.
   //
testProcess:   
    printf("Creating processes...\n");      
    i=128;	
    while(i<PROCESS_COUNT_MAX)
	{
		p = (void*) create_process( NULL, NULL, gui->screen, 0x400000, DISPATCHER_PRIORITY_LOW, 0, "TestLimits", RING3, (unsigned long ) KERNEL_PAGEDIRECTORY);	    
		if((void*)p == NULL ){
			goto testThread;
		}
		i++;	
	}
      

   //
   // Criando todas as threads poss�veis.
   //	
testThread:	  
/* 
    printf("Creating threads...\n");      	  
    i=128;
    while(i<THREAD_COUNT_MAX)
	{
		t = (void*) create_thread( NULL, NULL, gui->screen, 0x400000, DISPATCHER_PRIORITY_LOW, 0, "TestLimits");  
		if((void*)t == NULL ){
			goto done;
		}	    
		i++;	
	}
  
*/  
   //Nothing for now.
   
done:
    printf("microkernelTestLimit: Done.\n");   
	return;
};



void sys_dead_thread_collector(){
    dead_thread_collector();
	return;
};


//exit process.
//servi�o do sistema.
void sys_exit_process(int pid, int code){
	exit_process(pid,code);
	return;
};

//exit thread.
void sys_exit_thread(int tid){
    exit_thread(tid);
    return;
};


/*
 * sys_create_process:
 *     Servi�o do sistema.
 *     Interface para cria��o de um processo.
 */
void *sys_create_process( struct wstation_d *window_station,
                          struct desktop_d  *desktop,
                          struct window_d *window,
                          unsigned long init_eip, 
                          unsigned long priority, 
					      int ppid, 
					      char *name,
						  unsigned long iopl, 
						  unsigned long directory_address )
{
    //
	// @todo: Create some interface routine.
	//
	
	//@todo: Filtros para ponteiros NULL.
	
	// Create process.
    create_process( window_station, desktop, window, init_eip, 
	                priority, ppid, name, iopl, 
					(unsigned long) directory_address);
	
	//@todo: Return da fun��o create.
	
done:
    return NULL;
};


/*
 * sys_create_thread:
 *    Servi�o do sistema.
 *    Interface para cria��o de uma thread.
 */
void *sys_create_thread( struct wstation_d *window_station,
                         struct desktop_d  *desktop,
                         struct window_d *window,
                         unsigned long init_eip, 
                         unsigned long priority, 
					     int ppid, 
					     char *name )
{
    //
	// @todo: Create some interface routine.
	//
	
	//@todo filtros, para ponteiros NULL.
	
	
	// Create thread.
    create_thread( window_station, desktop, window, init_eip, 
	               priority, ppid, name); 
				   
    //@todo: return da fun��o create.
	
done:
    return NULL;
};


/*
 * sys_fork:
 * Fork ... Servi�o do sistema.
 */
int sys_fork(){
	return (int) fork();
};


/*
void sys_reboot(void)
{
    KiReboot();
	return;
};
*/


//Pega o id do processo atual.
int sys_getpid(){
	return (int) current_process;
};


//Pega o ID do processo pai do processo atual.
int sys_getppid()
{
    int pid;
	int ppid;
	struct process_d *p;
	
	pid = (int) current_process;
	
	if( pid >= 0 && pid < PROCESS_COUNT_MAX )
	{
        //Ponteiro da estrutura.
		p = (void*) processList[pid]; 		
		
		//erro.
		if( (void*) p == NULL ){
			return (int) -1;
		}
		
		//erro.
		if ( p->used != 1 || p->magic != 1234 ){
		    return (int) -1;	
		}
		
		//Retorna o id do processo pai.
		return (int) p->ppid;
	};
	
//fail.
fail:
    return (int) -1;	
};


/*
 **********************************************************************
 * KeReboot:
 *     Uma interface para chamar um servidor em user mode que realize a 
 * rotina de reboot. 
 */
void KeReboot()
{
	//
	// @todo
	//
	
    return;
};


/*
 * init_microkernel:
 *     Initialize the kernel base microkernel.
 *     Archtecture independent inicialization ...
 *
 * In these function:
 *    + Init timer.
 *    + Init tasks. (process)
 *    + Init IPC.
 *    + Init semaphore.
 *    + Lock the scheduler.
 *
 * @todo: Mudar para microkernelInit().
 *
 */
//int microkernelInit() 
int init_microkernel()
{
    int Status = 0;
	
    printf("MICROKERNEL:\n");

    // Lock task switch and lock scheduler. 
	set_task_status(LOCKED);
	scheduler_lock();
	
    // Init scheduler.
	init_scheduler();
	
	// Init processes, threads, 
	init_processes();
	init_threads();    
	
	// Init IPC and Semaphore.
	init_ipc();
    create_semaphore(); //@todo: criar fun��o.
	

	//
	// Queue.
	// 
	
	//Inicializar as filas que alimentar�o a lista do dispatcher.	
	queue = malloc( sizeof( struct queue_d ) );
	if( (void*) queue == NULL ){
	    panic("init_microkernel: queue\n");
	    die();
	}else{
		
		//Inicializa todas as filas do microkernel.
	    init_queue(queue);
		
		// Initializing the dispatcher list.
		init_dispatcher();
		//...
	};
	
	//...
	
	//
	// Dispatch Count Block.
	//
	
	DispatchCountBlock = malloc( sizeof( struct dispatch_count_d ) );
	if( (void*) DispatchCountBlock == NULL ){
	    panic("init_microkernel: DispatchCountBlock\n");
	    die();
	}else{
		
		DispatchCountBlock->SelectIdleCount = 0;
        DispatchCountBlock->SelectInitializedCount = 0;
		DispatchCountBlock->SelectNextCount = 0;
		DispatchCountBlock->SelectCurrentCount = 0;
		DispatchCountBlock->SelectAnyCount = 0;
		DispatchCountBlock->SelectIdealCount = 0;
		DispatchCountBlock->SelectDispatcherQueueCount = 0;
		//...
	};

    //More?!

// Done.
Done:
    Initialization.microkernel = 1;

#ifdef KERNEL_VERBOSE
    printf("Done!\n");
#endif

    return (int) Status;
};


//
// End.
//

