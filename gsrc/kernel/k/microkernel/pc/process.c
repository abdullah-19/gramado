/*
 * File: pc\process.c 
 *
 * Descri��o:
 *     Gerenciamento de processos.
 *     PM - Process Manager (Parte fundamental do Kernel Base).
 *     Interfaces para o kernel chamar as rotinas de gerenciamento de
 * processos. 
 *     As rotinas aqui s�o independentes da arquitetura, quem se preocupa
 * com a arquitetura s�o as rotinas de inicializa��o comecadas por Ki.
 *     'Ki' para m�dulos internos do kernel.
 *
 * Atribui��es:
 *     Rotinas de gerenciamento das processos, cria��o, inicializa��o,
 * finaliza��o, task switch e outros.
 *
 *    Tipos de Processos:
 *
 *    PB - Processos inclu�dos no Kernel Base.
 *    P0 - Processos em ring0.
 *    P1 - Processos em ring1.
 *    P2 - Processos em ring2.
 *    P3 - Processos em ring3, User Mode.	
 *
 * @todo: 
 * Essas interfaces devem ser padronizadas, obedecendo roteiros de abertura, 
 * fechamento, salvamento e restaura��o de pilha ou argumento.
 * +Criar a fun��o get_current_pid.
 *
 * In this file:
 * ============
 *     +processSendSignal
 *     +create_process 
 *     +CloseAllProcesses
 *     +show_process_information
 *     +muitas outras rotinas...
 *
 * Vers�o 1.0, 2015.
 */
 
 
/*
    Fluxo padr�o:
	
	The Standard Streams.
	
	Every process is initialized with three open file descriptors, 
	stdin, stdout, and stderr. 
	stdin is an abstraction for accepting input (from the keyboard or from pipes) and 
	stdout is an abstraction for giving output (to a file, to a pipe, to a console).

    That's a very simplified explanation but true nonetheless. 
	Those three file descriptors are collectively called 'The Standard Streams'.	
    
	Standard input - this is the file handle that your process reads to 
	get information from you.
    Standard output - your process writes normal information to this file handle.
    Standard error - your process writes error information here.
	
    Quando um programa inicia, 
	o sistema operacional automaticamente define quem � 
	a entrada padr�o (geralmente o teclado) e quem � 
	a sa�da padr�o (geralmente a tela).
*/ 
 
 
#include <kernel.h>



//
// Fun��es importadas.
//

extern unsigned long get_page_dir();


//
// Vari�veis internas.
//

//int processmanagerStatus;
//int processmanagerError;
//...

int caller_process_id;


int processNewPID;   //??


//
//...
//


/*
struct process_d *processNew();
struct process_d *processNew()
{
    struct process_d *New;	
	
	//@todo: Implementar.
	//Cria uma processo gen�rica e retorna o ponteiro.
	
	//
done:	
	return (struct process_d *) New;
};
*/



/*
 * processTesting:
 *     Testando se o processo � v�lido. Se for v�lido retorna 1234.
 *     @todo: repensar os valores de retorno. 
 * system call (servi�o 88.)
 * 
 */
int processTesting(int pid)
{
	struct process_d *P;
	
	P = (void*) processList[pid];
	
	if( (void*) P == NULL )
	{
		return (int) 0;
	}
	else
	{		
		
		if( P->used == 1 && P->magic == 1234){
	        return (int) 1234;			
		}
	};
	
fail:
	return (int) 0;
}



/*
 * processSendSignal:
 *     Envia um sinal para um processo.
 *     Se o sinal e o processo forem v�lidos, um sinal � colocado
 * no PCB do processo.
 *     @todo: Rotinas envolvendo sinais devem ir para outro arquivo.
 */
int processSendSignal(struct process_d *p, unsigned long signal)
{
	//SIGNAL_COUNT_MAX
	
	//Limit
    //if(signal >= 32){
	//	return 1;
	//}
	
	if(signal == 0){
		return 1;
	};
	
	//struct fail
	//if( (void*) p == NULL ){
	//	return 1;
	//}		
	
ok:	
	//Ok
	if( (void*) p != NULL ){	
		p->signal = (unsigned long) signal;
		return 0; //(int) signalSend(p,signal);
	}
	
	//...
	
fail:
	return 1;
};


