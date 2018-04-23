/*
 * File: sm\disk\volume.c 
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



void *volume_get_volume_handle( int number )
{
	//check limts
	if( number < 0 || number >= VOLUME_COUNT_MAX ){
		return NULL;
	}
	
	return (void *) volumeList[number];
};




void *volume_get_current_volume_info()
{
	return (void *) CurrentVolumeInfo; 
};



/*
 * volume_init:
 *     Inicializa o volume manager.
 */
//int volumeInit()
int volume_init()
{
    printf("volume_init: Initializing..\n");

	//
	// Inicializando uma estrutura global de informa��es sobre o 
	// volume do sistema.
	// ?? Em que arquivo est� essa estrutura.
	//	

	// Structure.
    volumeinfo = (void*) malloc( sizeof(struct volumeinfo_d) );
	if( (void*) volumeinfo == NULL )
	{
	    printf("sm-disk-volume_init:");
		die();
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


//
//fim.
//


