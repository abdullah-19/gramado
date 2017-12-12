/*
 * File: thread.c
 *
 * Descri��o:
 *     TM - Thread Manager.
 *     Rotinas com threads. 
 *     Criar, deletar ...
 *
 * Obs: As threads podem acessar o espa�o virtual do processo ao qual 
 * pertencem. mas n�o podem acessar mem�ria de outro processo.
 *
 * Obs: Nesse arquivo h� uma mistura de inicializa��o independente de
 * de arquitetura e inicializa��o da arquitetura i386. Essas inicializa��es
 * precisam estar em arquivos e diret�rios diferentes.
 * 
 * Historico:
 *     Vers�o: 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o: 1.0, 2016 - Revis�o.
 */


#include <kernel.h>


/*
 * Uma forma de proteger a estrutura de thread � deixa-la aqui restringindo o acesso � ela.
 *
 */

//
// Vari�veis internas.
// 

//int threadmanagerStatus;
//int threadmanagerError;
//...
 
 
/*
struct thread_d *threadNew();
struct thread_d *threadNew()
{
    struct thread_d *New;	
	
	//@todo: Implementar.
	//Cria uma thread gen�rica e retorna o ponteiro.
	
	//
done:	
	return (struct thread_d *) New;
};
*/

/*
 * create_thread:
 *     Cria um thread para rodar em user mode. 
 *
 * @todo: O processo ao qual o thread vai ser atribu�do deve ser 
 * passado via argumento, se o argumento for nulo, ent�o usa-se o 
 * processo atual como dono do thread.
 *
 * Obs:
 *     Retorno do tipo ponteiro de estrutura.
 *     Retorna o endere�o da estrutura da tarefa.
 *     Retorna NULL se der errado.
 *
 * Obs: Essa rotina deve fazer a inicializa��o da parte independente
 * da arquitetura e chamar as rotinas referentes � inicializa���es
 * dependentes da arquitetura, que ficar�o em outro diret�rio.
 *
 * @todo: Esses argumentos presisam ser revistos. Escolher um conjunto 
 * melhor de argumentos.
 *
 * 2015, Created - Fred Nora.
 * 2016, Revis�o - FN.
 */								 
