/*
 * File: threadi.c 
 *
 * Descri��o:
 *      Thread internal.    
 *
 *     'Ki_' � para rotinas com threads oferecidas
 *     pelo modulo interno dentro do kernel base.
 *
 *     O kernel acessa as rotinas envolvendo threads
 *     atrav�z dessas chamadas.
 *
 *     Faz parte do Process Manager, 
 *     parte fundamental do Kernel Base. 
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2018 - Revision.
 */

 
#include <kernel.h>



/*
 * KiShowPreemptedTask
 * @todo: Substituir a palavra task por thread. KiShowPreemptedThread
 *
 */
void KiShowPreemptedTask()
{
    //return;
};

 
/*
 * KiSetTaskStatus:
 *     @todo: Substituir a palavra task por thread. KiSetThreadStatus
 */ 
void KiSetTaskStatus(unsigned long status)
{
    //@todo: criar interface para mudanca de status.
	
	set_task_status(status);
};


/*
 * KiGetTaskStatus
 * @todo: Substituir a palavra task por thread. KiGetThreadStatus
 * #bugbgu task n�o � um termo usado.
 */
unsigned long KiGetTaskStatus ()
{ 
    return (unsigned long) get_task_status(); 
};



/*
 * KiSaveContextOfNewTask
 * @todo: Substituir a palavra task por thread. KiSaveContextOfNewThread
 *
 * ?? isso est� muito estranho !!
 */
void 
KiSaveContextOfNewTask( int id, unsigned long *task_address )
{
    //return;
};


/*
 * KiReloadCurrentTask
 *  @todo: Substituir a palavra task por thread. KiReloadCurrentThread.
 *
 */
void KiReloadCurrentTask (){
	
    //return;
};


void KiSetQuantum (unsigned long q){
    
    //return;
};


unsigned long KiGetQuantum (){
	
    return (unsigned long) 0; 
};


void KiSetCurrentQuantum (unsigned long q){
	
    //return;
};


unsigned long KiGetCurrentQuantum (){
	
    return (unsigned long) 0; 
};


void KiSetNextQuantum ( unsigned long q ){
	
    //return;
};


unsigned long KiGetNextQuantum (){
	
    return (unsigned long) 0; 
};


void KiSetFocus (int pid){
	
	//return;
};


//Pegar o foco do que? thread?!
int KiGetFocus (){
	
    return (int) 0; 
};


void KiDebugBreakpoint (){
	
    //return;
};


void KiShowTasksParameters (){
	
    //return;
};


void KiMostraSlots (){
	
	mostra_slots();
};


void KiMostraSlot (int id){
	
	mostra_slot(id);
};


void KiMostraReg (int id){
	
	//mostra_reg(id);
    //return;
};


/*
 ***************************************
 * KiShowThreadList:
 *     Mostra os parametros de ALGUMAS das 
 * threads existentes em 
 * threadList[i]. (as primeiras da lista).
 */
void KiShowThreadList (){
	
    mostra_slots();
};


/*
 *****************************************
 * mostra_slots:
 *
 * @todo:
 * Obs: Estamos mostrando informa��es sobre todos 
 * os processos e todas threads.
 * Por�m esse arquivo � para lidar com threads, 
 * ent�o a torina de lidar com processos
 * deve ir pra outro arquivo.
 * @todo: Mudar nome.
 * #bugbug: N�o encontro o prot�tipo dessa fun��o.
 */
