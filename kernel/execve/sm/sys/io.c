/*
 * File: io.c
 *
 * Descri��o:
 *     Arquivo principal do I/O Manager.
 *     I/O control. 
 *     Obs: Note que o I/O Manager est� fora do executive, mas faz parte 
 *          do Kernel Base.
 *
 * Todo gerenciamento de dispositivos, toda concess�o de acesso passar� por 
 * esse gerenciador, que faz parte da camada de abstra��o de hardware 
 * oferecida pelo kernel base.
 * Essa parte do gerenciamento de i/o est� no kernel base em kernel mode. 
 * Porem haver� outras partes. Haver� um subsistema em user mode, e talvez 
 * um m�dulo externo do kernel em kernel mode.
 *
 * I/O system: Inclui I/O manager
 *
 *  @todo: Cria fun��es para gerenciar ioBuffers.
 * The actual Streams modules lives in kernel space on Unix, and are 
 * installed (pushed) and removed (popped) by the ioctl system call.
 * Vers�o 1.0, 2016.
 */
 
 
#include <kernel.h> 



/*
 * ioServices:
 * Servi�os de i/o oferecidos pelo kernel base.
 * Quando o m�dulo 'service.c' receber chamadas para servi�os de i/o 
 * ele pode desviar as chamadas para serem atendidas por essa rotina.
 */

void *ioServices ( unsigned long number, 
                   unsigned long arg2, 
                   unsigned long arg3, 
                   unsigned long arg4 )
{

    struct process_d *P;
    int Caller_Process_ID;

	// #todo: 
	// O subsistema em user mode deve fazer algum tipo de filtragem
	// pra autorizar ou n�o um processo � usar o sistema de i/o.
	// Por�m aqui tamb�m � feito alguma filtragem pra conferir autoriza��o.
	// @todo:
	// Checar a token do processo, se a token do tipo HAL est� habilitada
	// para o processo que chama a rotina.	

	//
	// Id do processo que est� chamando a rotina de i/o.
	//


	//@todo: Filtrar argumentos.
	
	
	
	Caller_Process_ID = (int) get_caller_process_id();
	
	if ( Caller_Process_ID <0 || Caller_Process_ID >= PROCESS_COUNT_MAX )
	{
		//@todo: Mensagem de erro.
		return NULL;
	};
	
	
	P = (void *) processList[Caller_Process_ID];
	//if( (void*) P == NULL )
	//{
	    //@todo: Mensagem de erro.	
	//};
	
	//
	// Chama o servi�o solicitado.
	//
	
	switch(number)
    {
	    case 0:
		    //Nothing.
            break;
			
        case 1:
            //Nothing.
            break; 
        //...
		
		default:
		    //Nothing.
		    break;
	};


	//...


    return NULL;
}
   


/*
 * ioInit:
 *     Initialize i/o manager.
 */	
 
int ioInit (void){
	
	int Status = 0;
	
	//...
	
	return (int) Status;
}


//
// End.
//

