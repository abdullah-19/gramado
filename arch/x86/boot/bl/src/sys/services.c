/*
 * File: services.c 
 * 
 * Descri��o:
 *     Alguns servi�os internos oferecidos pelo Boot Loader.
 *     Obs: 
 *     O Boot Loader deve oferecer servi�os, funcionando como
 * m�dulo do kernel em kernel mode.
 *
 * Vers�o 1.0, 2015.
 */

 
#include <bootloader.h>


void *blServices ( unsigned long arg1, 
                   unsigned long arg2,
				   unsigned long arg3,
				   unsigned long arg4 )
{
	switch (arg1)
	{
		case 1:
		    break;
			
		case 2:
		    break;
		//...
	}
	return NULL;
}

/*
 * system_services:
 *     int 200 - Servi�os do Boot Loader.
 */
void system_services (){
	
    //Nothing.
	return;   
};

//
// fim.
//

