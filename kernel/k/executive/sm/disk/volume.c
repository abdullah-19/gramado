/*
 * File: volume.c 
 *
 * Descri��o:
 *     Gerenciador de volumes.
 *     Arquivo principal do Volume Manager.
 *     MB - M�dulos inclu�dos no Kernel Base.
 *
 *     Obs: O Gerenciador de Volumes um m�dulo muito importante do Kernel.
 *
 * Vers�o 1.0, 2015, 2016.
 */


#include <kernel.h>


/*
 * volume_init:
 *     Inicializa o volume manager.
 */
int volume_init()
{
    printf("volume_init: Initializing..\n");	

	// Structure.
    volumeinfo = (void*) malloc( sizeof(struct volumeinfo_d) );
	if( (void*) volumeinfo == NULL ){
	    printf("volume_init:");
		refresh_screen();
		while(1){}
	}else{
		
	    //
	    // Bom, como aqui j� temos uma estrutura v�lida,
	    // ent�o podemos complet�-la com par�metros padr�es.
	    //
	
	    volumeinfo->volumeId = 0;
	    volumeinfo->volumeUsed = 0;
	    volumeinfo->volumeMagic = 0;
	    //volumeinfo->volumeName = NULL;    //Pointer.		
		//...
	};
	
	//
	// @todo: More ??!
	//

done:	
    return (int) 0;
};


/*
int init_volume_manager();
int init_volume_manager()
{
	volume_init();
    return (int) 0;
};
*/


/*
int volumeInit(){
	;
};
*/


//
//fim.
//