/*
 * create_process:
 *     Cria process para ring 3.
 *     Preenche o PCB (Process Control Block).
 *
 * @todo: 
 *     Rotina ainda n�o implementada.
 *     Um processo na verdade inclui tudo sobre a tarefa,
 *     Desde o nome do arquivo at� o carregamento, registro e execu��o.
 *
 *     Um job � um conjunto de processos.
 *     Tudo depende do tipo (funcionalidade): 
 *     SYSTEM, PERIODIC, ROUND_ROBIN, IDLE.
 *
 *    Classes de processos (tipo de n�vel):
 *
 *    PB - Processos inclu�dos no Kernel Base.
 *    P0 - Processos em ring0.
 *    P1 - Processos em ring1.
 *    P2 - Processos em ring2.
 *    P3 - Processos em ring3, User Mode.
 *
 *
 * @todo: Esse fun��o deve chamar uma rotina de crai��o de diret�rio. 
 *        Quando um processo � criado deve-se criar o seu diret�rio
 *        e criar as pagetables que o processo vai usar de acordo
 *        com o tamanho do processo. 
 * @todo: processCreateProcess(...)
 *
 * Aumetar o n�mero de argumentos para dar o suporte necess�rio para criar um processo 
 * do jeito que for necess�rio
 */
process_descriptor_t *create_process( struct wstation_d *window_station,
                                      struct desktop_d  *desktop,
                                      struct window_d *window,
                                      unsigned long init_eip, 
                                      unsigned long priority, 
									  int ppid, 
									  char *name, 
									  unsigned long iopl,
									  unsigned long directory_address )
{
	int i;
	struct process_d *Process;
    struct process_d *Empty;    //Para a entrada vazia no array de processos.  	

    //@todo:
    //Melhorar esse esquema de numera��o e contagem de processos criados.
	
    if( processNewPID < USER_BASE_PID || processNewPID >= PROCESS_COUNT_MAX ){
		processNewPID = (int) USER_BASE_PID;	
	};
	
	i = (int) processNewPID;
	
	Process = (void *) malloc(sizeof(struct process_d));
	if( (void*) Process == NULL ){
	    printf("pc-process-create_process: Process");
		die();
		//@todo: aqui retorna NULL.
	};


//Loop.	
get_next:	
	
	//BugBug: Isso pode virar um loop infinito.
	
	i++;
	
	if(i >= PROCESS_COUNT_MAX)
	{
		printf("pc-process-create_process: End of list");
        refresh_screen();
		return NULL;
		//while(1){}
		i = USER_BASE_PID;
	};
	
	//Get empty.
	Empty = (void*) processList[i];
	
	//Se o slot estiver ocupado.
    if( (void*) Empty != NULL ){
		goto get_next;
	}else{
		
		//Object.
		Process->objectType = ObjectTypeProcess;
		Process->objectClass = ObjectClassKernelObjects;
		
		//c
		processNewPID = (int) i;
		//processNewPID++;
		
		//b,a,g, Identificadores.
		Process->pid = (int) i;                    //PID.
        Process->ppid = (int) ppid;                //PPID.    @todo:
		Process->uid = (int) GetCurrentUserId();   //UID. 
        Process->gid = (int) GetCurrentGroupId();  //GID. 

		//f,flag, State of process
		Process->state = INITIALIZED;    	
		
		//e - Error.
		//Process->error = 0;
		
		Process->used = 1;
		Process->magic = 1234;	
		
		//Name.
		//Process->name = name; //@todo: usar esse.
		//Process->cmd = NULL;  //nome curto que serve de comando.
		Process->name_address = (unsigned long) name;
		
		
		//Process->terminal =
		
		//
	    //    ****  Banco de dados ****
		//
		
		//bancos de dados e contas do processo.
		//Process->kdb =
		//Process->gdbListHead =
		//Process->ldbListHead =
		//Process->aspaceSharedListHead =
		//Process->aspacePersonalListHead =
		//Process->dspaceSharedListHead =
		//Process->dspacePersonalListHead =
		
		//Inicializando a lista de framepools do processo.
		//@todo: Todo processo deve sr criado com pelo menos um 
		//frame pool, o que � equivalente a 4MB.(uma parti��o)
		//Obs: Um framepool indica onde � a �rea de mem�ria fisica
		//que ser� usada para mapeamento das p�ginas usadas pelo processo.
		Process->framepoolListHead = NULL;
		
		//Lista de arquivos.
		//Process->Files =
		
		//Thread inicial.
		//Process->thread =
		
		//Process->processImageMemory =
		//Process->processHeapMemory =
		//Process->processStackMemory =
		
	    //
	    // ORDEM: O que segue � referenciado durante o processo de task switch.
	    //

		//
		// Page Directory: 
		//     Alocar um endere�o f�sico para o diret�rio de p�ginas do processo a ser
		// criado, depois chamar a fun��o que cria o diret�rio.
		//
		// @todo:
		// *IMPORTANTE: Por enquanto os processos s�o criadas usando o diret�rio de p�ginas 
		// do processo Kernel. Mas temos que criar um diret�rio novo pra cada processo criado.
		// O diret�rio de todos os processos de usu�rio ser�o iguais. Ter�o uma �rea de us�rio
		// particular e uma �rea compartilhada em kernel mode.
		//

		//@todo: 		
		Process->Directory = (unsigned long ) directory_address; //KERNEL_PAGEDIRECTORY; //@todo: Usar um pra cada processo.
        //@todo: Alocar um endere�o f�sico antes, depois chamar a fun��o que cria o pagedirectory.
		//Process->Directory = (unsigned long ) CreatePageDirectory(unsigned long directory_address)
		//Process->page_directory->Address = (unsigned long) KERNEL_PAGEDIRECTORY;
		
        //cancelados. 
		//Process->mmBlocks[32]
		//Process->mmblockList[32]
		
		
		//Process->processMemoryInfo
		
		
		//
		// @todo: Precisa alocar espa�o na mem�ria f�sica.
        //        Precisa criar page tables para essas areas de cada processo.		
		//        Os endere�os virtuais dessas areas dos processos s�o sempre os mesmos.
		//        mas os endere�os f�sicos dessas areas variam de processo pra processo.
		//

		//Imagem do processo.
		Process->Image = UPROCESS_IMAGE_BASE;           //Base da imagem do processo.
		//Process->ImageSize = 0;                             //Tamanho da imagem do processo.	    
		
		
		//
		// Heap e Stack:
		//
		// @todo: #BugBug 
		// O Heap e o Stack devem estar dentro da �rea de mem�ria do processo.
		// Uma pagetable do diret�rio � para o heap e outra para a stack.
        // Cada pagetable no diret�rio do processo � pra uma coisa.		
		// # O erro aqui � que todo processo criado com essa rotina tem  
		// o mesmo endere�o de heap e stack.
		//
		
		//Heap and Stack. 
		// ** (endere�os virtuais). **
		
	    Process->Heap        = UPROCESS_DEFAULT_HEAP_BASE;    //Endere�o do in�cio do Heap do processo.
	    Process->HeapEnd = 0;
		Process->HeapSize    = (UPROCESS_DEFAULT_HEAP_SIZE/1024);    //Tamanho do heap, dado em KB.

		//Process->HeapPointer
		//Process->HeapLastValid
		//Process->HeapLastSize
	    
		Process->Stack       = UPROCESS_DEFAULT_STACK_BASE;   //Endere�o do in�cio da Stack do processo.
	    Process->StackEnd = 0;
		Process->StackSize   = (UPROCESS_DEFAULT_STACK_SIZE/1024);   //Tamanho da pilha,dada em KB.	
	    Process->StackOffset = UPROCESS_DEFAULT_STACK_OFFSET; //Deslocamento da pilha em rela��o ao in�cio do processo. ??
	    

		//ring.
		Process->iopl = iopl; 
        
		
	    //PPL - (Process Permition Level).(gdef.h)
        //Determina as camadas de software que um processo ter� acesso irrestrito.
	    //Process->ppl = pplK0;
		
        //
        // Procedimento de janela para o processo criado.
		// Quando criamos um processo, atribuimos � ele o procedimento de 
		// janela padr�o do sistema at� que o processo pe�a para troc�-lo.
		// Poderia ser 0, mas pode dar page fault.
	    //
		
		Process->procedure = (unsigned long) &system_procedure;
	    
		//Msg support.
		//Argumentos do procedimento de janela.
		//@todo: Isso pode ser um ponteiro de estrutura,
		//a fila de mensgens pode ser uma fila de ponteiros.
		Process->window = NULL;    //arg1. 
	    Process->msg = 0;          //arg2.
	    Process->long1 = 0;        //arg3.
	    Process->long2 = 0;        //arg4.			
		
	    //Process->callerq	   //head of list of procs wishing to send.
        //Process->sendlink;    //link to next proc wishing to send.
        //Process->message_bufffer		   //pointer to message buffer.
        //Process->getfrom_pid		       //from whom does process want to receive.
        //Process->sendto_pid	               //pra quem.

	    //Signal
	    //Process->signal = 0;
	    //Process->signalMask = 0;
		
		//cancelada.
		//Process->process_message_queue[8]
		
		//Outras coisas.
		
		//Prioridade.
		Process->base_priority = (unsigned long) priority;
		Process->priority = (unsigned long)  Process->base_priority;
		
	    //Que tipo de scheduler o processo utiliza. (rr, realtime ...).
	    //Process->scheduler_type = ;   		
		
		
		//Process->step
		//Process->quantum
		//Process->timeout
		//Process->ticks_remaining
		
		//Process->ThreadQuantum   //As threads do processo iniciam com esse quantum.
		
		
		//Process->threadCount = 0;    //N�mero de threads do processo.
		
		//Process->tList[32] 
		
		//Lista de threads.
		Process->threadListHead = NULL;

        //Process->window_id 		
		
		//Process->preempted
		
		
		//Process->saved
		//Process->PreviousMode
		
		//context
	    //Process->ss
        //Process->esp
        //Process->eflags
        //Process->cs
        //Process->eip = init_eip;  //isso deve ser o entry point da thread principal.	
        //Process->ds
        //Process->es
        //Process->fs
        //Process->gs
        //Process->eax	
        //Process->ebx
        //Process->ecx
        //Process->edx
        //Process->esi
        //Process->edi
        //Process->ebp
	    //Continua o contexto ...
		
		//Process->event
		
	
        //Continua ...
		
        //Process->processDesktop
		//Process->processUserSession
		//Process->window_station
		//Process->desktop
		
		//Process->base_priority
		
		
		//Process->iob[3]
		
		//Process->wait4pid
		
		
		Process->zombieChildListHead = NULL;
		
		Process->exit_code = 0;
		
		//More?!
		//Obs: � bom lembrar que outros elementos podem ser 
		//configurados posteriormente.
		
		//Next.
		//Process->next_task;  //@todo deletar isso.
		//Process->next_process
		//Process->NextInitialized
		//Process->Parent
		//Process->Prev
		Process->Next = NULL; 

		//Coloca o processo criado na lista de processos.
		processList[i] = (unsigned long) Process;		
		//Nothing.
	};	

    //...	
	
//Done.
done:
    return (void*) Process;
};

 
/*
 * CloseAllProcesses:
 *     Bloqueia todos os processos da lista de processos.
 *     Menos o processo '0'.
 *     processCloseAllProcesses();    
 */
