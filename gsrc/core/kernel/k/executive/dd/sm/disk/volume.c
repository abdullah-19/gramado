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



int volumeShowVolumeInfo( int descriptor )
{
    if( descriptor < 0 || descriptor > VOLUME_COUNT_MAX ){
		goto fail;
	}	
	
	struct volume_d *v;
	
	v = (struct volume_d *) volumeList[0];
	
	if( (void *) v == NULL )
	{
		
	}else{
		
		if( v->used != 1 || v->magic == 1234 ){
			goto fail;
		}
		
		printf("volumeType={&d}\n",v->volumeType);
		printf("name={%s}",v->name);
		//printf("");
		//printf("");
		//printf("");
		//printf("");
		//printf("");
		//printf("");
		//printf("");
		
	};
	
	
done:
	return (int) 0;
fail:
    return (int) 1;
};

/*
 ***************************************************
 * volume_init:
 *     Inicializa o volume manager.
 */
//int volumeInit()
int volume_init()
{
	
#ifdef KERNEL_VERBOSE
    printf("volume_init: Initializing..\n");
#endif

    //
	// Inicializando a estrutura do volume 0,
	// que ser� o VFS.
	// Esse volume ser� do tipo buffer, ent�o n�o ser� 
	// carregado de nenhum disco.
	//

	
	// Volume info
	volumeinfo_conductor = (void*) malloc( sizeof(struct volumeinfo_d) );
	
	if( (void*) volumeinfo_conductor == NULL )
	{
	    printf("sm-disk-volume_init:");
		die();
	}else{
		

	    volumeinfo_conductor->id = 0;
	    
		volumeinfo_conductor->used = 1;
	    volumeinfo_conductor->magic = 1234;
	    
		volumeinfo_conductor->name = "VOLUME INFO";    		
		//...
	};
	
	
	// Volume.
	volume_conductor = (void*) malloc( sizeof(struct volume_d) );
	
	if( (void*) volume_conductor == NULL )
	{
	    printf("sm-disk-volume_init:");
		die();
	}else{
		
		//@todo:
		//volume_conductor->objectType = ?;
        //volume_conductor->objectClass = ?;
		
		
		// Ser� usado pelo VFS.
		volume_conductor->volumeType = VOLUME_TYPE_BUFFER;
		
	    volume_conductor->id = 0;
	    
		volume_conductor->used = 1;
	    volume_conductor->magic = 1234;
	    
		volume_conductor->name = "VOLUME 0";  

        volume_conductor->volume_info = (struct volumeinfo_d *) volumeinfo_conductor; 

        volumeList[0] = (unsigned long) volume_conductor; 		
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


