/*
 * File: execve.c 
 * 
 * Gramado Executive - The main file for the Executive module in the 
 * kernel base. 
 * 
 * (c) Copyright 2015-2019 Fred Nora.
 */


// #todo
// Rotinas de execve podem ir para sysmk.


 
#include <kernel.h>
//#include "eidata.h"  //Executive Internal Data.




/*
 *******************************************
 * sys_execve:
 *     Efetua o servi�o execve, rodando um novo programa no 
 * processo atual.
 *     T� usando a thread atual e transformando ela em 
 * thread de controle.
 * 
 *     #todo: 
 *     Melhorar a descri��o do objetivo dessa rotina.
 * 
 *     args:  ?(servi�o), name, arg, env.
 */
 
// #Aten��o
// Isso foi chamado depois de um fork(), nesse caso funciona as vezes 
// e em outra falha. 


// IN: ?(servi�o), name, arg, env.
int 
sys_execve ( int i, 
            const char *arg1, 
            const char *arg2, 
            const char *arg3 ) 
{

   //#todo
   //Checar validades ante de efetuar o execve.
   
    //if(i<0)
        //return -1;
 
 
    return (int) process_execve ( (int) i, (const char *) arg1, (const char *) arg2, (const char *) arg3 );
}





/*
 ************************************************
 * init_executive:
 *
 *     Initialize the kernel executive.
 *     Archtecture (independent) inicialization. 
 */
 
int init_executive (void){

    int Status = 0;


#ifdef KERNEL_VERBOSE
    printf ("EXECUTIVE:\n");
#endif


    debug_print ("init_executive:\n");


	// PCI - Pega informa��es da PCI.
    init_pci ();


	// CLOCK - Pega informa��es de Hora e Data.
    init_clock ();
    get_cmos_info ();

	//...


	// #importante: 
	// S� depois de inicializarmos o ata 'e que podemos carregar 
	// alguma coisa.

    debug_print ("init_executive: diskATADialog\n");
    diskATADialog ( 1, FORCEPIO, FORCEPIO );

	// ??
	// configura a tabela do kernel de fun��es exportadas
	// e tabela de ponteiros para tabelas dos outros programas em 
	// kernel mode.
    // Status = (int) executive_config_exported_functions();

	//Continua ...
    
	// #todo: 
	// Checar a validade de 'Initialization' ??

	//#debug
	//printf("Done!\n");

    Initialization.executive = 1;

    return (int) Status;
}


//
// End.
//