void CloseAllProcesses()
{
	int Index;
    struct process_d *P;

	//Menos o 0. 
    for(Index = 1; Index <= PROCESS_COUNT_MAX; Index++)
	{
		//Pega, bloqueia e tira da lista.
		P = (void*) processList[Index];
		P->state = PROCESS_BLOCKED;
        processList[Index] = (unsigned long) 0;		
	};		
	
	//Check process 0.
	P = (void*) processList[0];
	if( (void*) P == NULL ){
		panic("CloseAllProcesses error: Process 0 Struct.\n");
	};
	
	
	
	//@todo: checar estado do processo 0.
	
	
done:
    //CurrentProcess = (void*) P;
    //current_process = 0;	
    return;
};


/*
 * KeSaveCurrentContext:
 *     Interface para chamar rotina de salvamento de contexto de registradores.
 */
void KeSaveCurrentContext()
{
   //sendo uma interface pode chamar a rotina de acordo com a arquitetura presente.
   //para isso pode-se usar hal.
    return;
};

/*
 * KeRestoreCurrentContext:
 *     Interface para chamar rotina de restaura��o de contexto de registradores.
 */
void KeRestoreCurrentContext()
{
   //sendo uma interface pode chamar a rotina de acordo com a arquitetura presente.
   //para isso pode-se usar hal.
    return;
};
 
 
/*
 * KeCpuFaults:
 *     Interface para chamar a rotina de tratamento das fault de CPU.
 *     @todo: Talvez essa rotina possa ir para cpu.c
 *
 */ 
