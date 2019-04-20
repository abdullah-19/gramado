/*
 * Gramado Executive - The main file for the Executive module in the 
 * kernel base. 
 * (c) Copyright 2015-2016 Fred Nora.
 *
 * File: execve\execve.c 
 * 
 */
 
 
#include <kernel.h>
//#include "eidata.h"  //Executive Internal Data.

//unsigned char  EcecutiveName[] = "EXECUTIVE LAYER";


/*
 *****************************************************************
 * executive_gramado_core_init_execve:
 *
 *     Executa um programa no processo INIT dentro do ambiente Gramado Core. 
 *     Ou seja, na thread prim�ria do processo INIT.
 *     Ou seja, a aplica��o tem que ser compilada no mesmo endere�o do INIT.
 *     #obs: Isso funcionou.
 *
 * #bugbug:
 * E se o aplicativo fechar a thread, como usaremos a thread do processo init? 
 * Talvez a solu��o para isso seria criar outra. 
 * Nos casos em que o aplicativo chama o exit, n�o est� retornando para o 
 * shell. O fato � que quando chama o exit o estado da thread muda.
 *
 * IN:
 * servi�o, file name, arg, env.
 */
 
int 
executive_gramado_core_init_execve ( int i,              
                                     const char *arg1,    //name
                                     const char *arg2,    //arg
                                     const char *arg3 )   //env
{
	int Status = 1;  //fail.
	
	//??
	//Esse � o primeiro argumento.
	int Plane = 0;
	
	char *s;
	struct thread_d *Thread;

	
	//Usados gerenciamento de arquivo.
	size_t l; //lenght.
 	char bin_string[] = ".bin";	
	//char xxx_string[] = ".xxx";	
	
	//#debug
	//printf("0=%s ",&argv[0]);
    //printf("1=%s ",&argv[1]);
    

	// #importante
	// Testando carregar um programa para rodar no processo INIT, 
	// usando a thread prim�ria do processo !
	// � o mesmo que consierar que o processo INIT j� seja o clone 
	// de outro v�lido.
		
    //??		
	//array de ponteiros.
	unsigned long *p = (unsigned long *) arg2;
    
	// #importante:
	// Mem�ria compartilhada entre o kernel e o aplicativo.
	// O aplicativo vai ler esse trem 
	unsigned char *shared_memory = (unsigned char *) (0xC0800000 -0x100);
	
    // #IMPORTANTE:
    // PRECISAMOS ENVIAR A MENSAGEM SOMENTE DEPOIS QUE O NOVO PROGRAMA FOR 
	// COLOCADO NA MEM�RIA, SEN�O AO COLOCAR O PROGRAMA NA MEM�RIA A MENSAGEM 
    // SER� SOBRESCRITA.	
    // #TODO: CRIAR UM MECANISMO DE TROCA DE MENSAGENS MAIS EFICIENTE,
	// BASEADO NESSE.
	
    //=================================================
    //  ## CMD ##
    //	
		
	//#importante.
	//antes de tudo vamos testar o comando.
	//se ele n�o existir ent�o nem vamos mexer na estrutura da trhead.
	//se n�o mexermos na estrutura da thread ele continuar� presa no while 
	//do exit da libc.
	
	
		//
		// ## Load file ##
		//
		
		// #bugbug
		// # arg1=name ##
		
		// Devemos ver se a string n�o passa dos limites.
		
		// Como essa rotina � para executar um arquivo .bin,
		// caso n�o exista uma extens�o .bin e o nome seja menor que 8, 
		// podemos adicionar a extens�o .bin.
		
		l = (size_t) strlen ( (char *) arg1 );
		
		if ( l > 11 )
		{
		    // #fail 
			printf("executive_gramado_core_init_execve: File name fail\n");	
			//Obs: N�o sairemos da fun��o pois isso � um teste ainda.
			//goto fail;
		
		} else {
			
			
			//se n�o existe um ponto entre os oito primeiros chars,
            //ent�o colocamos a extens�o .bin logo ap�s o nome passado.
            //e ele � pelo menos menor que 11, mas deveria ser menor que oito.			
			
			if ( arg1[0] != '.' && 
			     arg1[1] != '.' && 
                 arg1[2] != '.' && 
                 arg1[3] != '.' && 
                 arg1[4] != '.' && 
                 arg1[5] != '.' && 
                 arg1[6] != '.' && 
                 arg1[7] != '.' )
				{
                    //#bugbug: J� pegamos esse valor.					
				    //l = (size_t) strlen ( (char *) arg1);
                    
					if ( l > 8 )
					{
						printf("executive_gramado_core_init_execve: File without ext is too long\n");
					    //Obs: N�o sairemos da fun��o pois isso � um teste ainda.
					    //goto fail;
					}
					
		            strcat ( (char *) arg1, (const char *) bin_string );
			    };
				
			// #obs:	
			// Se estamos aqui, isso significa existe um ponto 
            // nos primeiros oito bytes.
            // Ainda n�o sabemos se todo o nome do arquivo est� certo,
            // mas ja sabemos que n�o precisamos incluir uma exten��o.			
		};
		
	
	read_fntos ( (char *) arg1 );
		
    Status = (int) fsLoadFile ( VOLUME1_FAT_ADDRESS, 
					   VOLUME1_ROOTDIR_ADDRESS, 
	                   (unsigned char *) arg1, 
	                   (unsigned long) 0x00400000 );


	// Se n�o conseguimos carregar o arquivo, devemos abortar.
	if ( Status == 1 )
	{		
		printf("executive_gramado_core_init_execve: ERROR. Can't load file\n");
		goto fail;
	}
	
	//Ok
	//O comando existe e o arquivo foi carregado.
	//mas precisamos saber se a assinatura de PE � v�lida.
				
			// #bugbug: 
			// N�o deve existir suporte a PE dentro do kernel.
			// PE � propriet�rio.
			
	
	Status = (int) fsCheckELFFile ( (unsigned long) 0x00400000 );
	if ( Status == 0 )
	{
	    //printf("ok, its a valid ELF file\n");
		goto format_ok;
		
	}		
	if ( Status == 1 )
	{
		//a� ferrou ... n�o podemos continuar com um arquivo corrompido.	
        //como carregamos novo arquivo na mem�ria ent�o a antiga thread vai falaher.		
		printf("executive_gramado_core_init_execve: ERROR. It's not a valid ELF file\n");
		die();
        goto fail;
    }
	
	
	
	Status = (int) fsCheckPEFile ( (unsigned long) 0x00400000 );
			
	if ( Status == 1 )
	{
		//a� ferrou ... n�o podemos continuar com um arquivo corrompido.	
        //como carregamos novo arquivo na mem�ria ent�o a antiga thread vai falaher.		
		printf("executive_gramado_core_init_execve: ERROR. It's not a valid PE file\n");
		//die();
        //goto fail;
    }	

	
format_ok:	
	
	//#debug
	//tentando receber uma linha de ocmando inteira.
	//printf("\nexecutive_gramado_core_init_execve: testing..\n\n");
	
	//# ISSO DEU CERTO #
	//testando se o shell transferiu toda alinha de comandos para a mem�ria compartilhada.
	//printf(">>>cmdline2={%s}\n", shared_memory);
	
	
	//#IMPORTANTE:
	//se a linha de comandos est� na mem�ria compartilhada 
	//e o nome do arquivo programa foi passado via endere�o 
	//ent�o temos tudo o que � preciso 
	//para enviarmos a linha de comandos para o aplicativo.
	
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
	
	//unsigned longs.
	//for( i=0; i<512; i++ )
	//{
        
		//N�o queremos transferir o primeiro ponteiro 
		//pois ele � o nome do programa e n�o um argumento.
	//	shared_p[i] = p[i+1];
		
		//pipe[i] = src[i];
		//shared_memory[i] = src[i];
	//};
	
	//os ponteiros est�o na mem�ria compartilhada, 
	//mas as strings est�o onde ??
	//provavelmente as strings ainda esteja na mem�ria 
	//do shell, e o aplicativo n�o pode ler as strings que est�o 
	//na mem�ria do shell.
	//obs: o shell poderia copiar toda a linha de comando para 
	//a mem�ria compartilhada.
	//printf(">>>shared_p0={%s}\n"     ,shared_p[0]);
	//printf(">>>shared_p1={%s}\n"     ,shared_p[1]);
	//printf(">>>shared_p2={%s}\n\n"   ,shared_p[2]);
	//printf(">>>shared_p3={%s}\n\n"   ,shared_p[3]);	
	
 
	//#debug
	//ok. isso funcionou.
	//printf("Showpipe={%s}\n",pipe);
	//printf("Showsharedmemory={%s}\n",shared_memory);	 
	
	
	// Pegar o ponteiro da thread prim�ria do processo INIT.
    // o ponteiro vai continuar existindo mesmo que o deadthread collector 
	// tenha destruido a estrutura depois de um app mudar o estado para zombie 
	// por causa de um exit.
	//Ent�o � melhor criarmos uma thread nova. Mas se fizermos isso
	//sem antes o deadthread collector ter destruido a estrutua e liberado a mem�ria 
	//ent�o a antiga estrutura de thread ficar� sem porteiro e poderemos desalocar a mem�ria 
	//usanda, ou pior, se n�o mudarmos o status ele pode querer destruir a que estamos criando
	//ent�o s� criaremos se o ponteiro for NULL, significando que o deadthread collector 
	//ja destruiu a estrutura e aproveitou a mem�ria se conseguiu.
	
	//#importante:
	//Podemos eswperar que essa thread esteja no estado ZOMBIE.
	//Pois um aplicativo pode ter efetuado um exit.
	//se ela estiver no estado ZOMBIE
	
	//#importante
	//esperamos que tenha limpado IdleThread antes de usarmos o ponteiro.
	//isso � trabalho do exit e do deadthread collector.
	
	Thread = (struct thread_d *) IdleThread; 			
	
	if ( (void *) Thread == NULL )
	{
		//3imporante:
		//Vamos tornar isso um erro fatal por enquanto, para podermos refletir sobre 
		//esse assunto;
		printf("executive_gramado_core_init_execve: Thread Fail \n");
		die ();
		//goto fail;
		
	} else {
		
		//#importante:
		//Checar a flag de reaproveitamento.
		//Se a flag do reaproveitamento falhar ent�o o exit no kernel
		//n�o acionou ela para a threa InitThread, que � nossa �nica reaproveit�vel.
		//por enquanto.
		//if ( Thread->used != 1 || Thread->magic != 1234 )
		//{
		//	printf("executive_gramado_core_init_execve: Validation fail \n");
		//	die();
			//goto fail;
		//}
		
 
        //
		// ## state ##
		//
		
		//Vamos seguir a sequ�ncia de nacimento de um thread.
		//e cancelaremos caso n�o for poss�vel carregar o arquivo do 
		//programa.
		//INITIALIZED>STANDBY>READY>RUNNING ...
		
		Thread->state = INITIALIZED;
		
		// Significa que o contexto nunca foi salvo, pois o spawn 
		// n�o funciona em thread com o contexto salvo.
		
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
					
		queue_insert_data ( queue, 
		    (unsigned long) Thread, QUEUE_INITIALIZED );
            
			
		// #importante:
		// * MOVEMENT 1 ( Initialized ---> Standby ).
		SelectForExecution (Thread);    
            
		goto done; 
         
        //fail	
        //printf("executive_gramado_core_init_execve:  File support fail\n");
	};
	
	//fail	
fail:
    printf("executive_gramado_core_init_execve: #fail\n");
	//refresh_screen();
done:

    //#debug
	//printf(">>>shared_p0={%s}\n"     ,shared_p[0]);
	//printf(">>>shared_p1={%s}\n"     ,shared_p[1]);
	//printf(">>>shared_p2={%s}\n\n"   ,shared_p[2]);
	//printf(">>>shared_p3={%s}\n\n"   ,shared_p[3]);	
	//refresh_screen();
	//while(1){
	//	asm("hlt");
	//}
	//	
    
	
	//#debug
	//printf("done\n");	
	
	//
	// #Obs: 
	// +N�o devemos emitir mensagens no caso de acerto.
	// +refresh_screen s� no caso de erro.
	//
	
	//#bugbug
	//#obs: Estamos usando isso s� por enquanto para debug.
	refresh_screen();
	
	return (int) Status;	
};



