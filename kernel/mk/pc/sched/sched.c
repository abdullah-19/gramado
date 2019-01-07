/*
 * File: sched/sched.c
 *
 * Descri��o:
 *     O escalonador de processos do Kernel.
 *     Faz parte do Process Manager, parte fundamental do Kernel Base.
 *     The kernel schedules threads, not processes.
 *     Priority: +boosts for GUI foreground.
 *
 * Atribui��es:
 *     + Salva o contexto da tarefa OLD.
 *     + Incrementa a tarefa atual. (RR).
 *     + Retorna o contexto da tarefa NEW.
 *
 * Obs:
 *     O Scheduler n�o salva nem restaura contexto de tarefa.
 * Apenas troca a tarefa atual e faz um agendamento das pr�ximas
 * threads a rodarem.
 *     Ser� poss�vel usar mais de um m�todo de escalonamento. No momento,
 * o m�todo usado � escalonamento Round Robin, preemptivo,
 * com m�ltiplas filas e prioridade.
 *
 * @todo:
 *     N�o por interface nessa rotina. Perde velocidade.
 *
 * Hist�rico:
 *     Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o 1.0, 2016 - Aprimoramento geral das rotinas b�sicas.
 *     //...
 */

 /*
  * #importante:
  * Para implementarmos um scheduler como um processo, devemos separar 
  * as fun��es em fun��es de biblioteca e fun��es primitivas, que ser�o 
  * aquelas presentes no stub do scheduler. As fun��es primitivas 
  * ser�o as mesmas para todos os schedulers que criarmos.
  */
 

#include <kernel.h>


//
// Vari�veis internas.
//

//int schedulerStatus;
//int schedulerError;
//int schedulerType;
//...

//unsigned long schedulerQueue[4];



/*
 ***************************************************************
 * scheduler:
 *    Troca a thread atual, escolhe uma nova thread atual 
 * para rodar no momento.
 *    O m�todo � cooperativo, Round Robing.
 *
 * Ordem de escolha:
 * ================
 *  +fase 1 - Pega a pr�xima indicada na estrutura.
 *  +fase 2 - Pega nos slots a de maior prioridade.
 *  +fase 3 - Pega a Idle. 
 *            @todo: Nessa fase devemos usar a idle atual, 
 *            indicada em current_idle_thread.  
 *  //...
 *
 * Obs:
 *     O que estamos fazendo aqui � incrementar a tarefa atual e olhando se a
 * pr�xima tarefa da lista threadList[] est� pronta pra rodar.
 *
 * Obs:
 *     Pega na fila ReadyQueue.
 *     O scheduler deve sempre pegar da fila do dispatcher.
 *
 */
 