void KeCpuFaults(unsigned long fault_number)
{ 
	
	//
	// @todo: Limits ???
	//
	
	KiCpuFaults(fault_number);
	return;
};




/*
 * KeSpawnTask:
 *     Interface para rotina que executa pela primeira vez
 * uma tarefa em user mode.
 * @todo: deletar.
 */
void KeSpawnTask(int id)
{
    KiSpawnTask(id);
	//no return.
};




/*
 * KeSelectNextThread:
 *     Interface para chamar a rotina que seleciona a proxima tarefa para
 * rodar, baseado em criterios.
 *
 */
int KeSelectNextThread(int current)
{
    
	//@todo: analizar o retorno.
	return (int) KiSelectNextThread(current);
}


/*
 * KeCheckTaskContext:
 *     Interface para chamar uma rotina de an�lise de contexto.
 */
int KeCheckTaskContext( int task_id)
{
    //@todo: prepara��o antes de chamar.filtros.
	return (int) KiCheckTaskContext(task_id);
}



/*
 * KeInitTasks:
 *     Interface para chamar uma rotina de inicializa��o de variaveis
 * do mecanismo de gerenciamento de tarefas.
 *
 */
void KeInitTasks()
{
    KiInitTasks();
	
	//@todo: esse retorno poderia ser int.
	return;
};




