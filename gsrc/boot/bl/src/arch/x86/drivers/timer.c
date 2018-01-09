/*
 * File: timer.c 
 *
 * Descri��o:
 *     Handler da irq0 para o Boot Loader.
 *
 * ATEN��O:
 *    O timer no Boot Loader n�o faz task switch.
 *    O timer ser� usado principalmente para timeout de driver.
 *    O timer pode ser usado para contar tempo de inicializa��o.
 *    
 * Vers�o 1.0, 2015.
 */

 
#include <bootloader.h>


unsigned long timerTicks;


/*
 * timer:
 *     Timer handler for Boot Loader.
 */
void timer()
{
    timerTicks++;    //Incrementa o contador.
	
    if(timerTicks % 100 == 0){		
        //De tempos em tempos.
    };
    
    //Nothing for now.
	
done:
    return;      
};


/*
 * BltimerInit:
 *     Inicializa o m�dulo timer.
 */
int BltimerInit()
{
    int Status = 0;
	
	timerTicks = 0;
    
	//...
	
done:	
	return (int) Status;	
};

//
//Fim.
//