int scheduler (){
	
	int Index;
	struct thread_d *Thread;
	
	
	debug_print(" [*SCHEDULER*]");

	// spiritual quote:
	// "Constr�i um caminho de vag�es para o condutor andar".

	//Usado para task switch.
	//Conductor = (void *) rootConductor;
	//Inicia a lista.
 	//Conductor2 = (void *) rootConductor;
	//Next thread.
	//Conductor2->Next = (void *) threadList[next_thread];  
	
	
	//#importante 
	//rootConductor � um ponteiro v�lido,
	//mas a estrutura n�o foi inicializada.
	
	//Inicia a lista.
	//marca o in�cio, mas n�o usa esse ponteiro.
 	Conductor2 = (void *) rootConductor;
	
	
	//
	//  ## PRIMEIRO PONTEIRO V�LIDO ##
	//
	
	//Next thread.
	Conductor2->Next = (void *) threadList[next_thread];  
	
	//
	//  ## MARCADOR GLOBAL ##
	//
	
	
	//TID=0
	//vamos come�ar a lista dessa aqui.
	Conductor = (void *) Conductor2->Next;	
	
	
	// Obs: 
	// ## IMPORTANTE  ##
	// Os primeiros tipos a se pegar s�o os de 
	// prioridade maior.
	// @todo: Elevar a prioridade da threads interativas,
	// como teclado e mouse, e n�o elevar das threads de 
	// i/o de disco.
	// Elevar a prioridade da thread associada a janela 
	// com o foco de entada.

	// Obs: 
	// ## IMPORTANTE  ##	
	// A thread idle somente � usada quando o sistema 
	// estiver ocioso ou quando ela for a �nica thread.
	// E � importante que a thread idle seja usada, pois 
	// ela tem as instru��es sti/hlt que atenua a utiliza��o 
	// da CPU, reduzindo o consumo de energia.

	// Agora, antes de tudo, devemos pegar as threads 
	// nas listas onde est�o as threads de maior prioridade.

	//Encontra o id da thread de maior prioridade entre as 
	// threads que est�o no estado READY.
	//KiFindHigherPriority();

	// ## Importante  ##
	// Daqui pra baixo pegaremos na lista threadList[] 
	// onde est�o todas as threads.

    //@todo pegar primeiro por prioridade.	

	/*	
    //BLOCKED
	for(Index=0; Index <= THREAD_COUNT_MAX; Index++)
	{
		Thread = (void*) threadList[Index];
		if( (void*) Thread != NULL)
		{
			if(Thread->used == 1 && Thread->magic == 1234 && Thread->state == BLOCKED )
			{
				//ACORDA
				Thread->state = READY;
			    Conductor2->Next = (void*) Thread;
                Conductor2 = (void*) Conductor2->Next;
			};
			
		};
	};

	//WAITING
	for(Index=0; Index <= THREAD_COUNT_MAX; Index++)
	{
		Thread = (void*) threadList[Index];
		if( (void*) Thread != NULL)
		{
			if(Thread->used == 1 && Thread->magic == 1234 && Thread->state == WAITING )
			{
				//ACORDA
				Thread->state = READY;
			    Conductor2->Next = (void*) Thread;
                Conductor2 = (void*) Conductor2->Next;
			};
			
		};
	};

	*/

	//READY.
	for ( Index=0; Index < THREAD_COUNT_MAX; Index++ )
	{
		Thread = (void *) threadList[Index];
		
		if ( (void *) Thread != NULL )
		{
			if ( Thread->used == 1 && 
			     Thread->magic == 1234 && 
				 Thread->state == READY )
			{
			    Conductor2 = (void *) Conductor2->Next; 
				Conductor2->Next = (void *) Thread;
			};
		    //Nothing.
		};
		//Nothing.
	};

	/*
	//READY. again (de traz pra frente.
	for(Index=THREAD_COUNT_MAX; Index >= 0; Index--)
	{
		Thread = (void*) threadList[Index];
		if( (void*) Thread != NULL)
		{
			if(Thread->used == 1 && Thread->magic == 1234 && Thread->state == READY )
			{
			    Conductor2->Next = (void*) Thread;
                Conductor2 = (void*) Conductor2->Next;
			};
			
		};
	};	

	*/

	/*
    //RUNNING
	for(Index=0; Index <= THREAD_COUNT_MAX; Index++)
	{
		Thread = (void*) threadList[Index];
		if( (void*) Thread != NULL)
		{
			if(Thread->used == 1 && Thread->magic == 1234 && Thread->state == RUNNING )
			{
				//ACORDA
				Thread->state = READY;
			    Conductor->Next = (void*) Thread;
                Conductor = (void*) Conductor->Next;
			};
			
		};
	};
    */

	//Conductor2->Next = NULL;

	//
	// Continua pegando os tipos diferente.
	//


	//finaliza a lista
	Conductor2 = (void *) Conductor2->Next; 
	Conductor2->Next = NULL;

    return (int) Conductor2->tid;
};


/*
 ***************************************************
 * scheduler_start:
 *     +Inicializa o sheduler.
 *     +Trava o scheduler.
 *     +Torna IDLE a tarefa atual.
 *     //...
 *
 *     @todo: nao seria init_scheduler
 *     @todo: Mudar para schedulerInit,ou schedulerStart. ??
 */

void scheduler_start (){
	
	//Lock Scheduler.
	//Set current.
	
    scheduler_lock ();  

    // #bugbug 
	// Esse neg�cio de selecionar pelo impondo um ID d� problemas.
	// Temos que saber qual � o ID da thread que queremos.
	
	set_current (IDLE);     
	
	//...
};


/* scheduler_lock: */
void scheduler_lock (){
	
    g_scheduler_status = (unsigned long) LOCKED;
};


/* scheduler_unlock: */  
void scheduler_unlock (){
	
    g_scheduler_status = (unsigned long) UNLOCKED;
};


/*
 * scheduler_get_status:
 *     Pega o status do scheduler, se ele est� travado ou n�o.
 */
 
unsigned long scheduler_get_status()
{
    return (unsigned long) g_scheduler_status;
};


/*
 * new_task_scheduler: 
 *     ?? #deletar
 */
void new_task_scheduler()
{   
    return;    /* CANCELADA !*/  		
};	


/*
 * init_scheduler:
 *    Inicaliza o scheduler.
 *    @todo: Mudar para schedulerInit()
 *    #burbug: faz o mesmo que scheduler_start.
 */
//void schedulerInit()
void init_scheduler (){
	
	// @todo: Implementar inicializa��o de variaveis do scheduler.
	//        O nome poderia ser schedulerInit().
	//        Formato de classes.Init � um m�todo. 

//	return;    //N�o implementada.
};


/*
 * constructor.
int schedulerScheduler(){
	;
};
*/

/*
int schedulerInit(){
	;
};
*/


//
// End.
//