//void threadiShowSlots(){ 
void mostra_slots(){
	
    int i; 	
	struct process_d *p;    
    struct thread_d  *t;   


	//
	// Testando o for para process.
	//
	
	/*
	printf(" \n\n ** Process info ** \n\n");
	
	for( i=0; i<PROCESS_COUNT_MAX; i++)
    {
	    p = (void *) processList[i];
	    
		//Mostra as tarefas v�lidas, mesmo que estejam com problemas.
		if( (void*)p != NULL && 
		        p->used == 1 && 
				p->magic == 1234 )
	    {
			//@todo: Mostrar quem � o processo pai.
		    printf("PID={%d} Directory={%x} Name={%s} \n",p->pid ,p->Directory ,p->name_address);
	    };
    };
	*/
	
	//
	// Testando o for para threads.
	//
	printf("\n ## Thread info ## \n");
	
Scan:	
	for( i=0; i<THREAD_COUNT_MAX; i++)
    {
	    t = (void *) threadList[i];
	    
		//Mostra as tarefas v�lidas, mesmo que estejam com problemas.
		if( (void*)t != NULL && 
		        t->used == 1 && 
				t->magic == 1234 )
	    {
			//@todo: Melhorar isso.
		    printf("TID={%d} Step={%d} ownerPID={%d} pHandle={%x} State={%d}" 
			    "Handle={%x} Name={%s}\n\n",
		        t->tid, t->step, t->ownerPID, t->process, 
				t->state, (void*) t, t->name_address );
	        //...
		};
    };
    
	
	
done:	
    printf("Done\n");
	
	//#debug
	//refresh_screen();
	//while(1){}
    return;
};


/*
 *****************************************************
 * mostra_slot:
 *     Mostra as variaveis mais importantes de um slot.
 *     obs: N�o precisa mostrar o contexto, tem rotina pra isso.
 *     @todo: Mudar nome.
 *     #bugbug: N�o encontro o prot�tipo dessa fun��o. 
 */
//void threadiShowSlot(int id){  
void mostra_slot (int id){
	
    struct thread_d *t;

	// Limits.
    if ( id < 0 || id >= THREAD_COUNT_MAX ){
		
	    printf("pc-threadi-mostra_slot: id\n");
		goto fail;
	};
	
	
	// Structure.
	t = (void *) threadList[id];
	
	if( (void*) t == NULL )
	{
	    printf("pc-threadi-mostra_slot: t\n");
		goto fail;	
	}else{
	
	    // Show one slot.
	    printf("\n");
	    printf("TID  ownerPID pHandle  Prio  State   tName   \n");
	    printf("===  ======== ======== ====  =====   ======= \n");
        printf("%d   %d       %x       %d    %d      %s      \n" ,t->tid 
	                                                             ,t->ownerPID
                                                                 ,t->process															
														         ,t->priority 
														         ,t->state
														         ,t->name_address);
	};
	
    goto done;

	
fail:	
    printf("fail\n");	
done:
    return; 
};


/*
 *************************************************
 * mostra_reg:
 *    Mostra conte�do dos registradores de uma thread..
 *
 *    eflags
 *    cs:eip	
 *    ss:esp	
 *    ds,es,fs,gs
 *    a,b,c,d
 *
 *    @todo Mudar nome
 */
//void threadiShowRegisters (int id){   
void mostra_reg (int id){
	
    struct thread_d *t; 

	// Limits.
    if ( id < 0 || id >= THREAD_COUNT_MAX ){
		
	    goto fail;
	}
	
	// Structure.
    t = (void *) threadList[id];
	
	if ( (void *) t == NULL ){
		
	    goto fail;
	}
    	
	// Show registers.	
    printf("\n");
	printf("eflags=[%x]\n", t->eflags);
	printf("cs:eip=[%x:%x]\n", t->cs, t->eip );
	printf("ss:esp=[%x:%x]\n", t->ss, t->esp ); 
    
	printf("ds=[%x] es=[%x] fs=[%x] gs=[%x]\n",
	    t->ds, t->es, t->fs, t->gs );
	
	printf("a=[%x] b=[%x] c=[%x] d=[%x]\n",
	    t->eax, t->ebx, t->ecx, t->edx ); 
    
	goto done;

	
fail:	
    printf("fail\n");	
done:
    return; 
};


/*
 *******************************************************
 * set_thread_priority:
 *     Muda a prioridade de uma thread espec�fica.
 */
