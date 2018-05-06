/*
 * Gramado Executive - The main file for the Executive module in the 
 * kernel base. 
 * (c) Copyright 2015-2016 Fred Nora.
 *
 * File: k\executive.c 
 * 
 * Classes:
 *     system.ram (K5)
 *
 * Obs: O executive lida com gerentes que manipulam rotinas independentes
 *      de dispositivos. Seu ambiente natural � a RAM. � o ambiente de maior
 *      liberdade, sem as restri��es impostas pelos dispositivos externos.
 *      O ambiente do executive � ideal para oferecer vari�veis e fun��es globais.
 * e p�blicas. O ideal � que os processos em user mode somente tenham acesso
 * ao que � oferecido por essa camada.
 *      N�o � pertinente a inclus�o de c�digo assembly nessa camada.
 *
 * Descri��o:
 *     Arquivo principal do m�dulo Executive do Kernel Base.
 *     MB - M�dulos inclu�dos no Kernel Base.
 *     Cont�m drivers b�sicos em Kernel Mode, 
 *     compilados dentro do Kernel Base.
 *     Cont�m Managers e Servers e o SubSystem que recebe as 
 *     chamadas do usu�rio.
 *
 *   OBS: 
 *   *IMPORTANTE  
 *   Os m�dulos do executive s�o incluidos ao 'Kernel Base'
 *   na hora da compila��o, pois s�o excenciais para o 
 *   funcionamento do Kernel.
 *
 *   M�dulos em Kernel Mode tamb�m poder�o ser ligados dinamicamente, 
 *   mas n�o podem ser sistemicamente importantes. Pois n�o � certo deixar
 *   o usu�rio incluir um m�dulo que possa travar o sistema.
 *
 * @todo:
 * Os servi�os oferecidos por esse m�dulo aparecer�o aqui 
 * come�ando por 'sys_'. Corrigindo, na verdade os servi�os
 * devem come�ar por 'systemXXXXX'.
 *
 * executiveExecutive(); � o construtor
 *
 * Vers�o 1.0, 2015, 2016.
 */
 
 
#include <kernel.h>
//#include "eidata.h"  //Executive Internal Data.

//unsigned char  EcecutiveName[] = "EXECUTIVE LAYER";


/*
 *****************************************************************
 * executive_gramado_core_init_execve:
 *     Executa um programa no processo INIT 
 * dentro do ambiente Gramado Core. 
 * #obs: Isso funcionou.
 */
int executive_gramado_core_init_execve( const char *filename, 
                                        const char *argv[], 
                                        const char *envp[] )
{
	struct thread_d *Thread;
    
	//fail.
	int Status = 1;
	
	//fail.
	//if( (const char *) filename == NULL ){
	//	return 1;
	//}
	
	//
	// Testando carregar um programa para 
	// rodar no processo INIT, usando a thread 
	// prim�ria do processo !
	//
	
	printf("\nexecutive_gramado_core_init_execve: testing ...\n");
	printf("fileneme={%s}\n",filename);
	printf("arg={%x}\n",argv[0]);
	printf("env={%x}\n",envp[0]);
	
	//
	// Pegar o ponteiro da thread prim�ria do processo 
	// INIT.
	//
	
	Thread = (struct thread_d *) threadList[0];
	if( (void*) Thread == NULL )
	{
		goto fail;
	}else{
		
		if( Thread->used != 1 || Thread->magic != 1234 ){
			goto fail;
		}
		
		// Significa que o contexto nunca foi salvo ...
		// isso � importante, pois o spawn n�o funciona em thread 
		// com o contexto salvo.
		Thread->saved = 0; 
		
	    //Context.
	    //@todo: Isso deve ser uma estrutura de contexto.
	    Thread->ss  = 0x23;                          //RING 3.
	    Thread->esp = (unsigned long) 0x0044FFF0;    //idleStack; (*** RING 3)
	    Thread->eflags = 0x3200;  //0x3202, pois o bit 1 � reservado e est� sempre ligado.
	    Thread->cs = 0x1B;                                
	    Thread->eip = (unsigned long) 0x00401000;   //entry point  	                                               
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
		
		
		Thread->Next = NULL;
		
		//
		// Load file.
		//
 		
		
		//
		// #bugbug
		// #importante Precisamos do ponteiro v�lido para filename.
		// N�o podemos auter�-lo e depois us�-lo.
		//
		
		//fs/read.c
	    // "FILE    BIN"
        Status = (int) fsLoadFile( (unsigned char *) filename, 
		                           (unsigned long) 0x00400000 );

        //fail
		if( Status == 1 )
		{
			// @todo:
			// Configurar estrutura.
			goto fail;
		};
		
		// Se deu certo.
		if( Status == 0 )
		{
		    queue_insert_data(queue, (unsigned long) Thread, QUEUE_INITIALIZED);
            SelectForExecution(Thread);    // * MOVEMENT 1 ( Initialized ---> Standby ).
            goto done; 
        };	
         
        //fail		 
	};
	
	//fail
	
fail:
    printf("fail ");
done:
    printf("done\n");	
	refresh_screen();
	return (int) Status;	
};


/*
void executiveMain();
void executiveMain(){
	return;
};
*/

/*
 * executive_config_exported_functions:
 *
 *     Configura a tabela do Kernel de fun��es exportadas
 *	   e a tabela de ponteiros para tabelas dos outros programas em Kernel Mode.
 *
 */
/*
int executive_config_exported_functions(); 
int executive_config_exported_functions()
{
	
    ring0_exported = (void*) malloc(sizeof(struct ring0_exported_d));
	
	if( (void*) ring0_exported == NULL )
	{
	    return (int) 1;
	}
	else
	{
	    //ring0_exported->bm_exported  = ( ponteiro para a tabela de fun��es exportadas pelo boot manager)
		//ring0_exported->bl_exported  = ( ponteiro para a tabela de fun��es exportadas pelo boot loader)
		//ring0_exported->bk_exported  = ( ponteiro para a tabela de fun��es exportadas pelo kernel)
		
		
		//@todo: Criar a tabela do kernel e salvar o ponteiro nessa estrutura ai.
		//       Pegar os ponteiros de bootmanager e bootloader e colocar ai.
		
	};
	
	
	// @todo: Nothing more?
	
done:
    return (int) 0;
};

*/

/*
 * sys_showkernelinfo:
 *     Show kernel info.
 */
void sys_showkernelinfo()
{
	KiInformation();
	return;
};


/*
 * init_executive:
 *     Initialize the kernel executive.
 *     Archtecture (independent) inicialization. 
 */
int init_executive()
{
    int Status = 0;
	
	printf("EXECUTIVE:\n");
	
	//  PCI - Pega informa��es da PCI.
	init_pci();
	
	// CLOCK - Pega informa��es de Hora e Data.
    init_clock();
	
	//configura a tabela do kernel de fun��es exportadas
	//e tabela de ponteiros para tabelas dos outros programas em kernel mode.
    //Status = (int) executive_config_exported_functions();

	//Continua ...
	
Done:
    
	//@todo: Checar a validade de 'Initialization' ??

    Initialization.executive = 1;	
	printf("Done!\n");	
	return (int) Status;
};


/*
int executiveInit()
{};
*/

//
// End.
//

