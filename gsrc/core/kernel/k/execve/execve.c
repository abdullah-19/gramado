/*
 * Gramado Executive - The main file for the Executive module in the 
 * kernel base. 
 * (c) Copyright 2015-2016 Fred Nora.
 *
 * File: k\executive.c 
 * 
 */
 
 
#include <kernel.h>
//#include "eidata.h"  //Executive Internal Data.

//unsigned char  EcecutiveName[] = "EXECUTIVE LAYER";


//rotina interna de support.
//isso deve ir para bibliotecas depois.
//n�o tem prot�tipo ainda.
// Credits: Luiz Felipe
/*
void executive_fntos(char *name)
{
    int  i, ns = 0;
    char ext[4];
    //const char ext[4];
	
    //transforma em mai�scula
	while(*name && *name != '.')
	{
        if(*name >= 'a' && *name <= 'z')
            *name -= 0x20;

        name++;
        ns++;
    }

    // aqui name[0] � o ponto.
	// ent�o constroi a extens�o.
	for(i=0; i < 3 && name[i+1]; i++)
	{
        if(name[i+1] >= 'a' && name[i+1] <= 'z')
            name[i+1] -= 0x20;

        ext[i] = name[i+1];
    }

    while(ns < 8){
        *name++ = ' ';
        ns++;
    }

    for(i=0; i < 3; i++)
        *name++ = ext[i];

    *name = '\0';
};
*/


/*
 *****************************************************************
 * executive_gramado_core_init_execve:
 *     Executa um programa no processo INIT 
 * dentro do ambiente Gramado Core. 
 * #obs: Isso funcionou.
 *
 * IN:
 * servi�o, file name, arg, env.
 *
 */