thread_descriptor_t *create_thread( struct wstation_d *window_station,
                                    struct desktop_d  *desktop,
                                    struct window_d *window,
                                    unsigned long init_eip, 
                                    unsigned long init_stack, 
									int pid, 
									char *name)
{	
    //Structures.	
	struct process_d *Process;    //Process.
	struct thread_d *Thread;      //Thread.
	struct thread_d *Empty;       //Empty slot.

	//Identificadores.
	int ProcessID;	
	int i = USER_BASE_TID;		
	
	//
	// Limits da thread atual.
	//
	// @todo: N�o sei pra que isso. A thread atual n�o importa.
	//        deletar isso. 
	//
	if(current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
		return NULL;
	};
	
	//@todo:
	//Checar se a prioridade � um argumento v�lido.
	//if( priority == 0 ){}
	
	
	//
	// Filtrar o processo ao qual a thread pertencer�.
	//
	ProcessID = (int) pid;
	
	if( ProcessID < 0 || ProcessID >= PROCESS_COUNT_MAX ){
		ProcessID = current_process;
	};

	Process = (void*) processList[ProcessID]; 		
	if( (void*) Process == NULL ){
		printf("create_thread: Process struct\n");
		refresh_screen();
		while(1){}
	};
	
	//Alocando mem�ria para a estrutura da thread.
	//Obs: Estamos alocando mem�ria dentro do heap do kernel.
	Thread = (void*) malloc( sizeof(struct thread_d) );	
	if( (void*) Thread == NULL ){
	    printf("create_thread:");
		refresh_screen();
		while(1){}
	}else{  
	    //Indica � qual proesso a thread pertence.
	    //Thread->process = (void*) Process;
	};
	
	//Nothing.
	
//Loop.	
get_next:	
	
	//BugBug: Isso pode virar um loop infinito!
	
	i++;
	if(i >= THREAD_COUNT_MAX){
		i = USER_BASE_TID;    //Recome�a o loop na base para id de usu�rios.
	};
	
	//Get empty.
	Empty = (void*) threadList[i];
    
	//Se o slot estiver ocupado.
	if( (void*) Empty != NULL ){
        goto get_next;
    }else{
		
		//Object.
		Thread->objectType = ObjectTypeThread;
		Thread->objectClass = ObjectClassKernelObjects;
	
		//c,Put in list.
		Thread->tid = (int) i;
		
		//if( Thread->tid < 0 ){
		    //fail	
		//};		
		
		Thread->ownerPID = (int) pid;  //ID do processo ao qual o thread pertence.
	    Thread->used = 1;
	    Thread->magic = 1234;
		Thread->name_address = (unsigned long) name;  //Name.   
		//@todo: Usar Thread->name. 
		//Thread->cmd @todo.

        //Thread->process = (void*) Process;

		//Procedimento de janela.
	    Thread->procedure = (unsigned long) &system_procedure;
		//Msg support. //Argumentos.
		Thread->window = NULL;        //arg1.
	    Thread->msg = 0;              //arg2.
	    Thread->long1 = 0;            //arg3.
	    Thread->long2 = 0;            //arg4.		

        //Caracteristicas.
	    Thread->type = TYPE_SYSTEM; //TYPE_IDLE;    //?? //Type...@todo: Rever. 
	    Thread->state = INITIALIZED;  
		//Apenas Initialized, pois a fun��o SelectForExecution
		//seleciona uma thread para a execu��o colocando ela no
		//state Standby.	
		
		//A prioridade b�sica da thread � igual a prioridade b�sica do processo.
		Thread->base_priority = (unsigned long) KernelProcess->base_priority; //Process->base_priority;
		Thread->priority = (unsigned long) Thread->base_priority; // priority; //A prioridade din�mica da thread foi passada por argumento.			
		
		//IOPL.
		//Se ela vai rodar em kernel mode ou user mode.
		//@todo: herdar o mesmo do processo.
		Thread->iopl = RING3;  //Process->iopl;  		
		Thread->saved = 0;                //Saved flag.	
		Thread->preempted = PREEMPTABLE;  //Se pode ou n�o sofrer preemp��o.
		
		//Heap and Stack.
	    //Thread->Heap;
	    //Thread->HeapSize;
	    //Thread->Stack;
	    //Thread->StackSize;

        //Temporizadores.  		
	    Thread->step = 0;                           //Quantas vezes ela usou o processador no total.
        Thread->quantum =  QUANTUM_BASE;            //QUANTUM_BASE
        Thread->quantum_limit = QUANTUM_LIMIT; //(9*2);  //O boost n�o deve ultrapassar o limite. QUANTUM_LIMIT			
		
		
		
        Thread->standbyCount = 0;
	    Thread->runningCount = 0;    //Tempo rodando antes de parar.
	    Thread->readyCount = 0;      //Tempo de espera para retomar a execu��o.
	    Thread->ready_limit = READY_LIMIT;
	    Thread->waitingCount  = 0;
	    Thread->waiting_limit = WAITING_LIMIT;
	    Thread->blockedCount = 0;    //Tempo bloqueada.		
	    Thread->blocked_limit = BLOCKED_LIMIT;
		
	    Thread->ticks_remaining = 1000;    //Not used now.	


	    //signal
	    //Sinais para threads.
	    Thread->signal = 0;
        Thread->signalMask = 0;	


        //
		// @todo: Essa parte � dependente da arquitetura i386.
		//        poder� ir pra outro arquivo.
		//
		
		//
		// #BUGBUG 
		// **** N�O TEMOS UMA PILHA ****
		// Precisamos de uma pilha para user mode.
		// Colocaremos uma pilha simulada para teste ..
		// isso funcionar� em apenas uma thread...
		// na proxima vai dat problema.
		//
		
		//Context.
	    Thread->ss = 0x23;    //RING 3.
	    Thread->esp = (unsigned long) init_stack;//@todo: 
	    Thread->eflags = 0x3200;
	    Thread->cs = 0x1B;                                
	    Thread->eip = (unsigned long) init_eip;  
        ////OBS: eip Aceita endere�os inv�lidos, a thread fecha nesses casos por PG fault.		
	    Thread->ds = 0x23; 
	    Thread->es = 0x23; 
	    Thread->fs = 0x23; 
	    Thread->gs = 0x23; 
	    Thread->eax = 0;
	    Thread->ebx = 0;
	    Thread->ecx = 0;
	    Thread->edx = 0;
	    Thread->esi = 0;
	    Thread->edi = 0;
	    Thread->ebp = 0;	
		
		
		//cpu.
		//Thread->cpuID = 0;
		//Thread->confined = 0;
		//Thread->CurrentProcessor = 0;
		//Thread->NextProcessor = 0;
		
		//Page Directory. (#CR3).
		//Estamos usando o page directory do processo.
		//page directory do processo ao qual a thread pertence.
		Thread->Directory = (unsigned long ) Process->Directory; 



		
		
		

		
	
		
		 
	    

		//@todo: Por enquanto as threads s�o criadas usando o diret�rio de p�ginas do kernel.
		


        //ServiceTable ..

        //Ticks ...
        //DeadLine ... 

	 

	
		
		//Thread->PreviousMode  //ring???
		
		//Thread->idealprocessornumber
		
		//Thread->event
		
  	
	
	
	
	
	    //
	    // ORDEM: O que segue � referenciado com pouca frequencia.
	    //

	    Thread->waitingCount = 0;    //Tempo esperando algo.
	    Thread->blockedCount = 0;    //Tempo bloqueada.	
	
        //� qual processo pertence a thread.  
		Thread->process = (void*) Process; 	 	                      
        
		//Thread->window_station
		//Thread->desktop
         


	

		
		//Thread->control_menu_procedure
		
		//Thread->wait4pid =

		//...
        //@todo:
        //herdar o quantum do processo.
        //herdar a afinidade do processo.(cpu affinity) 


        Thread->exit_code = 0;
	    

		//@todo: Incrementar a contagem de threads no processo.
		//Process->threadCount++;
		
		//Pr�xima thread da lista.
		Thread->Next = NULL;
		
		//Coloca na lista.
		threadList[i] = (unsigned long) Thread;	
	};

	//
	// Running tasks.
	//
	
	//ProcessorBlock.running_tasks = 2;
    //@todo: isso deve ir pra outro lugar.
	//talvez dentro de SelectForExecution.
		
//
// Done.
//
	
done:
    
	//
	// Warning !!! (N�O COLOCAR PARA EXECU��O, OUTRA FUN��O DEVE COLOCAR PARA EXECU��O)
	//
	
    //SelectForExecution(t);  //***MOVEMENT 1 (Initialized ---> Standby)
    return (void*) Thread;
};


