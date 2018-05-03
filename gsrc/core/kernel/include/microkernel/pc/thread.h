/*
 * File: microkernel\thread.h
 *
 * Descri��o:
 *     Header para threads.
 *     Pertence ao m�dulo microkernel, dentro do kernel.
 *
 * @todo: Contagem de threads.
 * Vers�o 1.0, 2015, 2016.
 */
 
typedef int tid_t; 
 
//
//#define FIRST_THREAD theadList[0]
//#define LAST_THREAD  threadList[THREAD_COUNT_MAX-1] 
 
 
// N�mero m�ximo de threads.
#define THREAD_COUNT_MAX 256 

//O primeiro �ndice na contagem das threads do sistema e o
//primeiro �ndice na contagem das threads dos usu�rios.
#define SYSTEM_BASE_TID 0  
#define USER_BASE_TID 100  

//
// Preempt support.
//

//?? Se pode ou n�o entrar em preemp��o. 
#define    PREEMPTABLE 1  //PODE ENTRAR
#define  UNPREEMPTABLE 0  //NAO PODE ENTRAR EM PREEMP��O
#define  PREEMPT_PODE     PREEMPTABLE
#define  PREEMPT_NAOPODE  UNPREEMPTABLE

  
// Identificadores de thread.
#define IDLE_ID  0 
#define IDLE  IDLE_ID 
#define THREAD_IDLE  IDLE_ID  
//... 
 
 
#define THREAD_MAGIC 1234
 
 
 
/*
 * Globais.
 */ 
 
 
 
/*
 * Estruturas para threads.
 */
 
  
/*
 * thread_event_type_t:
 *     Enumerando os tipos de eventos que fazem a tarefa entrar em modo de 
 * espera:
 *
 *  EVENT_NULL       - Nulo. 
 *	EVENT_PREMMPTED  - Preemp��o
 *  EVENT_SEMAPHORE  - Sem�foro.
 *  //...
 */
typedef enum {
	EVENT_NULL,           // Nulo. 
	EVENT_PREMMPTED,      // Preemp��o.
	EVENT_SEMAPHORE,      // Sem�foro.
	//continua... @todo
}thread_event_type_t;
 
 
/*
 * thread_type_t:
 *     Enumerando os tipos de threads:
 *
 *   idle     - Threads do tipo idle.     
 *   rr       - Threads do tipo round robin.
 *   periodic - Threads do tipo peri�dicas. 
 *   system   - Threads do tipo system.
 *   ...
 *
 * Ordem de implanta��o:
 * ====================
 *     (De acordo com a ordem de constru��o de um sistema).
 *
 *    TYPE_NULL     0
 *    TYPE_SYSTEM   1  // Fundamentais para o funcionamento do sistema.  
 *                     // Primeiras tarefas rodando quando o O.S. est� sendo criado.
 *                    
 *    TYPE_IDLE     2  // Exclusivo para Threads do tipo idle. 
 *                     // ( V�rios tipos de rotinas podem ser realizadas durante o per�odo
 *                     // que o processador estiver ocioso. O usu�rio pode configurar
 *                     // o que o sistema deve fazer nesses momentos de ociosidade).
 *					   
 *    TYPE_PERIODIC 3  // Tarefas peri�dicas. 
 *                     // (Rodam de tempos em tempos, como o deadthread collector).
 *
 *    TYPE_RR       4  // Threads tipo round robin. 
 *                     //(Confinadas em um processador, N�o importa a prioridade, nem o deadline).
 *
 *    TYPE_REALTIME 5  // Realtime + round robin. 
 *                     // (Confinadas em um processador, Importa a prioridade o dead line, o step
 *                     //  principalmente � sistemicamente importante que se cumpra a execu��o em tempo.) 
 *
 *    TYPE_UI       6  // UI user interface thread @todo:
 * 
 *    TYPE_IO       7  // i/o thread @todo
 *
 *    Continua ...
 *
 */
typedef enum {
    TYPE_NULL,
	TYPE_SYSTEM,	 // first-come-first-served.
	TYPE_IDLE,       // 
	TYPE_PERIODIC,   // periodic threads with predefined intervals.
	TYPE_RR,         // first-come-first-served cooperative.
    TYPE_REALTIME,
	//TYPE_UI, //@todo
	//TYPE_IO, //@todo
	//...
}thread_type_t;


