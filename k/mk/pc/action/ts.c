/*
 * File: microkernel\pc\ts.c
 *
 * Descri��o:
 *     Task Switching.
 *     Faz parte do Process Manager, parte fundamental do Kernel Base.
 *     Rotinas de Task Switch. Salva o contexto da tarefa, chama o scheduler 
 * pra trocar a tarefa e depois chama o dispatcher pra rodar a pr�xima.
 *     O Task Switch � feito de mais de uma maneira, de acordo com o modo 
 * selecionado. (rr, prioridade ...)
 *     The most common reasons for a context switch are:
 *     The time slice has elapsed.
 *     A thread with a higher priority has become ready to run.
 *     A running thread needs to wait.
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2016 - Revision.
 *     //...
 */


#include <kernel.h>
 
 
//
// Vari�veis internas.
//
  
int lock_taskswitch;  
//int __taskswitch_lock;
//...


  
//  
// Fun��es internas.  
//

void taskswitchRR();
//...



//
// ...
//

/*
void taskswitchFlushTLB();
void taskswitchFlushTLB(){
	//@todo: Implementar.
	return;
}
*/


/*
 *********************************************************
 * KiTaskSwitch:
 *     Interface para chamar a rotina de Task Switch.
 *     Essa rotina somente � chamada por hw.inc.
 *     KiTaskSwitch em ts.c gerencia a rotina de 
 * troca de thread, realizando opera��es de salvamento e 
 * restaura��o de contexto utilizado vari�veis globais e 
 * extrutura de dados, seleciona a pr�xima thread atrav�s 
 * do scheduler, despacha a thread selecionada atrav�s do 
 * dispatcher e retorna para a fun��o _irq0 em hw.inc, 
 * que configurar� os registradores e executar� a 
 * thread atrav�s do m�todo iret.
 *
 * #importante:
 * Na verdade, � uma interface pra uma rotina que 
 * faz tudo isso.
 * 
 */
void KiTaskSwitch()
{
	//
	// @todo: Fazer alguma rotina antes aqui ?!
    //
	
	//
	// Obs: A qui poderemos criar rotinas que n�o lidem com a troca de 
	// threads propriamente, mas com atualiza��es de vari�veis e gerenciamento 
	// de contagem.
	// >> Na entrada da rotina podemos atualizar a contagem da tarefa que acabou de rodar.
	// >> A rotina task_switch fica respons�vel apenas troca de contexto, n�o fazendo 
	// atualiza��o de vari�veis de contagem.
	// >> ?? Na sa�da ??
	//
	
	// ?? quem atualizou as vari�veis de crit�rio de escolha ??? o dispacher ??
	
	
	//Limits.
	if( current_thread < 0 || 
	    current_thread >= THREAD_COUNT_MAX )
	{
	    printf("KiTaskSwitch error: current_thread TID={%d}", 
		    current_thread );										   
        die();
	};
	

	//Limits.
	if( current_process < 0 || 
	    current_process >= PROCESS_COUNT_MAX )
	{
	    printf("KiTaskSwitch error: current_thread TID={%d}",
		    current_process );										   
        die();
	};	
	
	
	//
	// ## Task switch ##
	//
	
TaskSwitch:	
	task_switch();
	

    //
    // obs: Nessa hora ja temos um thread atual e um processo atual selecionados.
    // podemos checar as vari�veis para conferir se n�o est� fora dos limites.
    // Se estiverem fora dos limites, podemos usar algum etodo para selecioanrmos 
    // outro processo ou outra thread com limites v�lidos.
    //	
	
	
done:	
	//Retornando para _irq0 em head\x86\hw.inc.
    return;
};


/*
 *******************************************************
 * task_switch:
 *
 *     Gerencia a rotina de troca de thread, realizando 
 * opera��es de salvamento e restaura��o de contexto, 
 * seleciona a pr�xima thread atrav�s do scheduler, 
 * despacha a thread selecionada atrav�s do dispatcher e 
 * retorna para a fun��o _irq0 em hw.inc, que configurar� 
 * os registradores e executar� a thread atrav�s do 
 * m�todo iret.
 *
 * Obs:
 *     Apenas a interface KiTaskSwitch chama essa rotina.
 */