//=================================================
//execve para .exe 

/*
 *****************************************************************
 * executive_gramado_core_init_execve:
 *
 *     Executa um programa no processo INIT dentro do ambiente Gramado Core. 
 *     Ou seja, na thread prim�ria do processo INIT.
 *     Ou seja, a aplica��o tem que ser compilada no mesmo endere�o do INIT.
 *     #obs: Isso funcionou.
 *
 * IN:
 * servi�o, file name, arg, env.
 */
 
int 
executive_gramado_core_init_execve_exe ( int i,              
                                         const char *arg1,   //name
                                         const char *arg2,    //arg
                                         const char *arg3 )   //env
{
	int Status = 1;  //fail.
	
	//??
	//Esse � o primeiro argumento.
	int Plane = 0;
	
	char *s;
	struct thread_d *Thread;

	
	//Usados gerenciamento de arquivo.
	size_t l; //lenght.
 	//char bin_string[] = ".bin";	
	//char xxx_string[] = ".xxx";	
	char exe_string[] = ".exe";
	
	//#debug
	//printf("0=%s ",&argv[0]);
    //printf("1=%s ",&argv[1]);
    

	// #importante
	// Testando carregar um programa para rodar no processo INIT, 
	// usando a thread prim�ria do processo !
	// � o mesmo que consierar que o processo INIT j� seja o clone 
	// de outro v�lido.
		
    //??		
	//array de ponteiros.
	unsigned long *p = (unsigned long *) arg2;
    
	// #importante:
	// Mem�ria compartilhada entre o kernel e o aplicativo.
	// O aplicativo vai ler esse trem 
	unsigned char *shared_memory = (unsigned char *) (0xC0800000 -0x100);
	
    // #IMPORTANTE:
    // PRECISAMOS ENVIAR A MENSAGEM SOMENTE DEPOIS QUE O NOVO PROGRAMA FOR 
	// COLOCADO NA MEM�RIA, SEN�O AO COLOCAR O PROGRAMA NA MEM�RIA A MENSAGEM 
    // SER� SOBRESCRITA.	
    // #TODO: CRIAR UM MECANISMO DE TROCA DE MENSAGENS MAIS EFICIENTE,
	// BASEADO NESSE.
	
	//#debug
	//tentando receber uma linha de ocmando inteira.
	//printf("\nexecutive_gramado_core_init_execve: testing..\n\n");
	
	//# ISSO DEU CERTO #
	//testando se o shell transferiu toda alinha de comandos para a mem�ria compartilhada.
	//printf(">>>cmdline2={%s}\n", shared_memory);
	
	
	//#IMPORTANTE:
	//se a linha de comandos est� na mem�ria compartilhada 
	//e o nome do arquivo programa foi passado via endere�o 
	//ent�o temos tudo o que � preciso 
	//para enviarmos a linha de comandos para o aplicativo.
	
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
	
	//unsigned longs.
	//for( i=0; i<512; i++ )
	//{
        
		//N�o queremos transferir o primeiro ponteiro 
		//pois ele � o nome do programa e n�o um argumento.
	//	shared_p[i] = p[i+1];
		
		//pipe[i] = src[i];
		//shared_memory[i] = src[i];
	//};
	
	//os ponteiros est�o na mem�ria compartilhada, 
	//mas as strings est�o onde ??
	//provavelmente as strings ainda esteja na mem�ria 
	//do shell, e o aplicativo n�o pode ler as strings que est�o 
	//na mem�ria do shell.
	//obs: o shell poderia copiar toda a linha de comando para 
	//a mem�ria compartilhada.
	//printf(">>>shared_p0={%s}\n"     ,shared_p[0]);
	//printf(">>>shared_p1={%s}\n"     ,shared_p[1]);
	//printf(">>>shared_p2={%s}\n\n"   ,shared_p[2]);
	//printf(">>>shared_p3={%s}\n\n"   ,shared_p[3]);	
	
 
	//#debug
	//ok. isso funcionou.
	//printf("Showpipe={%s}\n",pipe);
	//printf("Showsharedmemory={%s}\n",shared_memory);	 
	
	
	// Pegar o ponteiro da thread prim�ria do processo INIT.
	// #bugbug: Perceba que isso � uma constante.
	// ?? usar uma vari�vel, por exemplo:
	// init_thread ou dolly thread ou IdleThread->tid;
	
	//Thread = (struct thread_d *) threadList[0];
	
	Thread = (struct thread_d *) IdleThread; 
	
	if ( (void *) Thread == NULL )
	{
		//#fail
		printf("executive_gramado_core_init_execve: Thread Fail \n");
		goto fail;
		
	} else {
		
		if ( Thread->used != 1 || Thread->magic != 1234 )
		{
			printf("executive_gramado_core_init_execve: Validation fail \n");
			goto fail;
		}
		
		// Significa que o contexto nunca foi salvo, pois o spawn 
		// n�o funciona em thread com o contexto salvo.
		
		Thread->saved = 0; 
		
		Thread->plane = Plane;
		
	    //Context.
	    //@todo: Isso deve ser uma estrutura de contexto.
		
	    Thread->ss  = 0x23;                          //RING 3.
	    Thread->esp = (unsigned long) 0x0044FFF0;    //idleStack; (*** RING 3)
	    Thread->eflags = 0x3200;  //0x3202, pois o bit 1 � reservado e est� sempre ligado.
	    Thread->cs = 0x1B;                                
	    
		//#importante
		//eip, entrypoint 
		
		Thread->eip = (unsigned long) 0x00401000;   	
        //Thread->eip = (unsigned long) 0x00400400;   			
	    
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
		// ## Load file ##
		//
		
		// #bugbug
		// # arg1=name ##
		
		// Devemos ver se a string n�o passa dos limites.
		
		// Como essa rotina � para executar um arquivo .bin,
		// caso n�o exista uma extens�o .bin e o nome seja menor que 8, 
		// podemos adicionar a extens�o .bin.
		
		l = (size_t) strlen ( (char *) arg1 );
		
		if ( l > 11 )
		{
		    // #fail 
			printf("executive_gramado_core_init_execve: File name fail\n");	
			//Obs: N�o sairemos da fun��o pois isso � um teste ainda.
			//goto fail;
		
		} else {
			
			
			//se n�o existe um ponto entre os oito primeiros chars,
            //ent�o colocamos a extens�o .bin logo ap�s o nome passado.
            //e ele � pelo menos menor que 11, mas deveria ser menor que oito.			
			
			if ( arg1[0] != '.' && 
			     arg1[1] != '.' && 
                 arg1[2] != '.' && 
                 arg1[3] != '.' && 
                 arg1[4] != '.' && 
                 arg1[5] != '.' && 
                 arg1[6] != '.' && 
                 arg1[7] != '.' )
				{
                    //#bugbug: J� pegamos esse valor.					
				    //l = (size_t) strlen ( (char *) arg1);
                    
					if ( l > 8 )
					{
						printf("executive_gramado_core_init_execve: File without ext is too long\n");
					    //Obs: N�o sairemos da fun��o pois isso � um teste ainda.
					    //goto fail;
					}
					
		            //strcat ( (char *) arg1, (const char *) bin_string );
					strcat ( (char *) arg1, (const char *) exe_string );
			    };
				
			// #obs:	
			// Se estamos aqui, isso significa existe um ponto 
            // nos primeiros oito bytes.
            // Ainda n�o sabemos se todo o nome do arquivo est� certo,
            // mas ja sabemos que n�o precisamos incluir uma exten��o.			
		};
		
		
		// #bugbug
		// #importante: 
		// +Precisamos do ponteiro v�lido para filename.
		// +N�o podemos auter�-lo e depois us�-lo.
		
		// #importante: 
		// Isso precisa ser nesse momento e n�o antes, pois pode corromper o 
		// espa�o destinado aos argumentos dentro do vetor ao acrescentar 
		// zeros.
		
		// Isso est� em fs/read.c
	    // fsLoadFile usa o formado "FILE    BIN".
        // Vamos transformar 'file.txt' em "FILE    BIN". 		
		
		read_fntos ( (char *) arg1 );
		 
        Status = (int) fsLoadFile ( VOLUME1_FAT_ADDRESS,  
						   VOLUME1_ROOTDIR_ADDRESS, 
		                   (unsigned char *) arg1,  
						   (unsigned long) 0x00400C00 );
		
		// Se n�o conseguimos carregar o arquivo, devemos abortar.
		if ( Status == 1 )
		{
			// @todo:
			// Configurar estrutura.

			printf("executive_gramado_core_init_execve: ERROR. Can't load file\n");
			goto fail;
		};
		
		// Se conseguimos carregar o arquivo, devemos checar a validade 
		// do arquivo na mem�ria.
		if ( Status == 0 )
		{
			// #bugbug: 
			// N�o deve existir suporte a PE dentro do kernel.
			// PE � propriet�rio.
			
			//#bugbug 
			//ter�azmos que checar por (MZ) para .exe
			
			//Status = (int) fsCheckPEFile ( (unsigned long) 0x00400000 );
			//
			//if ( Status == 1 ){
			//	printf("executive_gramado_core_init_execve: ERROR. It's not a valid PE file\n");
			//	//goto fail;
			//}
			
			// ??
			// Colocar a trhead na fila de inicializadas.
			
		    queue_insert_data ( queue, 
			    (unsigned long) Thread, QUEUE_INITIALIZED );
            
			
			// #importante:
			// * MOVEMENT 1 ( Initialized ---> Standby ).
			SelectForExecution (Thread);    
            
			goto done; 
        };	
         
        //fail	
        printf("executive_gramado_core_init_execve:  File support fail\n");
	};
	
	//fail	
fail:
    printf("executive_gramado_core_init_execve: #fail\n");
	//refresh_screen();
done:

    //#debug
	//printf(">>>shared_p0={%s}\n"     ,shared_p[0]);
	//printf(">>>shared_p1={%s}\n"     ,shared_p[1]);
	//printf(">>>shared_p2={%s}\n\n"   ,shared_p[2]);
	//printf(">>>shared_p3={%s}\n\n"   ,shared_p[3]);	
	//refresh_screen();
	//while(1){
	//	asm("hlt");
	//}
	//	
    
	
	//#debug
	//printf("done\n");	
	
	//
	// #Obs: 
	// +N�o devemos emitir mensagens no caso de acerto.
	// +refresh_screen s� no caso de erro.
	//
	
	//#bugbug
	//#obs: Estamos usando isso s� por enquanto para debug.
	refresh_screen();
	
	return (int) Status;	
};