/*
 * KeCreateTask:
 *     Interface para chamar uma rotina de cria��o de um processo.
 * nao uma thread.
 *
 */
int KeCreateTask( int task_id, 
                 unsigned long *task_eip, 
				 unsigned long *task_stack, 
				 unsigned long prior )
{
    return 0;
};




/*
 * KeInitTask:
 *     Interface para chamar uma rotina de inicializa��o de uma tarefa.
 */
int KeInitTask(int id)
{
    return 0;
};



/*
 * KeFork:
 *     Interface para chamar a rotina de cria��o de uma thread atrav�s
 * de duplica��o ou clonagem.
 *     @todo: Ke chama rotina de modulo externo.
 *            n�o � necessario.
 *
 */
int KeFork()
{
    //essa � uma interface... vai chamar rotina de inicializa��o que chamara rotina fork.
    return 0; 
};


/*
 * KeExecProcess:
 *     Interface para chamar a rotina que executa uma tarefa, dado
 * nome e endere�o.
 *
 */
int KeExecProcess(int pid, unsigned long *process_address)
{
    return 0;
};



/*
 * KeDispatchProcess:
 *     Interface para chamr uma rotina para despachar uma thread.
 *     Server em user mode.
 */
void KeDispatchProcess(){
    return;
}

void KeDoProcessReady(int pid){
    return;
};

void KeDoProcessRunning(int pid){
    return;
};

void KeDoProcessSleeping(int pid){
    return;
};