/*
 * thread_state_t:
 *    Enumeram os estados de uma threads, (8 estados).
 *    Existem 2 grupos, 'Earth' e 'Space'.
 *    Obs: Rodam no 'espa�o'.
 *
 * Earth: (INITIALIZED, STANDBY, ZOMBIE, DEAD).
 * Space: (READY, RUNNING, WAITING, BLOCKED).
 *
 *  INITIALIZED,    //Earth, Criado o contexto e par�metros.
 *  STANDBY,        //Earth, Pronta para rodar pela primeira vez. Ir para o 'espa�o'.
 *  ZOMBIE,         //Earth, Terminou a execu��o. Voltou para a 'terra'.
 *  DEAD,	        //Earth, Deleted.	
 
 *  READY,          //Space, Thread is ready to run again.
 *  RUNNING,        //Space, Thread is currently running.
 *  WAITING,        //Space, Thread is waiting.	
 *  BLOCKED,        //Space, Thread is blocked by an event.
 *
 * Obs: 
 *     Na pr�tica, a troca de status est� seguindo um organograma de 
 * movimentos. 
 *     @todo: Descrever esses movimentos na documenta��o. Incluir a 
 * visualiza��o atrav�s do gr�fico.
 *
 */
typedef enum {
    INITIALIZED,    //0 Earth, Criado o contexto e par�metros.
	STANDBY,        //1 Earth, Pronta para rodar pela primeira vez. Ir para o 'espa�o'.
	ZOMBIE,         //2 Earth, Terminou a execu��o. Voltou para a 'terra'.
	DEAD,	        //3 Earth, Deleted.	
	READY,          //4 Space, Thread is ready to run again.
	RUNNING,        //5 Space, Thread is currently running.
	WAITING,        //6 Space, Thread is waiting.	
	BLOCKED,        //7 Space, Thread is blocked by an event.
}thread_state_t;

 
/*
 * thread_d: 
 *
 *    TCB - Thread Control Block.
 *
 *    A estrutura onde guarda informa��es sobre a thread.
 *    @todo: Come�ar com informa��es sobre disco e arquivos.
 *    Obs: Deve ficar por �ltimo o que for referenciado com menos frequ�ncia.
 */
typedef struct thread_d thread_t;
struct thread_d 
{	
	object_type_t objectType;
	object_class_t objectClass;
	
	//call back //d
	
    //
	// Identificadores.
	//
	
	int tid;                //c, thread ID.   
	
	int ownerPID;           //ID do processo ao qual o thread pertencer. 
	//int ownerPPID;        //Acho que isso n�o � necess�rio !! 
	
	unsigned long used;     //a, @todo: Poderia ser int.
    unsigned long magic;    //g, @todo: Poderia ser int.
	
	thread_state_t state;    //f, flag, Estado atual da tarefa. ( RUNNING, DEAD ...).	
	
	//e, error. @todo:
	//unsigned long error;
	
	//ext.
	
	
	
	//
	// Names.
	//
	
	//char *name;  //@todo: Usar isso.
	unsigned long name_address;
	unsigned long name_pointer;   	
	char short_name[4];
	char *cmd;	   
	
    //
	// CPU.
	//
    
	int cpuID;            //Qual processador.
	int confined;         //Flag, confinado ou n�o.
	int CurrentProcessor;
	int NextProcessor;
	//int IdealProcessor;
	
	
	//
	// ORDEM: O que segue � referenciado durante a interrup��o de timer.
	//

	
	//...
	
	//
	// ORDEM: O que segue � referenciado durante o processo de task switch.
	//
	
	/*
	 * Page directory information:
     *
	 *   ** COLOCAR O DIRET�RIO DE P�GINAS QUE A THREAD USA
	 *      AJUDA NA HORA DO TASKSWITCH **
	 */
	unsigned long Directory; 


	
	//IOPL of the task. (ring).
	unsigned long iopl; 
	