void set_thread_priority ( struct thread_d *t, unsigned long priority ){
	
    unsigned long ThreadPriority;
	
	if ( (void *) t == NULL )
	{
	    return;
		
	}else{
		
        if ( t->used != 1 || t->magic != 1234 ){
		    return;
	    }	
		//...
	};
	
	ThreadPriority = t->priority;

	// se aprioridade solicitada for igual da prioridade atual.	
	if ( priority == ThreadPriority ){
		return;
	}
	
do_change:
	
	// se aprioridade solicitada for diferente da prioridade atual.
	if ( priority != ThreadPriority )
	{
		//Muda a prioridade.
        t->priority = priority;
		
		/*
		switch(t->state) 
		{
		    case READY:
		        //@todo if(priority < ThreadPriority){};
				break; 

		    case RUNNING:
			    //@todo
				//if( (void*) t->Next == NULL )
				//{
				//	if(priority < ThreadPriority){
					    //@todo: encontra e prepara uma outra tarefa.
				//	};
				//};
		        break; 
			    //Nada para os outros estados.
		    default:
			    //Nothing for now.
			    break;
		};
		*/
		
    };
//Done.
done:
    return;	
};


/*
 ************************************************************
 * SetThreadDirectory:
 *     Altera o endere�o do diret�rio de p�ginas de uma thread.
 *     Apenas a vari�vel. N�o altera o CR3.
 */
void SetThreadDirectory ( struct thread_d *thread, unsigned long Address ){
	
    if ( (void *) thread == NULL )
	{
        goto fail;
        
	}else{
		
		//@todo:
		//Aqui podemos checar a validade da estrutura,
		//antes de operarmos nela.
	};
	
//Nothing.		
done:
	thread->Directory = (unsigned long) Address;	
fail:
	return;
};


/*
 ***********************************************************
 * GetThreadDirectory:
 *     Pega o endere�o do diret�rio de p�ginas de uma thread.
 */
unsigned long GetThreadDirectory ( struct thread_d *thread ){
	
    if ( (void *) thread == NULL )
	{
        goto fail;        
	}else{
		
		//@todo:
		//Aqui podemos checar a validade da estrutura,
		//antes de operarmos nela.
	};
//Nothing.	
done:
	return (unsigned long) thread->Directory;
fail:
    return (unsigned long) 0;    
};


/*
 * show_preempted_task: 
 *
 *    Mostrar informa��es sobre a tarefa de baixa prioridade
 *    que teve seu contexto salvo e deu a vez pra outra de
 *    maior prioridade.
 *
 *    @todo: Criar uma variavel global que identifique
 *           a tarefa com contexto salvo.
 *
 */
void show_preempted_task(){
    
	//return;
};


void show_tasks_parameters(){  
	
	//return; 
};



/*
 ****************************************
 * release:
 * #importante
 * Isso deve liberar uma thread que estava esperando 
 * ou bloqueada por algum motivo.
 * Obs: Aqui n�o dvemos julgar se ela pode ou n�o ser 
 * liberada, apenas alteramos se estado.
 *
 */
void release ( int tid ){
	
    struct thread_d *Thread;
	
	//Limits. 
	if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
	    
		goto fail;
	}
	
	Thread = (void *) threadList[tid];
	
	if ( (void *) Thread == NULL )
	{
		goto fail;
		
	}else{
		
        //Se estiver corrompida.
        if ( Thread->magic != THREAD_MAGIC ){
			
			goto fail;
		}
		
		//#importante:
		//N�o estamos selecionando ela para execu��o
		//Apenas estamos dizendo que ela est� pronta para 
		//executar.
		Thread->state = READY; 
	};	
	
fail:
done:
    return;
};



/*
 *******************************************************
 * exit_thread:
 *     Exit a thread.
 *     Torna o estado ZOMBIE mas n�o destr�i a estrutura.
 *     Outra rotina destruir� as informa��es de uma 
 * estrutura de thread zombie.
 */
