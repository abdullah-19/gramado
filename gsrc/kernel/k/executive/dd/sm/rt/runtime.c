/*
 * File: rt\runtime.c
 *
 * Descri��o:
 *     Arquivo principal do m�dulo '/rt' do kernel base.
 *     Interfaces para rotinas de runtime do kernel.
 *
 * Objetivo:
 *    Fazer a inicializa��o do mecanismo de aloca��o din�mica, incluindo 
 * configura��es de heaps, stacks, mmblocks ... 
 *
 * Hist�rico:
 *    Vers�o 1.0, 2015 - Created.
 *    Vers�o 1.0, 2016 - Aprimoramento geral das rotinas b�sicas.
 *    ...
 */


#include <kernel.h>


//Vari�veis internas.
//int runtimeStatus;
//...


/*
 * init_runtime:
 *     Initializing run time. 
 *     Kernel heap and stack stuffs.
 *     @todo: Mudar para runtimeInit().
 */
int init_runtime()
{
    //	
	// Init Memory Manager:	
	// Heap, Stack, Pages, mmblocks, memory sizes, memory zones ...
	//
	init_mm();
	
	
	//
	// Nessa hora a mem�ria j� funciona e o malloc tambem. e mensagens.
	//
	// o video esta usando ainda as configura��es de buffer e lfb faitas pelo boot loader.
	//
	
	//
	//@todo: Suspensa a configura��es de p�ginas
	//       por enquanto fica tudo como foi configurado pelo bootloader.
	//
	
	
	//Cria o diret�rio de p�ginas do processo kernel, page tables e 
	//area para pageframes de mem�ria paginada.
	SetUpPaging();		
	
    // Continua ...
	
done:
    g_module_runtime_initialized = 1;
    return (int) 0;    
};


/*
 * KiInitRuntime: 
 *     Init runtime.
 *     +Clear bss segment.
 *     +Init heap, malloc.
 *     +...
 *     @todo: 
 *         Criar aqui, apenas a interface que chama a rotina 
 *         de inicializa��o da runtime.
 *
 */
int KiInitRuntime()
{
    //@todo: ... interface.
	
    return (int) init_runtime();
};


/*
int runtimeInit(){
	;
};
*/


//
// End.
//