void KeDoProcessZombie(int pid){
    return;
};

void KeDoProcessDead(int pid){
    return;
};


/*
 * KeStartTask:
 *     Interface para chamar uma rotina de execu��o de um processo,
 * dado seu id e seu endere�o.
 */
void KeStartProcess(unsigned long pid, unsigned long *task_address){
    return;
};

 
 
/*
 * KeAcordarTarefa:
 *     Interface para chamar uma rotina de acordar um processo..
 */ 
void KeWakeupProcess(int pid)
{
    return;
}; 
 
  
/*
 * KeProcessExit:
 *     Interface para chamar uma rotina de 
 *     tratamento de fechamento de um processo.
 *     Chama um m�dulo externo para sair de uma tarefa.
 *     Chama um Server em user mode.
 */ 
void KeProcessExit()
{
    return;
};

/*
 * KeKillProcess:
 *     Chama um server em user mode pra
 *     fechar um processo.
 *       
 */ 
void KeKillProcess(int pid)
{
    return;
};

void KeNewProcessScheduler()
{
    return; 
}



int KeSetPriority()
{
    return 0;
}

//Aumenta a prioridade de um processo.
int KeIncreasePriority(int pid)
{
    return 0;
};


 


void KeSaveContextOfNewTask(int id, unsigned long *task_address)
{
    return;
};


void KeReloadCurrentTask()
{
    return;
};


void KeSetQuantum( unsigned long q)
{
    return;
};

unsigned long KeGetQuantum()
{ 
    return 0; 
};

void KeSetCurrentQuantum( unsigned long q)
{
    return;
};

unsigned long KeGetCurrentQuantum()
{ 
    return 0; 
};

void KeSetNextQuantum( unsigned long q)
{
    return;
};

unsigned long KeGetNextQuantum()
{ 
    return 0; 
};


//@todo: N�o faz sentido chamar um m�dulo externo
//para setar o foco.
void KeSetFocus(int pid)
{
    return;
};

//@todo: N�o faz sentido chamar um m�dulo externo
//para pegar o foco.
int KeGetFocus()
{ 
    return (int) 0; 
};

void KeDebugBreakpoint()
{
    return;
};

void KeShowTasksParameters()
{
    return;
};

void KeMostraSlots()
{
    return;
};

void KeMostraSlot(int id)
{
    return;
};

void KeMostraReg(int id)
{
    return;
};

void KeShowPreemptedTask()
{
    return;
};



/*
 * show_process_information:
 *    Mostra informa��es sobre o processo atual.
 *    Current Process Info.
 *    @todo: Mostrar em uma janela pr�pria.
 */
void show_process_information()
{
	int i;
	struct process_d *Current;	
    
	
	//
	// Janela: @deve-se criar a janela e fazer os resultados aparecerem dentro dela.
	//
	
	//struct window_d *hWindow;    //janela. 	
	
	
	printf("\n Current Process Info:\n\n");
	
	//Limits.
	if( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
		return;
	};
	
	//Struct.
	Current = (void*) processList[current_process];
	if( (void*) Current == NULL ){
	    printf("show_process_information:\n");
        return; 		
	}else{
		//Index.
		printf("PID={%d} PPID={%d} UID={%d} GID={%d} \n",Current->pid
		                                                ,Current->ppid
                                                        ,Current->uid
                                                        ,Current->gid);
	    //Name
		printf("Name={%s} \n",Current->name_address);
		
		//Image Address.
		printf("ImageAddress={%x} \n",Current->Image);
		
		//Directory Address. *IMPORTANTE.
		printf("CurrentProcessDirectoryAddress={%x} \n",Current->Directory);		
		
		//Heap and stack.
		printf("Heap={%x}  HeapSize={%d KB}  \n",Current->Heap  , Current->HeapSize );
		printf("Stack={%x} StackSize={%d KB} \n",Current->Stack , Current->StackSize );
		
		//...
	};
	
	
	
	//
	// Testando o for para process.
	//
	printf(" *** Process info *** \n\n");
	
	for( i=0; i<PROCESS_COUNT_MAX; i++)
    {
	    Current = (void *) processList[i];
	    
		//Mostra as tarefas v�lidas, mesmo que estejam com problemas.
		if( (void*) Current != NULL && 
		        Current->used == 1 && 
				Current->magic == 1234 )
	    {
			//@todo: Mostrar quem � o processo pai.
		    printf("PID={%d} Name={%s} Directory={%x} \n",Current->pid ,Current->name_address, Current->Directory);
			//printf("PID={%d} PPID={%d} Name={%s}\n",Current->pid ,Current->ppid, Current->name_address);
	    };
    };	

	//...
	
done:	
    printf("Done.\n");
	refresh_screen();
	return;
};