/*
 * GetCurrentThreadId
 *     Pega o id da thread atual.
 *     Obs: current_thread j� � o id.
 */
int GetCurrentThreadId(){
	return (int) current_thread;
};


/*
 * GetCurrentThread:
 *     Retorna o endere�o da estrutura da thread atual.
 *
 */
void *GetCurrentThread()
{
    struct thread_d *Current;
	
	//Limits.
	if(current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
		return NULL;
	};
	
	Current = (void*) threadList[current_thread];	
	if( (void*) Current == NULL ){
        return NULL;
	};
//Done.
done:
	return (void*) Current;
};


/*
 * FindReadyThread:
 *     Pega a primeira thread READY que encontrar.
 *     E se n�o encontrar nenhuma, retorna NULL.
 */
void *FindReadyThread()
{
	int Index;
    struct thread_d *Thread;  
    
	for(Index=0; Index<THREAD_COUNT_MAX; Index++)
	{
	    Thread = (void *) threadList[Index];	
		
		if( (void*) Thread != NULL )
		{
            if( Thread->used == 1 && 
			    Thread->magic == 1234 && 
				Thread->state == READY )
			{
				//Done.
	            return (void *) Thread;
			};
			//Nothing.
	    };
        //Nothing.		
	};
//Nenhuma foi encontrada.   
fail:
    return NULL;
};