int 
executive_gramado_core_init_execve( int i,              
                                    const char *arg1,   
                                    const char *arg2,   
                                    const char *arg3 )  
{
	int Status = 1;  //fail.
	
	//Esse � o primeiro argumento.
	int Plane;
	
	char *s;
	struct thread_d *Thread;

	
	//#debug
	//printf("0=%s ",&argv[0]);
    //printf("1=%s ",&argv[1]);
    

	//
	// Testando carregar um programa para 
	// rodar no processo INIT, usando a thread 
	// prim�ria do processo !
	//
	
		
	//origem e destino.
	unsigned char *src = (unsigned char *) arg2; 
	unsigned char *pipe = (unsigned char *) pipe_gramadocore_init_execve->_base; 
	
    
	//?? funcionou ??
	//pois o base era um char*
	//mem�ria compartilhada entre o kernel e o aplicativo.
	//o aplicativo vai ler esse trem
	//unsigned char* shared_memory = (unsigned char*) (0x401000-0x100); 
	unsigned char* shared_memory = (unsigned char*) (0xC0800000 -0x100);

    //#IMPORTANTE:
    //PRECISAMOS ENVIAR A MENSAGEM SOMENTE DEPOIS QUE 
    //O NOVO APLICATIVO FOR COLOCADO NA MEM�RIA
    //SEN�O AO COLOCAR O APLICATIVO NA MEM�RIA A MENSAGEM 
    //SER� SOBRESCRITA.	

	
	
	//#debug
	//printf("\nexecutive_gramado_core_init_execve: testing ...\n\n");
	//printf(">>>fileneme={%s}\n",arg1);
	//printf(">>>arg={%s}\n",arg2);
	//printf(">>>env={%s}\n\n",arg3);
	
	
	//...
	
	
	//## teste
	//
	//if( ! strcmp( (char*)argv[0], "-f" ) ) 
	//if( strncmp( (char*) &argv[1], "-f", 2 ) == 0 )
	//{
	//	printf("executive_gramado_core_init_execve: FOREGROUND\n");
    //    Plane = FOREGROUND;
    //}else{
	//	printf("executive_gramado_core_init_execve: BACKGROUND\n");
	//	Plane = BACKGROUND;
	//};
	
	//fail.
	//if( (const char *) filename == NULL ){
	//	return 1;
	//}
	
	//
	// ENVIANDO A MENSAGEM
	//
	
	
	for( i=0; i<512; i++ )
	{
        pipe[i] = src[i];
		shared_memory[i] = src[i];
	};
	
 
	//#debug
	//ok. isso funcionou.
	//printf("Showpipe={%s}\n",pipe);
	//printf("Showsharedmemory={%s}\n",shared_memory);	 
	
	
	//
	// Pegar o ponteiro da thread prim�ria do processo 
	// INIT.
	//
	
	Thread = (struct thread_d *) threadList[0];
	if( (void*) Thread == NULL )
	{
		printf("executive_gramado_core_init_execve: Thread\n");
		goto fail;
	}else{
		
		if( Thread->used != 1 || 
		    Thread->magic != 1234 )
		{
			printf("executive_gramado_core_init_execve: used magic\n");
			goto fail;
		}
		
		// Significa que o contexto nunca foi salvo ...
		// isso � importante, pois o spawn n�o funciona em thread 
		// com o contexto salvo.
		Thread->saved = 0; 
		
		Thread->plane = Plane;
		
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
 		
		
		//#bugbug
		//string lenght
		//devemos ver se a string n�o passa dos limites.
		//Como essa rotina � para executar um arquivo .bin,
		//caso n�o exista uma extens�o .bin e o nome seja 
		//menor que 8 podemos adicionar a extens�o .bin.
		
		size_t l;
		l = (size_t) strlen( (char*) arg1);
		
		if( l > 11 )
		{
		    printf("executive_gramado_core_init_execve: File too long!\n");	
			//Obs: N�o sairemos da fun��o pois isso � um teste ainda.
		}else{
			
			
			 
			//se n�o existe um ponto entre os oito primeiros chars,
            //ent�o colocamos a extens�o .bin logo ap�s o nome passado.
            //e ele � pelo menos menor que 11, mas deveria ser menor que oito.			
			if( arg1[0] != '.' || 
			    arg1[1] != '.' || 
                arg1[2] != '.' || 
                arg1[3] != '.' || 
                arg1[4] != '.' || 
                arg1[5] != '.' || 
                arg1[6] != '.' || 
                arg1[7] != '.' )
				{ 
				    l = (size_t) strlen( (char*) arg1);
                    if( l > 8 )
					{
						printf("executive_gramado_core_init_execve: File without ext is too long!\n");
					    //Obs: N�o sairemos da fun��o pois isso � um teste ainda.
					}
					
					char bin_string[] = ".bin";
		            strcat( (char *)arg1, (const char *) bin_string );
			    };
			 
		};
		
		//
		// #bugbug
		// #importante Precisamos do ponteiro v�lido para filename.
		// N�o podemos auter�-lo e depois us�-lo.
		//
		
		//#importante: Isso precisa ser nesse momento e n�o antes,
		//pois pode corromper o espa�o destinado aos argumentos 
		//dentro do vetor ao acrescentar zeros.
		read_fntos( (char *) arg1);
		
		//fs/read.c
	    // "FILE    BIN"
        Status = (int) fsLoadFile( (unsigned char *) arg1, 
		                           (unsigned long) 0x00400000 );

        //fail
		if( Status == 1 )
		{
			// @todo:
			// Configurar estrutura.
			printf("executive_gramado_core_init_execve: Status\n");
			printf("Can't load file.\n");
			goto fail;
		};
		
		// Se deu certo.
		// Conseguimos carregar o arquivos.
		// Devemos checar a validade do arquivo na mem�ria.
		if( Status == 0 )
		{
			//checar
			//#bugbug: N�o deve existir suporte a PE dentro do kernel.
			//PE � propriet�rio.
			Status = (int) fsCheckPEFile( (unsigned long) 0x00400000 );
			if( Status == 0 )
			{
				printf("executive_gramado_core_init_execve: Status\n");
			    printf("It's not a valid PE file.\n");
			}
			
		    queue_insert_data( queue, 
			    (unsigned long) Thread, QUEUE_INITIALIZED );
            
			// * MOVEMENT 1 ( Initialized ---> Standby ).
			SelectForExecution(Thread);    
            goto done; 
        };	
         
        //fail		 
	};
	
	//fail
	
fail:
    printf("fail\n");
done:
	
	//refresh_screen();
	//while(1){
	//	asm("hlt");
	//}
	//	
    
	//#debug
	//printf("done\n");	
	
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
 *************************************
 * sys_showkernelinfo:
 *     Show kernel info.
 */
void 
sys_showkernelinfo()
{
	KiInformation();
};



/*
 ************************************************
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