/*
 * SetProcessDirectory:
 *     Configura o endere�o do diret�rio de p�ginas do processo.
 *     @todo: Isso pode ser um servi�o oferecido pelo kernel,
 * para um gerenciador de processos em user mode usar.
 * @todo: processSetDirectory(...)
 */
void SetProcessDirectory(struct process_d *process, unsigned long Address)
{
    if((void*) process != NULL){
        process->Directory = (unsigned long) Address;        
	};
	return;
};


/*
 * GetProcessDirectory:
 *     Pega o endere�o do diret�rio de p�ginas do processo.
 *     @todo: Isso pode ser um servi�o oferecido pelo kernel,
 * para um gerenciador de processos em user mode usar.
 * @todo: processGetDirectory(...)
 */
unsigned long GetProcessDirectory(struct process_d *process)
{
    if((void*) process != NULL){
        return (unsigned long) process->Directory;
	};
	return (unsigned long) 0;
};


/*
 * GetPageDirValue:
 *     Pega o endere�o do diret�rio de p�ginas do processo.
 *     processGetPageDirValue()
 */
unsigned long GetPageDirValue(){
    return (unsigned long ) get_page_dir();
};



/*
 * init_task:
 *     Inicia um processo.
 *     @todo: Mudar o nome para init_process().
 *            Inicializa a estrutura de um processo criado.
 *            Quando cria o processo, cria apenas sua estrutura,
 *            ficando a cargo dessa rotina inicializar a estrutura criada. 
 */ 
int init_task(int id)
{ 
    //@todo: inicializar uma estrutura para um processo criado.
	//obs: ja tem essa rotina feita em aalgum lugar..
	
    return (int) 0;   
};



/*
 * init_tasks: 
 *     Inicia as vari�veis.
 *
 * @todo: 
 *     Bug bug, problemas na inicializa��o das estruturas.
 *     Implementar a aloca��o din�mica na cria��o das tarefas.
 */
void init_tasks()
{
    init_processes();	
	return;
};



/*
 * init_processes:
 *    Inicaliza o process manager.
 */
void init_processes()
{
    int i;	
	
	//
	// Iniciando vari�veis globais.
	//
	
	kernel_request = 0;    // O que fazer com a tarefa atual.
	kernel_tick = 0;       // ?? Contagem de tempo de execu��o da tarefa atual.	                          
	kernel_switch = 0;     // ?? Ativa o kernel switch do scheduler.
    current_process = 0;
	
	//
	// Inicializando a lista de processos.
	//
	
	i=0;
	while(i < PROCESS_COUNT_MAX){
	    processList[i] = (unsigned long) 0;
        ++i;
	};


	//
	// @todo: mais algo??
	//
	
Done:	
	return;
};



/*  
 * dead_task_collector:
 *     Coletor de processos mortos.    
 * 
 *     Essa rotina deve pegar os processos
 *     que est�o no estado TERMINATED  e zerar
 *     as estruturas, liberar memoria, fechar arquivos...
 *     liberar o espa�o nos slots...
 *  
 *     @todo: Mudar o nome para processDeadProcessCollector();
 *
 */ 
void dead_task_collector()
{
	
	//
	// @todo
	//
	
done: 	
	return;
};