void task_switch()
{
	int New;
	int Max;    
	
	//@todo: Melhorar o nome dessas vari�veis.
	struct process_d *P;         // Processo atual. 
    struct thread_d *Current;    // Thread atual. 	
	//...
	
	
	//
	// Obs: Devemos atualizar a vari�vel global que indica quel � a thread atual 
	//      e a vari�vel global que indica qual � o processo atual.
	//      >> Essas vari�veis sofre altera��es durante essa rotina. Pois 
	// quando essa rotina come�a, as veri�veis representam o contexto antigo, e 
	// quando essa rotina termina as vari�veis devem representar o pr�ximo contexto.
	//      >> Portanto a �nica atualiza��o que interessa � no termino dessa rotina.
	//         ?? Essa atualiza��o pode ser feita na interface KiTaskSwitch ??
	//
	//
	
	
	Max = DISPATCHER_PRIORITY_MAX;
	
	//
	// Thread atual e processo atual.
	//
	
	// Valida 'Struct' da thread atual.	
    Current = (void *) threadList[current_thread]; 
	if( (void*) Current == NULL )
	{
	    printf("task_switch error: Struct={%x}", (void*) Current );										   
        die();
	};

	// Processo atual.
	// No caso da estrutura do processo so qual o thread 
	// pertence ser inv�lida.
	// Obs: N�o queremos que a thread perten�a a um processo inv�lido.
    P = (void*) Current->process;
	if( (void*) P == NULL )
	{
	    printf("task_switch error: P Struct={%x}", (void*) P );										   
        die();		
	};
	
	//Ok o processo ao qual o thread pertence � um processo v�lido.
	if( (void*) P != NULL )
	{
		/* #testar #bugbug ... antes de liberarmos esse filtro precisamos 
		                       atualizar corretamente o current process
							   no fim da rotina de task switch. Talvez at� mesmo em spawn 
							   e init idle thread.
                               							   
		//Conferindo se o valor salvo na estrutura do processo � o mesmo 
		//que foi salvo na vari�vel global que indica qual � o processo atual.
		if( P->pid != current_process ){
	        printf("task_switch error: P->pid != current_process ");										   
            die();					
		}
		*/
		
		
		if(P->used == 1 && P->magic == 1234)
		{
			//Obs: Se o if mais acima for verdadeiro essa 
			//atualiza��o n�o � nessess�ria. 
		    current_process = (int) P->pid;
		};
		
		//Obs: Podemos fazer outros testes referentes ao 
		//processo que o thread pertence.
		//...
		
	};
	//...	
	
	//
	// Contagem.
	//            step: Quantas vezes ela j� rodou no total.
	//    runningCount: Quanto tempo ela est� rodando antes de parar.
	//
	Current->step++;          
	Current->runningCount++;
	
	//
	// @todo: Agora a estrutura de processo tem uma 
	// vari�vel double Cycles.
	// A contagem de ticks ser� registrada na estrutura 
	// do processo tamb�m.      
	//

    //Outras configura��es iniciais.
	
	
	
	/*
	 * LOCKED:
	 * O mecanismo de task switch est� desligado, mas a tarefa � v�lida para 
	 * user mode. 
	 * Retorna sem salvar o contexto.
	 *
	 * @todo: 
	 *     Checar a tarefa atual. Seu iopl. (Deve ser ring0).
	 *     *importante: N�o colocamos isso na interface KiTaskSwitch porque 
	 *                  � bom fazermos todas as checagens antes de acionarmos 
	 *                  essa condicional.
	 */	 
	if(task_switch_status == LOCKED)
	{    		
	    //
	    // @todo: 
	    // Incrementar o tempo que todas as outras est�o sem rodar.
	    // Usando a thread list.
	    // @todo: Checar contexto. iopl. stacks.    
	    //
		
		// Atualiza a contagem de tipo de sele��o.
		IncrementDispatcherCount(SELECT_CURRENT_COUNT);
		
		// Obs: taskswitch locked, Retorna sem salvar.
		return;  
	};
	
	
	/* 
	 * UNLOCKED:
	 * O mecanismo de taskswitch esta destravado, podemos usa-lo.
	 *
	 * > Salvamos o contexto.
	 * > Retornameo se n�o venceu a cota.
	 * > Se venceu a cota � preemp��o por vencimento de cota.
	 * > Checamos se existe uma thread no estado standby, querendo rodar pela primeira vez.
	 *   Se ouver essa rotina � desviada para um spawn e n�o prossegue.
	 * > Checamos se existem 'requests': Pedidos pendentes.
	 *
	 */
	if( task_switch_status == UNLOCKED )
	{   
		//
	    // ## SAVE CONTEXT ##
	    //
		
	    save_current_context();
	    Current->saved = 1;	
				
        // Checa o 'quantum'. 
		//(Limite que uma thread pode usar o processador).
        // Continua rodando at� vencer a cota.		
		// @todo: Talvez possa restaurar antes de retornar.
		// Retorna sem restaurar, se ainda n�o venceu a cota.
		if( Current->runningCount < Current->quantum )
		{
			
			// Atualiza a contagem de tipo de sele��o.
			IncrementDispatcherCount(SELECT_CURRENT_COUNT);
			return; 
		
		}else{
			
		    //
			// ## PREEMPT ##
			//
		    // Aqui a thread atual pode estar no estado running ou initialized.
		    // +Se estiver running, torna ready. (preempt).
		    // +Se estiver initialized, vai checar e executar mais a frente.
		    // #bugbug ?? E se tiver em outro estado ?? O que fazemos ??
			//
			//
		
		    // * MOVEMENT 3 (Running --> Ready).
		    if( Current->state == RUNNING )
		    {
                // MOVEMENT 3 (running >> ready)  				
			    Current->state = READY;    //Preempt.
		    
			    // In�cio da fila onde a prioridade � menor.
			    if( Current->preempted = PREEMPTABLE )
				{
					// @todo: Nesse momento prioridade pode ser baixada para 
					// o n�vel de equil�brio.
			        queue_insert_head( queue, 
					                   (unsigned long) Current, 
									   QUEUE_READY );	
			    };
			
			    // Fim da fila onde a prioridade � maior.
			    if(Current->preempted = UNPREEMPTABLE)
				{
					//Obs: N�o abaixa a prioridade.
			        queue_insert_data( queue, 
					                   (unsigned long) Current, 
									   QUEUE_READY );	
			    };
			    //Nothing.
		    };
		    
			//
			// @todo:
			// A tarefa mais pr�xima de sua Dead Line � a tarefa de maior 
			// prioridade.
			// check_for_ ... (mais pr�xima da deadline)
			//
			
			//
			// *IMPORTANTE:
			// Checando se tem threads em standby que 
			// est�o esperando pra rodar pela primeira vez.
		    // Obs: Essa chamada n�o retorna se encontrar 
			// uma thread assim. 
			//
			
			// schedi.c
			check_for_standby(); 
            
            //
            // Request:
            // Esse � o momento ideal para atender os requests. 
			// Pois a thread que estava utilizando o processador, 
			// utilizou toda a sua cota. 			 
			// Ent�o agora atendemos o request, (Internal Signal), 
			// somente depois de a thread ter utilizado o processador 
			// durante todo o tempo que lhe foi concedido. :)
            //			
			
			KiRequest();
			
            //
            // N�o havendo thread para ser inicializada, nem request, 
			// roda uma thread da fila do dispatcher.
            //
			
		    goto try_next;
		};
	    //Nothing.
	};
	
    //
	// * Se estamos aqui � porque houve uma falha anormal. Ent�o tentaremos 
	//   selecionar a pr�xima thread. 
	//
	
//crazyFail:	
	goto dispatch_current;      	
	
	//
	// Seleciona a Current, usando a fila do dispacher.
	//
	
try_next: 

	//
	// Obs:
	// No pior dos casos o condutor ir� parar no fim da lista.
	//
	
	// Checa se � o �ltimo da lista encadeada.
	if( (void*) Conductor->Next == NULL )
	{
		// #Agendar.
		// Fim da lista.  
		// Recriar a lista. 
		
		KiScheduler();     
		
		// Com a lista atualizada, v� em frente.
		goto go_ahead;  
	};
	
	
	// Se ainda n�o for o �ltimo da lista encadeada.
	if( (void*) Conductor->Next != NULL )
	{
	    // O condutor avan�a e pega o pr�ximo da lista.
		Conductor = (void*) Conductor->Next;
		goto go_ahead;
	}else{ 
	    //@todo: Se o pr�ximo for NULL.
		//goto go_ahead;
	};
	
	//
	// *Importante:
	// E se n�o for o �ltimo da lista e pr�ximo for NULL?
	// � o que trataremos logo abaixo.
	//
	
	//Nothing.
	
go_ahead:
    
	//
	// Pronto, temos uma nova thread atual.	
	// Ela � representada pelo Conductor.
    //
	
	Current = (void *) Conductor;
	
    // Checa a validade da estrutura.	
    // Se for inv�lida, tentamos a pr�xima novamente.
	// @todo: #bugbug Isso pode gerar um loop infinito.
	
	if( (void *) Current == NULL )
	{ 
	    //#bugbug Isso pode gerar um loop infinito.
		goto try_next;  
	}else{
		
		//
		// Obs: O 'else' � para o caso da estrutura 
		// ter um ponteiro v�lido.
		// Agora checamos os outros par�metros da estrutura.
		//
		
		// Estrutura corrompida.
		// Se for inv�lido, tentamos novamente.
		// @todo: #bugbug Isso pode gerar um loop infinito.
		
		if( Current->used != 1 || 
		    Current->magic != 1234 )
		{
			//#bugbug Isso pode gerar um loop infinito.
			goto try_next;	
		};		
		
		//Se n�o est� pronta a thread.
		//Tentamos novamente.
		//@todo: #bugbug Isso pode gerar um loop infinito.
	    if( Current->state != READY )
		{
			//#bugbug Isso pode gerar um loop infinito.
	        goto try_next; 
		};
		
		// Selecionamos uma thread atrav�z do dispatcher. 
		// Incrementando a contagem do tipo de sele��o.
		IncrementDispatcherCount(SELECT_DISPATCHER_COUNT);
		
		//
		// ## Finalmente ##
		//
		
		// Obs: Current � a thread selecionada.
		// Salvamos o TID da thread selecionada.
		current_thread = (int) Current->tid;
		
		// Despacha a thread selecionada.
		goto dispatch_current;    			
	};	
	
	//
	// Nothing.
	// @todo: #bugbug. Se estamos aqui � porque houve 
	// uma falha anormal. Vamos optar pela atual e n�o 
	// tentaremos mais nada.
    //
 	
//superCrazyFail:
	goto dispatch_current; 
	//#Obs: N�o tentaremos novamente.
	//goto try_next; 
	
	//Nothing.
	
//	
// Done.
// Nesse momento a current_thread foi selecionada e 
// n�o tem volta.
//
dispatch_current:
    
	// �ltima filtragem, s� por paran�ia. 
	// if(current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
	//     goto try_next;
	// };
	
	// Checa a validade da thread selecionada.
	// @todo: Resili�ncia. Tomar uma decis�o e n�o desistir.
	// A idle � sempre uma �ltima op��o.
    Current = (void *) threadList[current_thread];
	if( (void *) Current == NULL )
	{
		panic("task_switch.dispatch_current error: Struct");
	    die();
	}else{
		
		// @todo: Resili�ncia. Tomar uma decis�o e n�o desistir.
		// A idle � sempre uma �ltima op��o.		
	    if( Current->used != 1 || 
		    Current->magic != 1234 || 
			Current->state != READY )
	    {
	        panic("task_switch.dispatch_current error: Param.");
	        die();
	    };
        //Obs: Podemos filtrar outros par�metros sistemicamente importante.		
		//Nothing.
	};
	
	// Reinicia a contagem de tempo rodando. Isso �, 
	// quantas vezes a thread usou a CPU at� esgotar o quantum, sua cota.
	Current->runningCount = 0;
	
	
	//
	// #importante:
	// Chama o dispatcher. 
	// Isso coloca a thread selecionada no estado RUNNING 
	// e restaura o contexto.
	//
	
	//
	// * MOVEMENT 4 (Ready --> Running).
	//
	
	dispatcher(DISPATCHER_CURRENT);    	
	
	//
	// #importante: 
	// Retornamos do dispatcher.
	// O dispatcher restaurou o contexto para a pr�xima thread, 
	// passando os valores da estrutura para vari�veis que ser�o 
	// usadas para carregar os registradores na hora do iretd.
	// Agora vamos retornar para a rotina da _irq0 feita em assembly, 
	// que est� em head\x86\hw.inc, mas antes, voltamos para a 
	// interface KiTaskSwitch() que chamou essa rotina.
	//
	
	//Nothing.
	
//Done.
done:
    
	//
	// @todo: 
	// Salvar em uma vari�vel global o cr3 do processo 
	// da thread selecionada para que a rotina em assembly, 
	// (head\x86\hw.s), configure o cr3.	
	// Para isso precisamos descobrir qual � o processo atual.
	// Current � o pr�ximo thread a rodar.
	//
	// Current->ppid � o ID do processo ao qual o thread pertence ???
	// #bugbug: @todo: O que precisamos � nos certificar que esse 
	// ID � v�lido.
	//
	// @todo: Criar na estrutura de thread a vari�vel 
	// 'int owner_process_id;'
	//        Isso resolve o problema.
	//
	// #bugbug @todo: filtrar se o id � v�lido.
	//
	
	// #Overflow. 
	// Checar se n�o � maior que o n�mero m�ximo de �ndices 
	// que a lista suporta.
	
	if( Current->ownerPID < 0 || 
	    Current->ownerPID >= PROCESS_COUNT_MAX )
	{
		//fail;
		printf("taskswitch: ownerPID fail \n", 
		    Current->ownerPID );
		die();
	};
	
	// Checando processo.
	
    P = (void*) processList[Current->ownerPID];
	
	// Estrutura inv�lida
	if( (void*) P == NULL )
	{
		//fail;
		printf("taskswitch: Process %s struct fail \n", 
		    P->name_address );
		die();
	};
	
	// Estrutura v�lida.
	if( (void*) P != NULL )
	{
		// Corrompida.
		if( P->used != 1 || 
		    P->magic != 1234 )
		{
			//fail
		    printf("taskswitch: Process %s corrompido \n", 
			    P->name_address );
		    die();			
		};
			
		// N�o corrompida.	
		if( P->used == 1 && 
		    P->magic == 1234 )
		{
		    current_process = (int) P->pid;
			
			//Debug:
			//@todo: Isso � provis�rio.
			//So deixaremos passar o diret�rio do processo kernel.
			//if( (unsigned long) P->Directory != KERNEL_PAGEDIRECTORY){
			
			// Obs: #bugbug: Aviso.
			// Permite v�rios endere�os, menos '0'.
			// Pois '0' pode indicar que a vari�vel n�o foi 
			// inicializada corretamente.
			// Um endere�o inv�lido pode ser colocado em cr3, 
			// travando o sistema.
			
			if( (unsigned long) P->Directory == 0 )
			{	
			    //fail
		        printf("taskswitch: Process %s directory fail\n", 
				    P->name_address );
		        die();
			};
			
			//
			// ## Salvando  ##
			//
			
			// Bom, aqui essas duas op��es de vari�veis 
			// existem na estrutura, estou trabalhando nisso.
			// Importante � que salvamos em uma vari�vel global, 
			// agora acess�vel pela rotina em assembly _irq0.
			
			//Obs: Essa � a op��o usada por enquanto.
			current_process_pagedirectory_address = (unsigned long) P->Directory;
			
			//current_process_pagedirectory_address = (unsigned long) P->page_directory->Address;
		    
			goto doneRET;
		};
		
		
		//
		//  ## Fail  ##
		//
		
		printf("task_switch: Struct \n");
		die();
	};
	
		//
		//  ## Fail  ##
		//
	
//test:
	//#debug
	//#breakpoint
	printf("task_switch: debug breakpoint\n");
	refresh_screen();
	while(1){
		asm("hlt");
	}
		
doneRET:
	
	
	//
	//  ## Finalmente  ##
	//
	
	//Debug:
	//Provis�rio
	//current_process_pagedirectory_address = (unsigned long) KernelProcess->Directory;
	//current_process_pagedirectory_address = (unsigned long) KERNEL_PAGEDIRECTORY;
	
    // 
	// Done.
	// Ao retornar, a rotina _irq0 em assembly atualiza o 
	// registrador CR3 com o ponteiro para o diret�rio de p�ginas 
	// do processo ao qual a thread pertence, alimenta os 
	// registradores e efetua iretd. #fim :)
	//
	
	//
	// Nesse momento retornaremos para a interface que 
	// chamou essa fun��o.
	// Depois a interface retorna para o driver de timer, 
	// em _irq0.
	//
	
	return; 		
};