void exit_thread (int tid){
	
    struct thread_d *Thread;
	
	//Limits. 
	if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
		
	    goto fail;
	}
	
	if ( tid == idle ){
		
		goto fail;
	}
	
	Thread = (void *) threadList[tid];
	
	if ( (void *) Thread == NULL )
	{
		goto fail;
		
	}else{
		
        //Se estiver corrompida.
        if ( Thread->magic != THREAD_MAGIC ){
			
			goto fail;
		}
		
		//Thread->exit_code = 0;    //@todo: Isso � necess�rio. Onde est�.
		Thread->state = ZOMBIE; 
	};
		
	
	// # reavaliar isso.
	// Se a thread fechada � a atual, 
	// necessitamos de algum escalonamento.	
    if ( tid = current_thread ){
		
	    scheduler();
    }
	
	
fail:
//Nothing.		
done:
	return;
};


/*
 *****************************************************
 * kill_thread:
 *     Destr�i uma thread.
 *     Destroi a estrutura e libera o espa�o na lista. 
 */
void kill_thread (int tid){
	
    struct thread_d *Thread;
	
	//Limits.
	if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
		
	    goto fail;	
	}
	
	if ( tid == idle ){
		
		goto fail;
	}
	
	
	//
	// @todo: 
	//    Deve acordar o pai que est� esperando o filho fechar.
	//    Mas no caso estamos apenas fechando uma thread.
    //    Caso essa n�o seja a thread prim�ria, isso n�o deve 
	// causar o fechamento do processo.	
    //
	
	Thread = (void *) threadList[tid];
	
	if ( (void *) Thread == NULL )
	{
		goto fail;
		
	}else{
		
		
	    //@todo pegar o id do pai e enviar um sinal e acorda-lo
        //se ele estiver esperando por filho.		
        Thread->used = 0;
        Thread->magic = 0; 		
		Thread->state = DEAD; 
		//...
		
		ProcessorBlock.threads_counter--;
		if ( ProcessorBlock.threads_counter < 1 ){
			//#bugbug
			panic("kill_thread: threads_counter");
		}		
		
        threadList[tid] = (unsigned long) 0;
        Thread = NULL;		
	};
	
	// # reavaliar isso.
	// Se a thread fechada � a atual, 
	// necessitamos de algum escalonamento.	
    if ( tid == current_thread )
	    scheduler();
	
fail:	
	
//Done.
done:
    current_thread = idle;
	return;
};


/*
 **********************************************************
 * dead_thread_collector:
 *     Procura por uma thread no estado zombie mata ela.
 *
 * @todo
 *     Alerta o processo que a thread morreu.
 */
void dead_thread_collector (){
	
	int i;
    struct thread_d *Thread;   	  
    struct process_d *p;         
	
Scan:
	
	for ( i=0; i < THREAD_COUNT_MAX; i++ )
	{
	    Thread = (void *) threadList[i];
		
		if( (void *) Thread != NULL )
		{
		    if( Thread->state == ZOMBIE && 
			    Thread->used == 1 && 
				Thread->magic == 1234 )
			{
				
				if( Thread->tid == idle )
				{
					printf("dead_thread_collector: we can't close idle\n");
					die();
				}
				
				//kill_thread(Thread->tid);
				Thread->used = 0;
				Thread->magic = 0;
				Thread->state = DEAD; // Por enquanto apenas fecha.
				//...
			    
				// #importante:
				// Nessa hora precisamos notificar o 
				// a thread que estava esperando essa thread  
				// terminar.
				// Se essa for a thread prim�ria ent�o o processo 
				// ir� terminar tamb�m, ent�o o processo que esperava 
				// tamb�m dever� ser notificado.
				
                //Thread = NULL;
	            //threadList[i] = NULL;   //@todo: Liberar o espa�o na lista.
				
				ProcessorBlock.threads_counter--;
				if( ProcessorBlock.threads_counter < 1 )
				{
					//#bugbug
					panic("dead_thread_collector: threads_counter");
					//die();
				}
	
			};
			//Nothing.
		};
		//Nothing.
	};
	
	//@todo:
	// * MOVEMENT 10 (zombie --> Dead)
	// * MOVEMENT 11 (zombie --> Initialized) .. reinicializar.
	
done: 	
	return;
};


void kill_all_threads (){
	
	int i;
    
	for ( i=0; i < THREAD_COUNT_MAX; i++ ){
	    kill_thread (i);	
	}
};


//
// End.
//