	/*
	 * Contexto. @todo: usars uma estrutura.
	 */
	unsigned short ss;
    unsigned long esp;
    unsigned long eflags;
    unsigned short cs;
    unsigned long eip;	
    unsigned short ds;
    unsigned short es;
    unsigned short fs;
    unsigned short gs;
    unsigned long eax;	
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long esi;
    unsigned long edi;
    unsigned long ebp;
	//continua o contexto ...	
	
	
	//
	// ORDEM: O que segue � referenciado durante o processo de scheduler.
	//
	
	//poderia ser base_priority e dinamic_priority.
	unsigned long base_priority;  //Prioridade b�sica.
	unsigned long priority;         //Prioridade din�mica.
	
	
	
	/*
	 * preempted:
	 *     flag ~ Sinaliza que uma tarefa pode ou n�o sofrer preemp��o.
     *	   Uma tarefa de menor prioridade pode deixar o estado running 
	 * para assumir o estado ready em favor de uma tarefa de maior prioridade
	 * que assumir� o estado running.
	 *
	 */
	unsigned long preempted;
	
	//
	// ORDEM: O que segue � referenciado durante o processo de dispatch.
	//
	
	/*
	 * save ~ Sinaliza que a tarefa teve o seu contexto salvo.
	 */
	unsigned long saved;
	
	
	/*
	 * HEAP and STACK:
     * @todo: Usar a estrutura.
     *	 
	 */
	//struct heap_d *heap;
	
	unsigned long Heap;
	unsigned long Stack;
	unsigned long HeapSize;
	unsigned long StackSize;
	

	//endere�o de um array contendo ponteiros para variso servi�os
	//que a thread pode usar.
	//unsigned long ServiceTable;
	
	/* 
	 * Temporiza��o da tarefa. 
	 */
	
	//
    // @todo: Ticks and Deadline.
	//
	// Quanto tempo passou, mesmo quando a tarefa n�o esteve rodando.
	// Quando tempo a tarefa tem para que ela complete a sua execu��o.
	//
	//unsigned long Ticks;
    //unsigned long DeadLine.
	
	//Steps.
	unsigned long step;            //Quantas vezes a tarefa usou o processador. 		
	
	//Quantum. (time-slice, igual cota) 
	unsigned long quantum;         //tempo que a thread tem.
	unsigned long quantum_limit;   //limite quando dado boost. tempo limite rodando. 		
	
	//unsigned long  RemainingTime;  //??
	
	unsigned long standbyCount;
	
	//Contando tempo rodadndo.
	unsigned long runningCount; //Tempo rodando antes de parar.

	//Obs: A soma das 3 esperas � a soma do tempo de espera
	//depois que ela rodou pela primeira vez.
	
    //Contando o tempo nos estados de espera.
	unsigned long readyCount;   //tempo de espera para retomar a execu��o.
	unsigned long ready_limit;
	unsigned long waitingCount; //tempo esperando algo.	
	unsigned long waiting_limit;   //tempo limite que uma tarefa ready fica sem rodar.
    unsigned long blockedCount;
	unsigned long blocked_limit;
	
	
	//Ticks remaining. (tempo para a tarefa chegar ao fim, tempo total-tempo percorrito)
	unsigned long ticks_remaining; //rt, quanto tempo a tarefa tem dispon�vel para ser concluida.

	//unsigned long alarm;            //Tempo para o pr�ximo alarme, dado em ticks.
	
	//??iopl??
	unsigned long PreviousMode;	

	
	/*
     * @todo: afinidade e liga��o entre tarefas.
	 */
    //int idealprocessornumber;
	
	
	//
	// event: 
	// ?? Tipo de evento que fazem a tarefa entrar em modo de espera. 
	//	
    thread_event_type_t event;

    //
	// type: 
	// Tipo de tarefa.
    // (SYSTEM, PERIODIC, RR, IDLE).	
	//
	thread_type_t type;	
		
		
	//
	// ORDEM: O que segue � referenciado com pouca frequencia.
	//
	
	//lista de arquivos ??
	//fluxo padr�o. stdio, stdout, stderr
	//unsigned long iob[8];
    
	//ponteiros para as streams do fluxo padr�o.
	unsigned long standard_streams[3];
	
	struct _iobuf *root;	// 4 root directory
	struct _iobuf *pwd;	    // 5 (print working directory) 
	//...
		
	
	