/* sys_showkernelinfo:
 *     Show kernel info. */

void sys_showkernelinfo (void){
	
	KiInformation ();
}


 
/*
 * sys_reboot:
 *     Reboot, Servi�o do sistema.
 *     Chamando uma rotina interna de reboot do sistema.
 */

void sys_reboot (void){

    hal_reboot ();
}


/*
 * sys_shutdown:
 *     Chama uma rotina interna para desligar a m�quina.
 */

void sys_shutdown (void){

    hal_shutdown ();
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
	
	//#debug
#ifdef KERNEL_VERBOSE
	printf("EXECUTIVE:\n");
#endif	

	
    debug_print("init_executive:\n");
	
	// PCI - Pega informa��es da PCI.
	// CLOCK - Pega informa��es de Hora e Data.	
	
	init_pci();
	
	init_clock();
	get_cmos_info();
	
	//...
	
	//#importante: 
	//so depois de inicializarmos o ata 'e que podemos carregar alguma coisa.
	
	debug_print ("init_executive: diskATADialog\n");
	diskATADialog ( 1, FORCEPIO, FORCEPIO );
	
	// ??
	// configura a tabela do kernel de fun��es exportadas
	// e tabela de ponteiros para tabelas dos outros programas em kernel mode.
    // Status = (int) executive_config_exported_functions();

	//Continua ...
    
	//@todo: Checar a validade de 'Initialization' ??

	//#debug
	//printf("Done!\n");
	
    Initialization.executive = 1;	
	
	return (int) Status;
}


//
// End.
//