/*
 * exit_process:
 *     Exit process..
 *     Torna o estado PROCESS_TERMINATED, mas n�o destr�i a estrutura do 
 * processo. Outra rotina destruir� as informa��es.
 *     @todo: Liberar a mem�ria e os recursos usado pelo processo.
 *     @todo: Fechar as threads seguindo a lista encadeada.
 */
void exit_process(int pid, int code)
{
	int i;
    struct process_d *Process;
    struct thread_d *Threads;
	struct thread_d *Next;	
	//...
	
	//Limits. 
	if(pid < 0 || pid >= PROCESS_COUNT_MAX){
	    return;	
	};    
	
	//N�o fechar o processo 0.
	if(pid == 0){
		return;
	};

#ifdef KERNEL_VERBOSE	
	//Debug:
	printf("exit_process: Terminating process %d.\n",pid);
	refresh_screen();
#endif	
	
	//
	// Pega o ponteiro para a estrutura.
	//
	
	Process = (void*) processList[pid];
	
	if( (void*) Process == NULL ){
		return;
	}else{	

		//Se estiver corrompida.
        if(Process->used != 1 || Process->magic != PROCESS_MAGIC){
			return;
		};
		
		Process->exit_code = (int) code;    
		Process->state = PROCESS_TERMINATED; 
		//...
	};
		
#ifdef KERNEL_VERBOSE
	//Debug:
	printf("exit_process: Terminating threads..\n");
	refresh_screen();
#endif		

	//pega o primeiro da lista
	//Se o head da list na� foi inicializado corretamente d� page fault.
	Thread = (void*) Process->threadListHead;
		
	//se n�o h� nada na head.	
    if(Thread == NULL){
	    goto done;	
	};		
		
	
    
	//fecha todos da lista.
	while(1)
	{
        //salva o ponteiro para o pr�ximo.
		//qual deve fechar depois.
		
		printf("a\n");
		refresh_screen();
		
		Next = (void*) Thread->Next;
		
		//fim da lista?
		//'Thread' fecha agora.
        if(Thread == NULL){
		    goto done;	
		}else{
    
#ifdef KERNEL_VERBOSE    
		    //fecha a thread.
		    printf("exit_process: KILL thread %d.\n",Thread->tid);
#endif			
			
			kill_thread(Thread->tid);  					
		    
			//prepara qual deve fechar agora.
		    //hav�amos salvo e agora � vez dela.
		    //Obs: estamos reusando o ponteiro.
			Thread = (void*) Next;
		 };
        //Nothing.
	};		
	//nothing
done:
    //
	//@todo:
	//    Escalonar o processo atual. Se o processo fechado foi o processo 
	// atual, precisamos de um novo processo atual. Usaremos o processo zero 
	// por enquanto. Obs: Devemos fazer aqui apenas escalonameto de processo
	// e n�o de thread.
	//
	

	    //Zerando por enquanto.

	//?? Analizar essa parte.	
    //@todo: Select next process (idle)
	current_process = 0;	
    current_thread = 0;    //@todo: Deletar isso.	
	
	//More ?!
	
	//@Limpar a estrutura toda...
	//Process->
	//Process = NULL;
	
	//@todo: tirar da lista.
	
//Nothing.		
	return;
};


int get_caller_process_id(){
	return (int) caller_process_id;
};


void set_caller_process_id(int pid){
	caller_process_id = (int) pid;
	return;
};


/*
 * init_process_manager:
 *     Initialize process manager.
 *     processInitializeProcessManager();
 */
int init_process_manager()
{
	caller_process_id = (int) 0;
	
	processNewPID = (int) USER_BASE_PID;
	
	//...
	
	return (int) 0;
};


/*
 *Constructor.
int processmanagerProcessmanager(){
	;
};
*/

/*
int processmanagerInit(){
	;
};
*/

//pega o endere�o do heap do processo.
unsigned long GetProcessHeapStart(struct process_d *process)
{
    //Estrutura inv�lida.
	if((void*) process == NULL){
        return (unsigned long) 0;        
	};
	
	return (unsigned long) process->Heap;
};


//
// End.
//
