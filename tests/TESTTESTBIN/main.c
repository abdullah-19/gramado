/*
 * File: main.c 
 *
 * Descri��o: 
 *     Arquivo principal de IDLE.BIN.
 *     IDLE.BIN � um processo com apenas uma thread.
 *     IDLE.BIN � um aplicativo de 32bit em user mode. 
 *
 *     Esse programa dever� ser chamado sempre que o sistema estiver ocioso,
 * ou com falta de op��es vi�veis. Ent�o esse programa deve ficar respons�vel 
 * por alguma rotina de manuten��o do equil�brio de sitema, ou por ger�ncia de 
 * energia, com o objetivo de poupar energia nesse momento de ociosidade.
 *
 * O processo idle pode solicitar que processos de gerencia de energia entrem em
 * atua��o. Pois a chamada do processo idle em si j� � um indicativo de ociosidade
 * do sistema. Pode-se tamb�m organizar bancos de dados, registros, mem�ria, buffer,
 * cache etc.
 *
 *     O sistema pode configurar o que esse processo faz quando a m�quina 
 * est� em idle mode. Quando n�o h� nenhum processo pra rodar ou a cpu se 
 * encontra ociosa, pode-se usar alguma rotina otimizada presente neste 
 * programa. Parece que a intel oferece sujest�es pra esse caso, n�o sei.
 * 
 * Obs: O entry point est� em head.s
 *      Agora idle pode fazer systemcalls. 
 *
 * @todo: Criar argumento de entrada.
 *
 * Hist�rico:
 *     Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o 1.0, 2016 - Revis�o.
 *     ...
 */
 
 
//
// Includes.
// 
 
#include "api.h"      //api.
#include "stdio.h"    //libC em user mode.
#include "stddef.h"   //libC em user mode. 


//
// Vari�veis internas.
//

//Idle application support.
int idleStatus;
int idleError;
//...

//Idle driver support.
int driverInitialized;
//...


/*
struct idle
{
	struct thread_d *current_idle_thread;
	//int
}
*/

//
// Prot�tipos.
//


void idleLoop();
void driverInitialize();      // processo sendo considerado um driver servidor.
void driverUninitialize();    // desinicializa.
int idleInit();               // processo sendo consideredo cliente.
unsigned long idleServices(unsigned long number);  //Principal.
//...




//Another loop.
void idleLoop()
{
    while(1){
		asm ("pause");
	}	
};

 




/*
 * driverInitialize:
 *     O Kernel solicitou a rotina de inicializa��o do processo Idle na forma 
 * de driver. Faremos uma chamada ao Kernel dizendo que o driver est� 
 * inicializado. Na chamada passaremos alguns par�metros, como: O n�mero do 
 * servi�o, 129 e o c�digo de confirma��o, 1234. 4321.
 *
 */
void driverInitialize()
{
	//printf("Idle: Initializing driver ...\n");
	//refresh_screen();

	driverInitialized = 1;	
	//system_call( 129, 4321, 4321, 4321 );	
	
done:	
	printf("IDLE.BIN: Initialized.\n");
	refresh_screen();
	return;
};


/*
 * driverUninitialize:
 *     This method is called to uninitialize the driver object. In a real 
 * driver, this is where the driver would clean up any resources held by 
 * this driver. (m$)
 */
void driverUninitialize()
{
	//
	// D�vidas??
	// Devemos nos preparar para desinicializar o driver.
	// Desalocaremos os recursos usadados pelo driver, como mem�ria, 
	// arquivos abertos ...
	// Obs: N�o sei se atribui��o do pr�prio driver liberar os recursos.
	// Mas de toda forma, no m�nimo devemos avisar o Kernel que os recursos 
	// n�o s�o mais necess�rios.
	// @todo: Podemos enviar um comando para o kernel pedindo pra que ele n�o 
	// link esse driver ao sistema, ou que deixe ele n�o inicializado.
	//

	//printf("Idle: Unitializing driver ...\n");
	//refresh_screen();

	driverInitialized = 0;	
	
	//Faremos uma chamada para o Kernel 'deslinkar' o driver.
	//talvez 128. 127 126..???
	//system_call( ?? , 4321, 4321, 4321 ); 		
	
done:	
	printf("IDLE.BIN: Uninitialized.\n");
	refresh_screen();
	return;
};


/*
 * idleInit:
 *     Inicializando a aplica��o Idle.
 */
int idleInit()
{
	idleStatus = 0;
	idleError = 0;
	
	//printf("Idle: Initializing idle application ..\n");
	//refresh_screen();
	//...
	return (int) 0;
};


/*
 *****************************************************************************
 * idleServices:
 *     Essa fun��o oferece servi�os de acordo com o n�mero passado via 
 * argumento.
 * Essa deve ficar por �ltimo e ter acesso � qualquer rotina acima ou em
 * bibliotecas inclu�das.
 */
unsigned long idleServices(unsigned long number)
{
    // Checar se o driver est� inicializado.
	if(driverInitialized != 1){
		return (unsigned long) 1;    //erro
	}
	
	
	//
	// Selecionar o servi�o.
	//
	
    switch(number)
    {
		case 0:
		    printf("Idle Driver: NULL service.\n");
		    idleLoop();
			break;
			
		case 1:
		    printf("Idle Driver: service 1.\n");
		    //idleLoop();
			break;

        //...			
		
		default:
		    printf("Idle Driver: default service.\n");        
			//idleLoop();
			break;
	};	
	
	
done:
    refresh_screen(); 
    return (unsigned long) 0;	
};


/*
 ************************
 * testtest_main:
 */
int testtest_main (){
	
	printf("testtest_main:\n");
	printf("Initilizing TESTTEST.BIN ...");
	printf("DONE.");	
	refresh_screen();
	
	return 0;
};

//
// End.
//