/*
 * taskswitchRR:
 *     Task switch usando Round Robin.
 *     Obs: Esse m�todo ainda n�o foi habilitado.
 */
void taskswitchRR()
{
	int i;
	int Max = (int) ProcessorBlock.running_threads;
	struct thread_d *Current; //Thread atual.
	
	//Filtro.
	//if(current_thread ...){}
	
	Current = (void *) threadList[current_thread]; 
	if((void *) Current == NULL)
	{
	    panic("taskswitchRR error: Struct");
		die();
	};
	
	//
	//todo: Checar se a estrutura � v�lida. 
	//
	
	/*
	 *    Incrementa.
	 *     Continua na mesma tarefa enquanto a contagem de quantum for
	 *     menor que o quantum limite da tarefa atual.
	 *     Assim temos uma contagem de vezes que uma tarefa deve ficar
     *     no processador sem sair.	 
	 */	


	//se o runningCount ainda � menor que a cota, dispacha.
	//Obs: Cuidado !! A m�dia � (PRIORITY_NORMAL*2)
	if(Current->runningCount < Current->quantum){ 
		goto dispatch_current;
	}else{
		
        //?? @todo: Talvez aqui seja o movimento 3, running >> ready. Conferir.  		
		Current->state = READY;
		
		//
		// @todo: Nesse momento, colocar no fim da fila ready. tail
		//
		//queue_insert_data( queue, (unsigned long) Current, QUEUE_READY);
		
		//se ouver outra v�lida nos slots despacha.
		i = (int) current_thread;
				
		do
		{
		    i++;
			
			//Contando++
			if(i < Max)
			{
		        Current = (void *) threadList[i];
		        if( (void *) Current != NULL )
			    {
			        if( Current->used == 1 && 
					    Current->magic == 1234 && 
						Current->state == READY)
				    {
				        Current->runningCount = 0;
				        current_thread = (int) i;
				        goto dispatch_current;
				    };
			    };
			};
			
			// Overflow
			if(i >= Max)
			{ 
			    i = 0; 
				Current = (void *) threadList[i];
			    if( (void *) Current != NULL )
				{
				    if( Current->used == 1 && 
					    Current->magic == 1234 && 
						Current->state == READY )
					{
					    Current->runningCount = 0;
					    current_thread = (int) i;
						goto dispatch_current;
					};
				};
			};

		}while(i < Max);
			
        panic("taskswitchRR error: *HANG");
        die();   			
		//Use idle. N�o havendo outra alem da idle usa ela.
		//current_idle_thread.
		//current_thread = (int) 0;
		//goto dispatch_current;
	}; 
	
//	
// Done.
//
dispatch_current:	
	dispatcher(DISPATCHER_CURRENT);	
	return;	
};




