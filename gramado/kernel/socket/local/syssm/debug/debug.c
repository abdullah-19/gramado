/*
 * File: debug.c
 *
 * Modulo Debug.
 * MB - M�dulos inclu�dos no Kernel Base.
 *	
 * Descri��o:
 *     Kernel Debugger.
 *     Arquivo princial do m�dulo debug do executive do kernel.
 *     Checar se h� falhas no sistema. 
 *     Como um dr watson.
 * 
 * Obs:
 *     As fun��es aqui s�o usadas pelo kernel.
 *     Mas pode haver um aplicativo que utilizem essas fun��es.
 *
 * Obs:
 *     @todo: Come�ar a an�lise pela estrutura 'platform' e todo
 * o que est� dentro dela.
 *
 *
 * 2015 - Created by Fred Nora.
 *
 */

 
#include <kernel.h>


// Vari�veis internas.
//int debugStatus;
//int debugError;
//...




/*
 * debug_check_inicialization:
 *
 *     Checar se o kernel e os m�dulos foram inicializados.
 *     Checa o valor das flags.
 *     checar todos contextos de tarefas v�lidas.
 */

int debug_check_inicialization (void){

    int Status = 0;

    // Check phase.

    if ( KeInitPhase != 3 ){
       Status = 1;
       printf ("debug_check_inicialization: KeInitPhase phase={%d}\n",
           KeInitPhase );
       goto fail;
    }




    // Executive.
    if ( Initialization.executive != 1 ){
        Status = 1;
        printf ("debug_check_inicialization: executive\n");
        goto fail;
    }


    // Microkernel.
    if ( Initialization.microkernel != 1 ){
       Status = 1;
       printf ("debug_check_inicialization: microkernel\n");
       goto fail;
    }


    // Hal.
    if ( Initialization.hal != 1 ){
        Status = 1;
        printf ("debug_check_inicialization: hal\n");
        goto fail;
    }


    // More?!

//done:
    return (int) Status;

fail:
    die (); 
}




/*
 * debug_check_drivers:
 *    Checar se os drivers est�o inicializados.
 */
 
int debug_check_drivers (void){

    int Status = 0;


	if (g_driver_hdd_initialized != 1){
	    //erro
	}

	if (g_driver_keyboard_initialized != 1){
	    //erro
	}
	
    if (g_driver_pci_initialized != 1){
	    //erro
	}
	
    if (g_driver_rtc_initialized != 1){
	    //erro
	}
	
    if (g_driver_timer_initialized != 1){
	    //erro
	}


    return (int) Status;
}


/*
 ********************************************
 * debug_breakpoint:
 *     Para a execu��o do sistema.
 *     @todo: isso pode ir para o arquivo debug.c.
 */

void debug_breakpoint (void){

    printf ("debug_breakpoint:\n");
    die ();
}


//retorna um checksum dado um buffer e um tamanho.

unsigned long 
debug_compute_checksum ( unsigned char *Buffer, 
                         unsigned long Lenght )
{
    unsigned long CheckSum = 0;

    while (Lenght > 0)
	{	
		CheckSum = CheckSum + (unsigned long) *Buffer++;
	    
		Lenght--;	
	};

	return (unsigned long) CheckSum;
}



/*
 ****************************************
 * debug:
 *     Checa por falhas depois de cumpridas as 
 *     tr�s fases de inicializa��o.
 */

// #bugbug
// Ser� que o output est� realmente dispon�vel nesse momento ?!

int debug (void){

    int Status = -1; 

    // Checa inicializa��o. 
    // Fases, vari�veis e estruturas.

    Status = (int) debug_check_inicialization ();

    if (Status == 1){
        panic ("debug: debug_check_inicialization fail\n");
    }


    // 'processor' struct.

    if ( (void *) processor == NULL ){
        panic ("debug: processor struct fail\n");
    }

    // Check drivers status. 
    // ( Ver se os principais drivers est�o inicializados )

    debug_check_drivers ();


	/*
	 * @todo: 
	 *     Checar se existe componentes do sistema como mbr, root, fat 
	 * e arquivos e programas b�sicos do sistema.
	 */
	 
	 
	/* 
	 * @todo: 
	 *     Checar por falhas no sistema de arquivos.
	 */
	 
	 
	/*
     * @todo:	
	 *     Checar por falhas nas estruturas de tarefas.
	 */



	//...


    // printf ("debug: Done.\n");

    return 0; 
}



/*
 **********************************
 * debug_print:
 *     Serial debug support.
 * 
 */

void debug_print ( char *data ){

    uint32_t i;

    for ( i=0; data[i] != '\0'; i++ ){
        serial_write_char (data[i]);
    }
}



//
// End.
//

