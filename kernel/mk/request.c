/*
 * File: mk\request.c
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
 * KERNEL_REQUEST_NONE: 
 *     Request null, sem motivo definido.
 * KERNEL_REQUEST_TIME: 
 *     Contador de vezes que a thread rodou.
 * KERNEL_REQUEST_SLEEP: 
 *     Faz a thread atual dormir. Entrando no estado waiting.
 * KERNEL_REQUEST_WAKEUP: 
 *     Acorda a thread atual.
 * KERNEL_REQUEST_ZOMBIE: 
 *     Torna zombie a thread atual.
 * KERNEL_REQUEST_NEW: 
 *     Inicia uma nova thread. A thread roda pela primeira vez.
 *     Ela estar no estado INITIALIZED.
 * KERNEL_REQUEST_NEXT: 
 *     ?? Reinicia a execu��o de uma thread.
 * KERNEL_REQUEST_TIMER_TICK: 
 *      ?? O tick do timer.
 * KERNEL_REQUEST_TIMER_LIMIT: 
 *     ?? Um limite de contagem.
 * //...
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     //...
 */  
 
 
/*
    # planejando pr�ximos eventos ...

	{
	#bugbug: Situa��o em que temos que acordar uma threa por causa de evento de input.	
	+ Ocorreu um evento de input, devemos acordar a trhead que estava esperando por ele.
	#importante: Me parece que esse caso n�o deve ser tratado aqui e sim na hora do 
	tratamento da interrup��o, pois input n�o pode esperar. O tempo de resposta � importante.
	Lembrando que esse tratamento de requests � 'adiado' at� o fim da rotina de interrup��o 
	de timer.
	}
	
	{
	   ...	
	}
	

*/ 
 
 
 
#include <kernel.h>


/*
 *********************************************************
 * KiRequest: 
 *    Trata os requests. 
 *    S�o servi�os que ter�o seu atendimento atrazado at�
 * pouco antes de retornar da interrup��o do timer.
 * S�o servi�os para o ambiente ring0.    
 *
 *    (eventos, solicita��es , waits ...).
 *    Essa rotina � chamada em todas as 
 * interrup��es de timer.
 *
 *    @todo: Aqui deve ficar apenas as intefaces que 
 * chamam as rotinas de tratamento dos requests.
 */
 
void KiRequest (){
	
	//#todo: #bugbug: limite imposto
	int Max = 11;
	
    //
    // @todo:
	//     Check some limits.
    //	
	
	// Limits
    if( kernel_request < 0 || 
	    kernel_request > Max )
	{
        printf("KiRequest: %d", kernel_request );
		die();
	};
	
	//...
    
	request ();
};


/*
 *******************************************************
 * request:
 *     Trata os requests do Kernel.
 *    S�o servi�os que ter�o seu atendimento atrazado at�
 * pouco antes de retornar da interrup��o do timer.
 * S�o servi�os para o ambiente ring0.  
 *     2015 (Fred Nora) - Created.
 *     2016 (FN) - Revis�o.
 *     ...
 */
 
void request (){
	
    struct process_d *Process; 
    struct thread_d *Thread;	

	int Current;
	unsigned long r;    //N�mero do request.
	unsigned long t;    //Tipo de thread. (sistema, peri�dica...).
	int Max = 11;	
	
   
	// Current thread.
	Current = (int) current_thread;
	
	//
	// @todo: Filtro.
	//
	
	//if(Current ...){}
	
	Thread = (void *) threadList[Current];
	
	if ( (void *) Thread == NULL )
	{
		//kernel_request = 0;
		//...
		return;    //fail.
	
	} else {
		
	    r = (unsigned long) kernel_request;
	    
		t = (unsigned long) Thread->type; 
		
		Process = (void *) Thread->process;
	    
		//...
	};
	
	//Number limits.
	
	//#bugbug
	//obs: Estamos checando se uma vari�vel unsigned long � menor que zero.
	//     Isso  n�o � necess�rio.	
	
	if( r < 0 || r > Max )
	{
        printf("request: Limits! %d\n",r);
		die();
    };
	
    //
	// # Number #
	//
	
	switch(r) 
	{
	    //0 - request sem motivo, (a vari�vel foi negligenciada).
        case KR_NONE:
	        //nothing for now.       
	    break;

	    //1 - Tratar o tempo das threads de acordo com o tipo.  
		//#importante: De acordo com o tipo de thread.
	    case KR_TIME:		    
			switch (t)
            {
                case TYPE_IDLE:   
                case TYPE_SYSTEM:
                    //Nothing.				
                    break;

				//periodic
	            //Ticks Remaining: Quando o tempo de execu��o de uma 
	            //thread se esgota. (N�o faz parte do task switch,     
				//apenas trata o tempo que a tarefa tem para ficar     
				//executando). O tempo restante vai diminuindo.     					
                case TYPE_PERIODIC:   
				    Thread->ticks_remaining--;
					if( Thread->ticks_remaining == 0 )
					{
		                panic("request: Time out TIP={%d}", Thread->tid );					
	                };
                    break;
					
				//round robin.	
                case TYPE_RR:   
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
		    do_thread_sleeping (Current);
	        break;
         
	    //3 - acorda a current_thread.
	    case KR_WAKEUP:
		    //
		    break;

	    //4 - torna a current_thread zombie.
	    case KR_ZOMBIE:
	        do_thread_zombie (Current);
		    break;
			
		//5 - start new task.
		//Uma nova thread passa a ser a current, para rodar pela primeira vez.
		case KR_NEW:	
		    //Start a new thread. 
	        if(start_new_task_status == 1)
			{		
		        current_thread = start_new_task_id;
	        };
		    break;
			
        //6 - torna atual a pr�xima thread anunciada pela atual.
		case KR_NEXT:
            current_thread = (int) Thread->Next->tid; 
			break;	
			
		//7 - tick do timer.
		case KR_TIMER_TICK:
		    panic("KR_TIMER_TICK\n");
			//die();
		    break;
        
		//8 - limite de funcionamento do kernel.
        case KR_TIMER_LIMIT:
		    panic("KR_TIMER_LIMIT\n");
			//die();
		    break;
			
		//9 - Checa se ha threads para serem inicializadas e 
		// inicializa pelo m�todo spawn.	
		case KR_CHECK_INITIALIZED:
            check_for_standby();		
		    break;
			
		//#todo
		//Chama o procedimento do sistema.
        // ?? args ??	
        // o servi�o 124 aciona esse request.		
		case KR_DEFERED_SYSTEMPROCEDURE:
		    system_procedure ( 0, 0, 0, 0 );
			break;
			
		
		//@todo: Tratar mais tipos.	
		//...
		
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