/*
 * SelectForExecution:
 *     Um thread entra em standby, sinalizando que est� pronto para entrar 
 * em execu��o.
 *     Nesse caso, durante a rotina de taskswitch, checar-se-a se existe 
 * um thread em estado standby, caso haja, a thread � colocada pra executar 
 * pelo m�todo spawn. Esse m�todo de spawn j� foi testado, segundo a contagem,
 * duas thread come�aram a rodas atrav�s desse m�todo de spawn. provavelmente 
 * as threads 'shell' e 'taskman', pois a thread 'idle' � chamada com um 
 * spawn exclusivo para ela, o que � desnecess�rio e poder� ser revisto. @todo
 *     
 *  *** MOVIMENTO 1, (Initialized --> Standby).
 */
void SelectForExecution(struct thread_d *Thread)
{ 
	if( (void*) Thread == NULL){
        return;
	};  

	//
	// @todo: if initialized ---> Standby.
	// @todo: if zombie ---> Standby.
	//
	// Talvez aqui seja necess�rio checar o estado da thread.
	// Quem pode entrar no estado standby??
	// >> Uma thread no estado initialized pode entrar no estado standby 
	// >> Uma thread no estado zombie pode entrar no estado standby.
	// >> @todo: se uma thread estiver em qualquer um dos outros estados ela 
	// n�o pode entrar em stadby.
	//
	
setState:
    //*MOVIMENTO 1, (Initialized --> Standby).
    Thread->state = (int) STANDBY;
	queue_insert_data(queue, (unsigned long) Thread, QUEUE_STANDBY);	
	return;
};


//Get State. (Zero � tipo NULL?).
int GetThreadState(struct thread_d *Thread)
{ 
	if( (void*) Thread == NULL){
        return (int) 0;
	};  
	return (int) Thread->state;
};

//Get Type. (Zero � tipo NULL?).
int GetThreadType(struct thread_d *Thread)
{
	if( (void*) Thread == NULL){
        return (int) 0;
	};  
    return (int) Thread->type;
};



/*
 * show_thread_information:
 *     Mostra informa��es sobre as threads.
 */
void show_thread_information()
{
	struct thread_d *Current;	
	
	printf("Threads info:\n\n");		
	
	//Limits.
	if(current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
		return;
	};	
	
	Current = (void*) threadList[current_thread];
	if( (void*) Current == NULL){
	    printf("show_thread_information:\n");	
	    return;
	}else{
	    
		printf("CurrentThreadId={%d}\n",current_thread);
		//...
	};

    mostra_slots();    //Mostra Slots.
	
	/*
	 * @todo: 
	 *     Mostra filas: Ready, Waiting ...
	 *     checar estruturas de filas no debug.
	 *     Erro: Mostrar filas n�o deve fazer parte dessa rotina.
	 */
	//show_queue_information(queue);
	
	//Nothing for now!
	
//Done.	
done:	
    printf("Done.\n");
	return;
};



/*
 * init_threads:
 *     Inicializa o thread manager.
 *     Inicializa as estruturas e vari�veis 
 *     que lidam com threads.
 *
 */
int init_threads()
{ 
	//Globais.	 
	current_thread = 0;                        //Atual. 
	ProcessorBlock.running_threads = (int) 0;    //N�mero de tarefas rodando.	
	old = 0;                                   //?
    forkid = 0;                                //
    task_count = (unsigned long) 0;            //Zera o contador de tarefas criadas.
	//...
	
	//
	// @todo: Porque essas vari�veis usam o termo 'task'?
	//        task � sinonimo de process.
	//	
	
	//Vari�veis usadas na inicializa��o de uma nova tarefa.	
	start_new_task_status  = (unsigned long) 0;    //Se h� uma nova tarefa.
	start_new_task_id = (int) 0;                   //Id d� nova tarefa.
	start_new_task_address = (unsigned long) 0;	   //Endere�o da nova tarefa.
	
	//@todo: H� mais vari�veis para serem inicializadas??!!
	
	
	//Zerando a lista de threads.
	int i=0;
	while(i < THREAD_COUNT_MAX){
	    threadList[i] = (unsigned long) 0;    //NULL.
        ++i;
	};
	
	//
	// @todo: Nada mais?
	//
	
//Done.	
done:
	return (int) 0;
};


/*
 *Constructor.
int threadmanagerProcessmanager()
{};
*/

/*
int threadmanagerInit()
{};
*/


//
//fim.
//