/*
 ***************************************************
 * set_task_status:
 *    Configura o status do mecanismo de task switch.
 *    Se o mecanismo de taskswitch estiver desligado 
 * n�o ocorrer� a mudan�a.
 *
 * @todo: Mudar o nome dessa fun��o para taskswitchSetStatus(.);
 *
 */ 
void 
set_task_status( unsigned long status )
{
    //#bugbug: Mudar para int.	
	task_switch_status = (unsigned long) status;
};


/*
 **************************************************
 * get_task_status:
 *     Obtem o status do mecanismo de taskswitch.
 *
 * @todo: Mudar o nome dessa fun��o para taskswitchGetStatus();.
 */
unsigned long 
get_task_status()
{
    //#bugbug: Mudar para int.		
    return (unsigned long) task_switch_status;
};


/*
 **********************************************
 * taskswitch_lock:
 *     Trava o mecanismo de taskswitch.
 *     @todo: Mudar para taskswitchLock().
 */ 
void 
taskswitch_lock()
{
    task_switch_status = (unsigned long) LOCKED;
};


/*
 *********************************************
 * taskswitch_unlock:
 *     Destrava o mecanismo de taskswitch.
 *     @todo: Mudar para taskswitchUnlock().
 */ 
void 
taskswitch_unlock()
{
    task_switch_status = (unsigned long) UNLOCKED;
};


//
// End.
//