	//@todo: Uma thread pode estar esperando varias outras por motivos diferenes.
	//struct wait_d WaitBlock;
	


	

    //process.
	//� qual processo pertence a thread.
	struct process_d *process; 

	
	/*
	 * Janela e procedimento.
	 */
	struct wstation_d *window_station;  //Window Station da thread.
	struct desktop_d  *desktop;         //Desktop da thread.
	
	unsigned long procedure; //Endere�o do procedimento de janela da tarefa. 
	//unsigned long control_menu_procedure; //procedimento do control menu.
	
	//
	//ORDEM: O que segue � referenciado durante as trocas de mensagens.
	//       utiliza��o de canais e IPC.
	//
	
	struct window_d *window;    //arg1.
	int msg;                    //arg2.
	unsigned long long1;        //arg3.
	unsigned long long2;        //arg4.
	
	//?? mensagens pendentes.
	//struct thread_d *sendersList; //Lista encadeada de threads querendo enviar mensagem
	//struct thread_d *nextSender;  //pr�xima thread a enviar mensagem.
	
	/*
	 * wait4pid: 
	 * Uma thread pode estar esperando um processo fechar para que ela prossiga.
	 */
	unsigned long wait4pid;    //@todo: pode ser 'int'.
	
		
    //Motivo da thread fechar.
	int exit_code; 	
	

	//Signal
	unsigned long signal;
	unsigned long signalMask;
	
	//Next: 
    //Um ponteiro para a pr�xima thread da lista linkada. 
	struct thread_d *Next;
};
struct thread_d *IdleThread;       // Idle Thread. TID=0
struct thread_d *ShellThread;      // Shell Thread. TID=1
struct thread_d *TaskManThread;    // TaskMan Thread. TID=2

struct thread_d *Thread;           // Current.
//outros.
//struct thread_d *CurrentThread;  
struct thread_d *idle_thread;    // Iddle.
struct thread_d *cur_thread;     // Current.
struct thread_d *blocked_list_head;
struct thread_d *waiting_list_head;
struct thread_d *ready_list_head;
//Lista linkada de threads.
struct thread_d *Conductor2;
struct thread_d *Conductor;
struct thread_d *rootConductor;


int conductorIndex;

/*
 * threadList:
 *   
 *   **** LONG-TERM SCHEDULER FOR THREADS ****
 *
 */
unsigned long threadList[THREAD_COUNT_MAX];



/*
 * thread_list_d:
 *
 */ 
typedef struct thread_list_d thread_list_t; 
struct thread_list_d 
{   
	struct thread_d *Threads;    //List.	
	unsigned long priority;      //Prioridade.
	//...
};
//thread_list_t DispatcherList[10]; //list
//thread_list_t DispathcerPriorityQueue[10];
//unsigned long DispatcherList[10];

//DispatcherList[2].Threads[4].tid




/*
 * create_thread:
 *
 */ 
struct thread_d *create_thread( struct wstation_d *window_station,
                                    struct desktop_d  *desktop,
                                    struct window_d *window,
                                    unsigned long init_eip, 
                                    unsigned long init_stack, 
									int pid, 
									char *name);
void *GetCurrentThread();
void *FindReadyThread();
int GetThreadState(struct thread_d *Thread);
int GetThreadType(struct thread_d *Thread);
void dispatch_thread(struct thread_d *thread);
void set_thread_priority(struct thread_d *t, unsigned long priority);


/*
 * Page directory support.
 */
unsigned long GetThreadDirectory(struct thread_d *thread);
void SetThreadDirectory(struct thread_d *thread, unsigned long Address);


/*
 * Thread heap support.
 */
unsigned long GetThreadHeapStart(struct thread_d *thread);
//...

/*
 * Thread Stack support.
 */
unsigned long GetThreadStackStart(struct thread_d *thread);
//...

void SelectForExecution(struct thread_d *Thread);


void KiShowThreadList();
void show_thread_information(); 
int init_threads();

int GetCurrentThreadId();


//
// Finaliza��es...
//

void release(struct thread_d *t);
void exit_thread(int tid);       //Torna zunbi uma thread.
void kill_thread(int tid);       //Destr�i uma thread.
void dead_thread_collector();

//
//fim.
//


