/*
 * File: abort.c
 *
 * Descri��o:
 *     Rotina de aborto do kernel em caso de erro grave.
 *
 *    Vers�o 1.0, 2015, 2016. 
 */

 
#include <kernel.h>


/*
 * KeAbort:
 *     Interface para chamar m�dulo externo.
 *     Aborta o sistema atrav�z de chamada � m�dulo externo.
 *     Chama um m�dulo externo para opera��es de finaliza��o
 *     do sistema.
 *     Chama m�dulo externo (Server em user mode).
 */
/*
void KeAbort (){
	
    printf("KeAbort:");
    die();
};
*/



/*
 * systemAbort:
 *     Chama interface pra abortar todo o sistema
 *     inclusive o kernel.
 */

void systemAbort (void){
	
    KiAbort (); 
    
	//
	// Sem retorno !
	//
}



/*
 * KiAbort:
 *     Interface para abortar o kernel..
 *     Sem retorno !
 */

void KiAbort (void){
    
    abort ();	
}


/*
 ***********************************************************
 * abort:
 *     M�dulo interno do tipo MB. Faz parte do Kernel Base.
 *     Aborta o kernel e mostra o erro.
 *     Aborta o kernel devido a falhas do kernel.
 *
 *     @todo: Pode-se criar um arquivo de log de erro.
 *           //...
 */ 

void abort (void){
	
	asm (" cli ");
	
	debug_print("abort:\n");	
	
	save_current_context();
	
	if (KernelStatus != KERNEL_ABORTED)
	{
        KernelStatus = KERNEL_ABORTED;
	    scheduler_lock();  //@todo: tirar daqui.
		//...
	};
	
	//scheduler_lock();
	
	/*
	 * Daqui pra baixo s�o mensagens.
	 */    
	
	printf ("abort: Kernel aborted\n");
	
	//Confere a fase de inicializa��o. Se todas as foses foram cumpridas.
	if(KeInitPhase < 3){
	    printf("KeAbort: KeInitPhase={%d}\n",KeInitPhase);
	};
	
	
    // Mostra o n�mero do �ltimo erro.
	
	 
    switch (errno)
	{
	    case 0:
		    printf("erro %d\n", errno);
		    break;

	    //tempo esgotado pra uma tarefa sens�vel ao tempo.		 
	    case 1:        
		    printf("Acabou o tempo da Thread={%d}\n",current_thread);
		    break;

	    case 2:
		    printf("erro %d\n", errno);
		    break;
			
		//...
		
		default:
		    printf("default errno %d\n", errno);
		    break;
	};
    		
	mostra_slot(current_thread);
	mostra_reg(current_thread);
	
	
    //  Final message !
    
	 
	
	//@todo: Usar apenas uma printf.
	
	printf("* \n");                    //Bullet!
    printf("System Halted!\n");        //Message.
	
	if(VideoBlock.useGui == 1){
	    refresh_screen();
	};
	
    
	/*
     *  No return.
     */	  
	 
	while (1){         
		
	    asm("cli");
	    asm("hlt");                 
	
	};                          
};


//
// End.
//

