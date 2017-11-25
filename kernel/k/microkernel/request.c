/*
 * File: microkernel\request.c
 *
 * Descri��o:
 *     Arquivo principal da central de requests do kernel.
 *     Rotinas de atendimento dos requests que o kernel recebe dele mesmo. 
 * Requests s�o solicita��es feitas em kernel mode para o kernel atender. Uma 
 * rotina em kernel mode sinaliza um tipo de request atrav�s da configura��o
 * da vari�vel kernel_request e em algum momento da troca e escalonamento de
 * threads esse request � atendido com a chamada da rotina request().
 *     As requests s�o para o funcionamento do kernel, e n�o para atender
 * solicita��es de aplicativos em ring 3. Tamb�m serve para medir algumas 
 * demandas de recursos.
 *
 * * IMPORTANTE: 'Request' ser� considerado 'sin�nimo' de Signal !
 *                Uma rotina de sinal agendar� um request par o 
 *                kernel base atender.
 *                Signal ser� considerado um servi�o e far� parte 
 *                das rotinas do sistema em executive/sys. 
 *
 * Lista de requests:
 * =================
 *
 * KERNEL_REQUEST_NONE: Request null, sem motivo definido.
 * KERNEL_REQUEST_TIME: Contador de vezes que a thread rodou.
 * KERNEL_REQUEST_SLEEP: Faz a thread atual dormir. Entrando no estado waiting.
 * KERNEL_REQUEST_WAKEUP: Acorda a thread atual.
 * KERNEL_REQUEST_ZOMBIE: Torna zombie a thread atual.
 * KERNEL_REQUEST_NEW: Inicia uma nova thread. A thread roda pela primeira vez.
 * Ela estar no estado INITIALIZED.
 * KERNEL_REQUEST_NEXT: ?? Reinicia a execu��o de uma thread.
 * KERNEL_REQUEST_TIMER_TICK: ?? O tick do timer.
 * KERNEL_REQUEST_TIMER_LIMIT: ?? Um limite de contagem.
 * //...
 *
 * Hist�rico:
 *     Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o 1.0, 2016 - Revis�o das rotinas basicas.
 *     //...
 */  
 
 
#include <kernel.h>


/*
 * KiRequest: 
 *    Trata os requests. (eventos, solicita��es , waits ...).
 *    Essa rotina � chamada pelo timer em todas as interrup��es de timer.
 *
 *    @todo: Aqui deve ficar apenas as intefaces que chamam as rotinas de 
 * tratamento dos requests.
 */
void KiRequest()
{
	int Max = 11;
	
    //
    // @todo:
	//     Check some limits.
    //	
	
	//Limits
    if( kernel_request < 0 || kernel_request > Max )
	{
        printf("KiRequest: Number={%d}",kernel_request);
		refresh_screen();
		while(1){}
	};
	
	//...
	
	
done:
    request();
    return;
};


/*
 * request:
 *     Trata os requests do Kernel.
 *     2015 (Fred Nora) - Created.
 *     2016 (FN) - Revis�o.
 *     ...
 */
void request()
{  
	int Current;
	unsigned long r;    //N�mero do request.
	unsigned long t;    //Tipo de thread. (sistema, peri�dica...).
	int Max = 11;	
    struct process_d *Process; 
    struct thread_d  *Thread;	
	
    //
	// Current thread.
	//
	
	Current = (int) current_thread;
	
	//
	// @todo: Filtro.
	//
	
	//if(Current ...){}
	
	Thread = (void *) threadList[Current];
	if( (void*) Thread == NULL )
	{
		//kernel_request = 0;
		//...
		return;    //fail.
	}
	else
	{
	    r = (unsigned long) kernel_request;
	    t = (unsigned long) Thread->type; 
		Process = (void*) Thread->process;
	    //...
	};
	
	//Number limits.
	if( r < 0 || r > Max )
	{
        printf("request: Limits! %d\n",r);
		refresh_screen();
		while(1){}
		return;
    };
	
	//Number.	
	switch(r) 
	{
	    //0 - request sem motivo, (a vari�vel foi negligenciada).
        case KR_NONE:
	        //nothing for now.       
	    break;

	    //1 - Tratar o tempo das threads de acordo com o tipo.  
	    case KR_TIME:		    
			switch(t)
            {
                case TYPE_IDLE:   
                case TYPE_SYSTEM:
                    //Nothing.				
                    break;

                case TYPE_PERIODIC:   //periodic
					/*
	                 * Ticks Remaining: Quando o tempo de execu��o de uma 
	                 * thread se esgota. (N�o faz parte do task switch,     
					 * apenas trata o tempo que a tarefa tem para ficar     
					 * executando). O tempo restante vai diminuindo.     
	                 */
				    Thread->ticks_remaining--;
					if(Thread->ticks_remaining == 0){
		                panic("request: Time out TIP={%d}",Thread->tid);
                        while(1){} 						
	                };
                    break;
					
                case TYPE_RR:   //round robin.
				    //Nothing.
					break;
					
				//@todo: Implementar os outros tipos.	
					
				default:
				    //Tipo inv�lido.
                    break;						
            } 			   
	        break;
		
	    //2 - faz a current_thread dormir. 
   	    case KR_SLEEP:   
		    do_thread_sleeping(Current);
	        break;
         
	    //3 - acorda a current_thread.
	    case KR_WAKEUP:
		    //
		    break;

	    //4 - torna a current_thread zombie.
	    case KR_ZOMBIE:
	        do_thread_zombie(Current);
		    break;
			
		//5 - start new task.
		//Uma nova thread passa a ser a current, para rodar pela primeira vez.
		case KR_NEW:	
		    //Start a new thread. 
	        if(start_new_task_status == 1){		
		        current_thread = start_new_task_id;
	        };
		    break;
			
        //6 - torna atual a pr�xima thread anunciada pela atual.
		case KR_NEXT:
            current_thread = (int) Thread->Next->tid; 
			break;	
			
		//7 - tick do timer.
		case KR_TIMER_TICK:
		    panic("KR_TIMER_TICK");
			while(1){};
		    break;
        
		//8 - limite de funcionamento do kernel.
        case KR_TIMER_LIMIT:
		    panic("* KR_TIMER_LIMIT!\n");
			while(1){};
		    break;
			
		//9 - checa se ha threads para serem inicializadas e 
		//inicializa pelo m�todo spawn.	
		case KR_CHECK_INITIALIZED:
            check_for_standby();		
		    break;
			
		//@todo: Tratar mais tipos.	
		
		default:
		    //printf("Default Request={%d}\n",r);
		    break;
    };

    //
    // More ?!
    //	
	
/*
 * Done:
 *   Essas finaliza��es aplicam para todos os requests.
 */	
done:
    //thread->step++;                      //Aumenta o step para todos os tipos.
    kernel_request = (unsigned long) 0;    //Zera o request.
	return; 
};

//
// End.
//

