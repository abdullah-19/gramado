/*
 * File: main.c @todo: Criar a janela shell help.
 *
 * General purpose application.
 *     SHELL.BIN � um aplicativo de pr�posito geral. Desenvolvido como 
 * ferramenta do desenvolvedor para prover varios tipos de testes de recursos do sistema.
 *
 * Ok, isso � um program do tipo 'janela', o pequeno terminal 
 * que roda em uma janela filha ser� gerenciado pelo pr�prio aplicativo.
 * Isso � diferente de um programa feito para rodar em um terminal, onde o kernel 
 * gerenciar� a janela usada pelo programa. 
 *
 * Podemos usar esse terminal na janela filha para interpretar linguagem basic.
 *
 * Descri��o:
 * Shell do sistema. (SHELL.BIN)
 * Arquivo principal do Shell.
 * � um aplicativo de 32bit em user mode. 
 * P3 - Processos em ring3, User Mode.
 * O Programa recebe comandos do usu�rio e envia os comandos para o n�cleo 
 * atrav�s de chamadas ao sistema. 
 *
 * (@todo: envio de mensagens) A id�ia � que cada comando chame um processo 
 * diferente, mas existem servi�os internos que o pr�prio Shell ofere�e
 * fazendo uso dos recursos do Kernel ou pr�prios.
 *     	
 * � o console, um terminal virtual em user mode.    
 *
 * Obs: O Shell � o aplicativo apropriado para o desenvolvedor criar e testar 
 * as chamadas ao Kernel via interrup��o e para ver as mensagens do compilador.
 *      O programa que executa aplica��es em lote deve enviar os outputs para 
 * a tela do Shell. 
 *
 * Obs: O entry point est� em head.s
 *      @todo: N�o usar o arquivo head em assembly efeito de portabilidade.
 * Obs: O prompt e cursor est�o definidos em stdio.h
 *
 * * IMPORTANTE: O FOCO DO INTERPRETADOR DE COMANDOS DO SHELL APP DEVE SER
 * A GER�NCIA DE ARQUIVOS E DISPOSITIVOS DE ARMAZENAMENTO, EM SEGUNDO LUGAR
 * A GERENCIA DOS RECURSOS DO SISTEMA, POIS ISSO � ATRIBUI��O DO APP TASKMAN.
 *
 *
 * Hist�rico:
 *     Vers�o 1.0, 2016 -  Esse arquivo foi criado por Fred Nora.
 *     //...
 */

/*
 Como o kernel envia mensagens para o terminal em user mode?
 
 The mechanism for delivering messages to the console is implemented by the 
 printk function, defined in kernel/printk.c. 
 The function uses vsprintf (defined in lib/vsprintf.c) to create a message 
 string, places the string in the circular buffer of kernel messages and 
 passes it to all active console devices if the priority of the message is 
 less than console_loglevel. 
*/ 
 
 
//
// Includes.
// 

#include "api.h"
#include "types.h"
#include "stddef.h"   
#include "stdio.h"   
#include "stdlib.h"  
#include "string.h"
#include "shell.h"
#include "globals.h"




//#define DEFAULT_WINDOW_TITLE "Shell"

//
// Vari�veis internas.
//

int shellStatus;
int shellError;


//
// Prot�tipos para fun��es internas.
//


unsigned long shellProcedure( struct window_d *window, 
                              int msg, 
							  unsigned long long1, 
							  unsigned long long2 );
							  

 
 
 
/*
 *****************************************************************
 * GramadoMain: 
 *     Fun��o principal.
 *     The Application Entry Point.
 *
 * @todo:
 *    +Checar argumentos.
 *    +Criar a janela do TERMINAL. (Ok).
 *    +Enviar mensagem de texto para ele. (Ok).
 *    +Configurar procedimento de janela.
 *    +Definir a janela que � �rea do cliente.
 *    +Carregar um arquivo na �rea de cliente.
 *    +Testar outros recursos do Kernel.
 *    +Testar as chamadas para pegar inform��es sobre o proesso.
 *    +...
 *
 *  
 *  ## O SHELL � UM APLICATIVO DO TIPO JANELA DEVE TER UM MAIN DO TIPO JANELA ##
 *
 * Obs: Esses argumentos podem ser um padr�o.
 */
void *GramadoMain( int argc, char *argv[], unsigned long address, int view )
{
	
	//
	// Obs: Esse n�o � um programa que roda em modo terminal,
	// ele na verdade cria um terminal dentro de uma janela filha.
	// isso pode servir para esse programa interpretar linguagem basic por exemplo.
	// os programas em modo terminal n�o criar�o janelas e rodar�o nas 
	// janelas de terminal cridas para eles pelo kernel.
	//
	
	//#debug
	//deixe o kernel usar essa janela para teste.
	//Obs: N�o criaremos a janela principal desse programa 
	//para evitarmos erros com printf.
	
	/*
	 *Obs: Isso funcionou bem.
	apiBeginPaint();
	    MessageBox( 1, "SHELL.BIN","Initializing ...");
	apiEndPaint();
	
    refresh_screen();
	*/
	
	//chamando uma system call que ative a rotina de testes de 
	//escrita em janelas com o foco de entrada.
    //cancelada, acionaremos via procedimento de janela do sistema F12.
	//system_call( 222,0,0,0);
	//while(1){}	
	
	
	/*
	 * Uma boa ordem para os passos dessa rotina nos aplicativos  
	 * �: Step1=WindowClass, Step2=Window e Step3=Message.
	 */
	
	//@todo: Criar esse tipo de classe.
	//       Provavelmente deve coincidir com a estrutura presente
    //nas rotinas de gerenciamento de janela que est�o em kernel mode.	
	//struct window_class_d *wc; 
	
	
	struct window_d *hWindow;        //janela do aplicativo.

	
	//struct message_d *m;

	
	int Status = 0;
	int ShellFlag = 0;
	char *s;    //String	

	//...
	
	
	
	//
	// Isso configura alguns padr�es do aplicativo.
	// Os argumentos tratados abaixo podem modificar esses padr�es
	// Ex: Um argumento de entrada pode solicitar a troca de cor de fonte.
	//
	
	shellShell(); 	
	
	
	//#DEBUG
	//printf("S");
	//printf("Shell: testing strings \n");
	//shellSetCursor(8,8); 
	//printf("Shell: and cursor. \n");
	//refresh_screen();
	//while(1){}	
    
	
	//
	// Filtra a quantidade de argumentos.
	//
	
	// Se n�o h� argumentos.
	if(argc < 1){
		//#bugbug sprintf ( ?? o que deve ficar no argumentop ?? ) 
        //sprintf( stderr->_base,"Starting Shell with no arguments...\n");	 //Test.	
	    goto noArgs; 
	};
	
	//if (argc <= 1){
    //    fprintf( stderr, "%s\n", usage ); //File??
    //    exit(1);
    //}
	
	//
	// Filtra o primeiro argumento no formato 'palavra'.
	//
	
    //if (!strcmp( argv[1], "-help" ))
	//{
    //    printf( "%s\n", usage );
    //    refresh_screen();
	//	exit(0);
    //}
    
	//if (!strcmp( argv[1], "-version" ))
	//{
        //printf( "%s\n", shellGetVersion() );
    //    refresh_screen();
	//	exit(0);
    //}	
	
	//
	// Outras op��es de argumentos.
	//
	
	//Dependendo do argumento encontrado muda-se a flag.
    while(argc--) 
    {
        s = *++argv;
        if (*s == '-' || *s == '/') 
		{
            while(*++s) 
			{
                switch(*s) 
				{
                    case 'h':
                        //if (!strcmp( argv[argc], "help" )){ ... };
						//if (!strcmp( argv[argc], "HELP" )){ ... };
						//...
						ShellFlag = 0;
                        break;

                    case 'v':
					    //if (!strcmp( argv[argc], "version" )){ ... };
						//if (!strcmp( argv[argc], "VERSION" )){ ... };
						//...
                        ShellFlag = 0;
                        break;

                    case 'r':
					    //if (!strcmp( argv[argc], "XXX" )){ ... };
					    ShellFlag = 0;
                        break;

                    case 'f':
					    //if (!strcmp( argv[argc], "XXX" )){ ... };
						ShellFlag = 0;
                        break;

                    default:    
					    //usage();
						break;
                };
            };
        }
        else 
		{
			goto noArgs; 
            //usage();
        }
    };
	
	//Nothing.
	
noArgs:	
	

	
	//
	// @todo: Usar essa rotina para fazer testes de modo gr�fico.
	//	
	
	//Debug:
	//while(1){};
	
	//
	// @todo: 
	//     Set priority.
	//     Set Window Station, Desktop ...
	//     ...
	//
	// @todo:
	//     Quando essa janela � criada ainda estamos no ambiente de Logon,
	// dentro do desktop de Logon. N�o h� problema nisso por enquanto,
	// pois poderemos utilizar os recursos do shell antes de habilitarmos
	// todos os recursos o ambiente do usu�rio. Por�m ser� necess�rio, 
	// no futuro, executarmos o Shell no desktop do ambiente do usu�rio.
	//
	// @todo: 
	//     Precisamos de um ponteiro com a �rea de mem�ria que representa
	// a �rea de trabalho do Shell.
	//     Precisamos de um ponteiro com a �rea de cliente da janela do Shell.
	//
    // @todo: 
	//     Buffers. Buffers pra linha e buffer pra arquivo.	
	//
	// @todo: 
	//     Essa janela aparece centralizada e pequena, evidenciando que os 
	// argumentos n�o foram passados corretamente, ent�o o gerente de recursos 
	// gr�ficos usou dimens�es default. Provavelmente o Kernel n�o os 
	// recepciona devidamente ainda.
	//
	// ...
	//
	
	
	
	//
    // Create Window.
	//
	
	//Debug:
	//printf("\n\n Starting Shell Version ");
	//printf(SHELL_VERSION);
	//refresh_screen();	
	
	//Debug:
	//while(1){}
	
	

		
	
	
	
	//
	// Step 2. Creating a window frame.
	//
	
	
		
	
	//
    // N�o � necess�rio passar todos os argumentos de uma vez s�.
	// Podemos realizar 3 ou 4 chamadas para constru�rmos a janela.
	// Essa rotina tem 12 argumentos mas ela poder� realizar 3 chamadas
	// ao sistema para passar todos os argumentos.
	//
	
	
	//
	// *Importante:
	//      A janela do shell ser� uma aba dentro da janela do navegador,
	// essa janela do navegador � gerenciada pelo kernel, mas n�o passa de uma moldura 
	// com abas.
	// >> o kernel ja sabe que o processo tem uma aba, ent�o quando o processo 
	//tenta criar uma janela, a sua janela ser� criada dentro de sua aba.
	//
	
	//General purpose appplication  -  {} Developer version
	
	//@todo:
	//Criar essas fun��es na API.
	//unsigned long ScreenWidth = (unsigned long) APIGetScreenWidth();
	//unsigned long ScreenHeight = (unsigned long) APIGetScreenheight();
	
	
	//
	// Configurando o ret�ngulo que deve ser usado pelo terminal.
	//
	
	//#bugbug 
	// esses valores s�o usados para construir a janela princpal.
	// o que desejamos s�o os valores do ret�ngulo da �rea de cliente 
	// da janela principal.
	
	terminal_rect.left = shell_window_x;
	terminal_rect.top  = shell_window_y;
	terminal_rect.width = shellWindowWidth;
	terminal_rect.height = shellWindowHeight;
	
	apiBeginPaint();
	
	//hWindow = (void*) APICreateWindow( WT_EDITBOX, 1, 1," {} SHELL.BIN ",
	hWindow = (void*) APICreateWindow( WT_OVERLAPPED, 1, 1," {} SHELL.BIN ",
	                                   shell_window_x, shell_window_y, 
									   shellWindowWidth, shellWindowHeight,    
                                       0, 0, COLOR_BLACK, 0x83FCFF00 );	   
	if((void*) hWindow == NULL){	
		printf("Shell: Window fail");
		refresh_screen();
		while(1){}
		//exit(0);
	};
	

	
	/*
	 Imprimindo o ponteiro para a estrutura da janela criada 
	 Estamos testando se o retorno est� funcionando nesse caso.
	 */
	/* isso funcionou
	printf("Testing handle {%x}\n",hWindow);
	
	printf("Testing resize window\n");
	APIresize_window( hWindow, 640, 480);
	*/
	
	

	
	//
	// ** criando a top bar.
	//
	
	//#bug bug
	//enterCriticalSection();    // * Enter Critical Section.
	shellCreateTopBar();
	//exitCriticalSection();     // * Exit Critical section.	
	
	
	apiEndPaint();
	
	//
	// Funcionou setar o foco, e a mensagem foi para a janela certa.
	//
	
    //Registrar.
    APIRegisterWindow(hWindow);
	
    //Configurar como ativa.
    APISetActiveWindow(hWindow);
	
    //Setar foco.
	//*IMPORTANTE: � FUNDAMENTAL SETAR O FOCO, POIS O KERNEL DEPENDE DELE
	//PARA RETORNAR A MENSAGEM DA JANELA COM O FOCO DE ENTRADA.
    APISetFocus(hWindow);
	
	//#bugbug
	//janela usada para input de textos ...
	//o input de texto pode vir de v�rias fontes.
	//api_set_window_with_text_input(hWindow);
	
	// ** terminal **
	//definindo a janela como sendo uma janela de terminal.
	//isso faz com que as digita��es tenham acesso ao procedimento de janela de terminal 
	//para essa janela e n�o apenas ao procedimento de janela do sistema.
	system_call( SYSTEMCALL_SETTERMINALWINDOW, 
	             (unsigned long) hWindow, 
				 (unsigned long) hWindow, 
				 (unsigned long) hWindow );
				 
				 
				 
	//salva ponteiro da janela principal. 
	shell_info.main_window = ( struct window_d* ) hWindow;			 
		
	
	//
	// @todo: Apenas registrar o procedimento dessa janela na sua estrutura no kernel..
    // 
	
	
	//
    // ...		
	//
	
	
	//
	// Init Shell:
	//     Inicializa vari�veis, buffers e estruturas. Atualiza a tela.
	//
	
	
	
	enterCriticalSection();    // * Enter Critical Section.	
	Status = (int) shellInit(hWindow); 
	if(Status != 0){
		printf("[SHELL.BIN]: app_main: shellInit fail!");
		refresh_screen();
		while(1){};
		//exit(0);
	};
	exitCriticalSection();     // * Exit Critical section.		
	
	
	
	
    	
	
	
	//
	// Messages.
	//
	

    //printf("shellInit funcionou.\n");
	
	
	//g_cursor_x = 0;
	//g_cursor_y = 32;
	
	//system_call( SYSTEMCALL_DRAWTEXT, 
	//             shell_cursor_x,  
	//			   shell_cursor_y, 
	//			   (unsigned long) "Starting SHELL.BIN ... \n" );
	
	//system_call( SYSTEMCALL_DRAWTEXT, x,  y, (unsigned long) "$>_");		
	    
	//Printf Funcionando. (stdio.c)
	//printf("Shell printf test\n");   
	//printf_main();
		
	//Testing String 		
	//system_call( SYSTEMCALL_DRAWTEXT, 0,  0, (unsigned long) "/SHELL.BIN Testing strings...");
	//system_call( SYSTEMCALL_DRAWTEXT, 0, 20, (unsigned long) "Shell: 2 Testing strings...\n"); 
	//system_call( SYSTEMCALL_DRAWTEXT, 0, 40, (unsigned long) "Shell: 3 Testing strings...\n");

	//Refresh screen
	//system_call( SYSTEMCALL_REFRESHSCREEN, 0, 0, 0);
	
	
	
	//@todo: Isso � um teste.
	//system("reboot");
	
	//
	// Podemos tentar criar um processo.
	//

 	


    //
	// Get message.
	//

	/*
	 * @todo: 
	 *     Entrar num loop, solicitando ao kernel possiveis mensagens
	 * deixadas no buffer de mensagens do PCB do processo shell.
	 * Obs: O kernel coloca na fila de mensagens do processo, mensagens
	 * provenientes n�o apenas do teclado, mas tamb�m de outras fontes.
	 * Ent�o dessa forma, pode-se enviar mensagens de um processo para outro.
	 * *IMPORTANTE:Cabe a cada processo pegar as mensagens deixadas no buffer de mensagens 
	 * em seu PCB, atravez de uma chamada ao kernel.
	 */
    
    
	
	
	//
	// Por fim: Testar cursor e terminar.
	//

	//@todo: 0,0 n�o est� na �rea de cliente.
	
	/*
	printf("Testing cursor ...\n");
    
	shellSetCursor(shellMaxColumns,shellMaxRows);  
	printf("C");
	shellSetCursor(shellMaxColumns-1,shellMaxRows-1);
	printf("U");
	shellSetCursor(shellMaxColumns-2,shellMaxRows-2);
	printf("R");
	shellSetCursor(shellMaxColumns-3,shellMaxRows-3);
	printf("S");
	shellSetCursor(shellMaxColumns-4,shellMaxRows-4);
	printf("O");
	shellSetCursor(shellMaxColumns-5,shellMaxRows-5);
	printf("R");
	shellSetCursor(shellMaxColumns-6,shellMaxRows-6);
	printf(":)");
	*/
	
	
//initializePrompt:
	
	//inicializa o prompt.	
	//shellPrompt();	
    //refresh_screen();	
	
//entrandoNoWhile:	
	//printf("SHELL: While...");
	//while(1){}
	
	//printf(SHELL_PROMPT);
	//refresh_screen();	
	
	
	//** hang.
	//printf("*Hang");
	//refresh_screen();
    //while(1){} 	
	
	//
	// **** Mensagens  ****
	//
	
	//printf("Tentando pegar mensagem enviada para o procedimento de janela.");
	//refresh_screen();
	
	//isso � um teste pegar um valor por vez n�o � a melhor op��o.
	
	struct window_d *hwTest;
	int msgTest;
	void *long1;
	void *long2;
	
	//struct shell_message_d *msg;

        //
		// Get Message: 
		// Systemcall get message
		// Enviamos um ponteiro de estrutura de janela para que o Kernel possa 
		// pegar a mensagem que esta dentro da estrutura. Essa estrtura fica 
		// protegida no Kernel.
		//
		
		//
		// #bugbug: Na verdade essa rotina est� pegando a mensagem na janela 
		// com o foco de entrada. esse argumento foi passado mas n�o foi usado.
		//
		
	while(1)
	{


		//  #bugbug SYSTEMCALL_GET_KEYBOARD_MESSAGE est� pegando a mensagem de teclado,
		//          mas na verdade deveria apenas pegar a mensagem, sem se preocupar em 
		//          qual foi o dispositivo gerador do evento. ??!!
		

       apiBeginPaint();
	   
		//msg->window = (struct window_d *) system_call( SYSTEMCALL_GET_HWINDOW, 
 		hwTest = (struct window_d *) system_call( SYSTEMCALL_GET_HWINDOW, 
		                              (unsigned long) hWindow,  
									  (unsigned long) hWindow, 
									  (unsigned long) hWindow );
									  
        //#bugbug ( Aqui devemos pegar a mensagem sem se preocupar em
		//identificar o dispositivo gerador do evento.
		//msg->msg = (int) system_call( SYSTEMCALL_GET_KEYBOARD_MESSAGE,
        msgTest = (int) system_call( SYSTEMCALL_GET_KEYBOARD_MESSAGE, 		
		                              (unsigned long) hWindow,  
									  (unsigned long) hWindow, 
									  (unsigned long) hWindow );
									  
		 
		//msg->long1 = (unsigned long) system_call( SYSTEMCALL_GET_LONG1,
        long1 = (void*) system_call( SYSTEMCALL_GET_LONG1,		
		                             (unsigned long) hWindow, 
									 (unsigned long) hWindow, 
									 (unsigned long) hWindow );
									 
		 
		//msg->long2 = (unsigned long) system_call( SYSTEMCALL_GET_LONG2,
        long2 = (void*) system_call( SYSTEMCALL_GET_LONG2,		
		                             (unsigned long) hWindow, 
									 (unsigned long) hWindow, 
									 (unsigned long) hWindow );
									 
		apiEndPaint();
									 

        //
        // + PEGAMOS A MENSAGEM NA FILA DA JANELA COM O FOCO DE ENTRADA.
        //   ?? COMO DECIDIREMOS QUAL JANELA SER� AFETADA PELO PROCEDIMENTO 
        //	    QUE CHAMAREMOS AGORA ??
        //		
		
		// Send Message to procedure.
		
		//if( (int) msg->msg != 0 )
        if( (int) msgTest != 0 )			
		{
            //
            // *IMPORTANTE:
			//  SE TEMOS UM HANDLE DE JANELA, SIGNIFICA QUE O PROCEDIMENTO PODE 
			//  AFETAR MAIS DE UMA JANELA. PORTANTO JANELAS FILHAS OU ATE MESMO JANELAS 
			//  CRIADAS PELO SISTEMA PODER�O SER AFETADAS POR ESSE PROCEDIMENTO??
			//  @TODO: PASSAR O HANDLE DE JANELA PARA O PROCEDIMENTO.
            //			
		    shellProcedure( (struct window_d *) hwTest, 
			                (int) msgTest, 
							(unsigned long) long1, 
							(unsigned long) long2 );

		    //shellProcedure( msg->window, msg->msg, msg->long1, msg->long2 ); 
							
			//printf("Y-DEBUG: hwindow NULL\n"); //Deletar isso depois.
			//printf("Y-DEBUG: msg={%d}\n",msgTest); 
			//printf("Y-DEBUG: long1={%c}\n",long1); 
			//printf("Y-DEBUG: long2={%c}\n",long2); 
		};
		
		//Nothing.
	};
	
	
	//
	// Exit process.
	//
	
end:
    while(1){}    //Hang.
    //MessageBox(...);	
	exit(0);  
	return (int) 0;
};


/*
 ***********************************************
 * shellProcedure:
 *     Procedimento de janela.
 *     LOCAL
 */
unsigned long 
shellProcedure( struct window_d *window, 
                int msg, 
				unsigned long long1, 
				unsigned long long2 )
{
    unsigned long input_ret;
    unsigned long compare_return;	
	
    switch(msg)
    { 
		case MSG_KEYDOWN:
            switch(long1)
            {
				//Null key
				case 0:
				    return 0;
					//#BUGBUG esse null aqui dava problemas na digita��o.
					//printf(" null ");
				    break;
				
				//reset prompt.
				//case '8':
				//    shellPrompt();
				//    goto done;
				//	break;
				    
				//test return
				//case '9':
				case VK_RETURN:
				    input('\0'); //finaliza a string.
					shellCompare(window);
					goto done;
                    break;
                //Mostrar o buffer
                //case '1':
                   // printf("%s\n",&prompt[0]);
				//    break; 

				//Mostrar o buffer	
               // case '2':
                //    printf("%s\n",prompt);
				//    break; 

				//	
                //case '#':
				//case '3':
				//    printf("Shell procedure case 3\n");
                //    break;
					
                //case '9':
                //    MessageBox( 1, "Shell","Test 9");
                //    break; 					
                              
                //Texto - Envia o caractere.
                   //Imprime os caracteres normais na janela com o foco de entrada.
				//enfilera os caracteres na string 'prompt[]'.
				   //para depois ser comparada com outras strings.
                default:			   
				    input( (unsigned long) long1);      //Coloca no stdin
                    shellInsertNextChar((char) long1);  //Coloca no stdout
					
					//#BUGBUG 
					//N�o est� havendo o refresh do char, ele � pintado no 
					//backbuffer mas n�o aparece � efetuado o refresh do char.
					//obs: isso � o certo para alguns casos, para outros o char 
					//tem que aparecer.
					printf("%c", (char) long1); 		//#importante: IMPRIMINDO.			
					goto done;
                    break;               
            };
        break;
		
		case MSG_KEYUP: 
		    //printf("%c", (char) 'u');
           // printf("%c", (char) long1);  			
		    break;
		
		//N�o intercptaremos mensagens do sistema por enquanto.
		//As mensagens do sistema s�o interceptadas primeiro pelo procedimento 
		//do sistema.
		//case MSG_SYSKEYDOWN:
		//    switch(long1)
		//    {
		//		case VK_ESCAPE: printf("shell procedure Esc\n");
		//	};
		//break;
		
		case MSG_SYSKEYUP: 
		    //printf("%c", (char) 'U');
            //printf("%c", (char) long1);			
		    break;
          
		//
        //  *** Aqui o procedimento de janelas do kernel vai enviar uma mensagem 
		// notificando que os bot�es de controle F1 ou F2 foram apertados ... 
		//      F1 significa que temos que abri o menu de aplicativos e 
		// F2 significa que temos que abrir a janela do interpretador de comando ...
        //		
		  
		case MSG_COMMAND:
            switch(long1)
			{
				case 0:
				    MessageBox( 1, "Shell","Testing MSG_COMMAND.NULL.");
				    break;
				
				//About
				case CMD_ABOUT:
				    MessageBox( 1, "Shell","Testing MSG_COMMAND.CMD_ABOUT.");
				    break;
				
				//clicaram no bot�o
				case BN_CLICKED:
				    if(window == i1Window){
					     //@todo: abre o menu de aplicativos
					}
				    if(window == i2Window){
					   //@todo: abre o interpretador de comandos.
					}
					//#debug
					printf("  ** BN_CLICKED  **  \n");
				break;
				//...
				
				//default:
				//break;
			}
		break; 		
		
		//Essa mensagem pode ser acionada clidando um bot�o.
		//case MSG_CLOSE:
		    //??
		    //isso deve fechar qualquer janela que esteja usando esse procedimento.
			//pode ser uma janela filha ou ainda uma janela de dialogo criada pelo sistema.
			//??
		//break;
		
		//Essa mensagem pode ser acionada clidando um bot�o.
		//case MSG_DESTROY:
		//break;
		
		//Quando a aplicativo em user mode chama o kernel para 
		//que o kernel crie uma janela, depois que o kernel criar a janela,
		//ele faz uma chamada ao procedimento de janela do aplicativo com a mensagem 
        //MSG_CREATE, se o aplicativo retornar -1, ent�o a rotina em kernel mode que 
        //esta criando a janela, cancela a janela que est� criando e retorn NULL.
        //		
		//case MSG_CREATE:
		//    break;
		
		//Mensagem desconhecida.
		default:
		    //printf("shell procedure: mensagem desconhecida\n");
		    goto done;
		    break;	  
    };


    //
    // Nothing for now !
    //


//printf("Shell procedure\n");

	
	
done:
    //@todo: variavel que indica que estamos em modo gr�fico.
	//if(VideoBlock.useGui == 1)
	//{
	    //Debug.
		//refresh_screen(); 
		//Obs: #bugbug perceba que o procedimento de janela do 
		//sistema tamb�m tem um refresh screen.
	//};	
	
	return (unsigned long) apiDefDialog(window,msg,long1,long2);
    //return (unsigned long) 0;	
};


/*
 * shellWaitCmd:
 *     Espera completar o comando com um [ENTER]. ##suspensa
 */
void shellWaitCmd()
{
	//
	// @todo: Cuidado com loop infinito.
	//
	
	//
	// Obs: Quem muda o status � o procedimento,
	//      quando recebe o retorno da fun��o input:
	//
	
	
	//
	// Uma interrup��o de teclado aciona o procedimento
	// do shell que atualiza o status.
	//
	
	
	
	//
	// BUG BUG :
    //
    //    Pra esse shell funcionar teria que habilita
    //    agora a interrup��o de teclado e somente ela
    //    para que a interrup��o de timer n�o bagun�e as
    //    coisas fazendo troca de contexto.
    //	
	//
	
	//asm("sti");    //@todo; N�o habilitar!
	
    //Loop.  
	
    do{	 
		if( prompt_status == 1 ){
			prompt_status = 0;     
			return;
	    };
	
	}while(1);
	
exit:
    prompt_status = 0;
    return;	
};


/*
 **********************************************************
 * shellCompare:
 *     Compara comandos digitados com palavras chave.
 *     Compara a palavra em 'prompt[]' com outras e chama o servi�o.
 * o enter foi o caractere digitado, vamos comparar palavras.
 *
 * Credits:
 * + Stephen Brennan - https://brennan.io/2015/01/16/write-a-shell-in-c/
 * + Frederico Lamberti Pissarra 
 *
 */
 
#define LSH_TOK_DELIM " \t\r\n\a" 
 
unsigned long shellCompare(struct window_d *window)
{
    unsigned long ret_value;
	
	//
	// ?? E se tivermos v�rias palavras na linha de comando ??
	//
		
    // Returns first token 
    //char *token = strtok(prompt, " -");
	
	//#debug
    printf("shellCompare: Testing ...\n");
    refresh_screen();	
   
    char *tokenList[80];
    int i = 0;
	int token_count;
	
	
	// ?? what ?
    // first call returns pointer
	// to first part of user_input
	// separated by delim
	
    tokenList[0] = strtok( prompt, LSH_TOK_DELIM);
	
	
	//para o argumento atual
	//@todo: isso precisa ser limpado sempre.
	
	char *token;
	
	token = (char *) tokenList[0];
	
	i=0;                                  
    while( token != NULL )
	{
        // Coloca na lista.
        tokenList[i] = token;

		//#debug
		//Mostra
        printf("shellCompare: %s \n", tokenList[i] );
        refresh_screen();

		// Incrementa o �ndice da lista
        i++;
		
		token = strtok( NULL, LSH_TOK_DELIM );
		
		//salvando a contagem.
		token_count = i;
    }; 

	//Finalizando a lista.
    tokenList[i] = NULL;

	//#debug
    printf("shellCompare: %s \n", tokenList[i] );
    //refresh_screen();	

	//#debug
    printf("shellCompare: Test done!\n");
    refresh_screen();	


	// Zerando o �ndice do tokenList
    i=0;	
   
    //printf("shellCompare: Testing ...\n");
    //refresh_screen();
   
    // Keep printing tokens while one of the
    // delimiters present in str[].
    //while(token != NULL)
    //{
    //    printf("%s\n", token);
	//	refresh_screen();
	//	token = strtok(NULL, " -");
    //}
	
		
		//do_command(argc, argv);
		goto do_command;	
	
	
do_command:	
    //nothing.	
do_compare:

    //
	// #Importante:
	// Devemos pegar os argumentos salvos na lista.
	// O primeiro argumento � o nome do aplicativo que deve ser executado
	// e os outros argumentos s�o argumentos que devem ser passados para 
	// o aplicativo a ser executado.
	//
	
	//
	// Talvez aqui devamos usar tokenList[0] e n�o prompt.
	//
	

		
    // L1 RAM /objetcs   
	// (diret�rio raiz para os arquivos que s�o diret�rios de objetos)
	// os objetos ser�o listador em um arquivo que nunca ser� salvo no disco.
	if( strncmp( prompt, "/objects", 6 ) == 0 )
	{
	    printf("info: Open object manager root dir ...\n");
        goto exit_cmp;
    };

	// L2 disk  /diret�rio raiz do sistema de arquivos.
	if( strncmp( prompt, "/", 5 ) == 0 )
	{
	    printf("info: Open file system root dir ...\n");
        goto exit_cmp;
    };

	// L3 LAN  // acesso a arquivos da lan
	//os arquivos lan ser�o listador em um arquivo que nunca ser� salvo no disco.
	if( strncmp( prompt, "/lan", 6 ) == 0 )
	{
	    printf("info: Open lan root dir ...\n");
        goto exit_cmp;
    };
 
	// L4 WAN   //acesso a arquivos da wan
	// os arquivos lan ser�o listador em um arquivo que nunca ser� salvo no disco.
	if( strncmp( prompt, "/wan", 6 ) == 0 )
	{
	    printf("info: Open wan root dir ...\n");
        goto exit_cmp;
    };
	
    //
	// Ordem alfab�tica.
	//
	

	
	//@todo
	//token
	//testando tokenList
	if( strncmp( prompt, "token", 5 ) == 0 )
    {
		printf("\nTesting tokenList ...\n");
		printf("\nTotal={%d}\n",token_count);
		printf("\n Comand = %s \n",tokenList[i]);
		refresh_screen();
		
		i++;
		token = (char *) tokenList[i];
		
		if( token == NULL ){
			goto exit_cmp;
		}else{
		    printf("\n argument_number={%d} argument={%s}\n", i, tokenList[i]);	
            
			if( strncmp( (char*) tokenList[i], "-a", 2 ) == 0 )
			{
			    printf("[OK] argumento %s selecionado.\n", tokenList[i]);
		    }
			//...
		};
		
		
		i++;
		token = (char *) tokenList[i];
		
		if( token == NULL ){
			goto exit_cmp;
		}else{
		    printf("\n argument_number={%d} argument={%s}\n", i, tokenList[i]);	
            
			if( strncmp( (char*) tokenList[i], "-b", 2 ) == 0 )
			{
			    printf("[OK] argumento %s selecionado.\n", tokenList[i]);
		    }
			//...
		};		
		
		
		i++;
		token = (char *) tokenList[i];
		
		if( token == NULL ){
			goto exit_cmp;
		}else{
		    printf("\n argument_number={%d} argument={%s}\n", i, tokenList[i]);	
            
			if( strncmp( (char*) tokenList[i], "-c", 2 ) == 0 )
			{
			    printf("[OK] argumento %s selecionado.\n", tokenList[i]);
		    }
			//...
		};		
		printf("\n");
		goto exit_cmp;
	};
    	
	
	// boot
	// ?? Boot info talvez.
	if( strncmp( prompt, "boot", 4 ) == 0 )
	{
	    printf("~boot\n");
		//boot();
        goto exit_cmp;
    };

	
    // cls
	// Clear the screen.
	if( strncmp( prompt, "cls", 3 ) == 0 )
	{
		//@todo
        shellClearScreen();
        shellSetCursor(0,0);
	    shellPrompt();
        goto exit_cmp;
	};
	
	
	// dir
	// Lista os arquivos no estilo DOS.
	if( strncmp( prompt, "dir", 3 ) == 0 )
	{
	    printf("~dir\n");
		//fs_show_dir(0); 
        goto exit_cmp;
    };
	
	
	// echo
	// Echo de terminal.
    if( strncmp( prompt, "echo", 4 ) == 0 )
	{
		//printf("%s\n",prompt[4]);
		printf("%s\n",prompt);
		goto exit_cmp;
    };	
	
	
	// exit
	// Exit the application.
    if( strncmp( prompt, "exit", 4 ) == 0 )
	{
        printf("~exit\n");
		refresh_screen();
		exit(0);
		goto exit_cmp;
    };

	// hd ??
    if( strncmp( prompt, "hd", 2 ) == 0 )
	{
	    printf("~hd\n");
        goto exit_cmp;
    };
	
	
	// help
	// Mostra ajuda.
    if( strncmp( prompt, "help", 4 ) == 0 )
	{
		shellHelp();
		goto exit_cmp;
    };	
	
	
	// install	
    // ??
	if( strncmp( prompt, "install", 7 ) == 0 )
	{
	    printf("~install\n");
		//fs_install();
        goto exit_cmp;
    };
	
	
    // ls
	// lista arquivos no estilo unix.
	if( strncmp( prompt, "ls", 2 ) == 0 )
	{
		//@todo: Isso deve ser um aplicativo.
		printf("~ls\n");
        goto exit_cmp;
	};	


    // metrics
	// Mostra algumas informa��es de m�trica do sistema.
	if( strncmp( prompt, "metrics", 7 ) == 0 )
	{
		shellShowMetrics();
        goto exit_cmp;
	};	

	
	// newfile
	if( strncmp( prompt, "newfile", 7 ) == 0 )
	{
	    printf("~newfile - create empty file.\n");
		//fs_create_file( "novo    txt", 0);
        goto exit_cmp;
    };
	
	
	// newdir
	if( strncmp( prompt, "newdir", 7 ) == 0 )
	{
	    printf("~newdir - create empty folder.\n");
		//fs_create_dir( "novo    dir", 0);
        goto exit_cmp;
    };

    
	// mbr
	// ?? Talvez mostrar informa��es sobre o mbr ou realizar testes.
    if( strncmp( prompt, "mbr", 3 ) == 0 )
	{
	    printf("~mbr\n");
		
		//#bugbug pagefault
		
		shellTestMBR();
		printf("done\n");
		goto exit_cmp;
    }; 

	
    // reboot
    // @todo: Isso dever� ser um aplicativo.	
    if( strncmp( prompt, "reboot", 6 ) == 0 )
	{
	    printf("~reboot\n");
		goto exit_cmp;
    };
	
	
    // root
	// ??
    if( strncmp( prompt, "root", 4 ) == 0 )
	{
	    printf("~/root\n");
		//testa_root();
		goto exit_cmp;
    }; 

	
    // save
	if( strncmp( prompt, "save", 4 ) == 0 )
	{
	    printf("~save root\n");
        goto exit_cmp;
    };

	
	// service
	if( strncmp( prompt, "service", 7 ) == 0 )
	{
	    printf("~service - testa servicos do kernel:\n");
		//test_services();
        goto exit_cmp;
    };
	
    // shellinfo
	// informa��es sobre o aplicativo.
	if( strncmp( prompt, "shellinfo", 9 ) == 0 )
	{
		
	    printf("~@todo: shell info.\n");
		shellShowInfo();
        goto exit_cmp;
    };	

	// slots
	if( strncmp( prompt, "slots", 5 ) == 0 )
	{
	    printf("~slots - mostra slots \n");
		//mostra_slots();
        goto exit_cmp;
    };
	
	
	// start
    if( strncmp( prompt, "start", 5 ) == 0 )
	{
		// Isso deve setar o foco na janela do shell.
		// ao mesmo tempo que reinicia o input para digita��o 
		// e ajusta as margens do cursor. :)
		// Qualquer editbox precisa desse tipo de ajuste.
	    
		APISetFocus(window);
		shellPrompt();
		printf("~start\n");
		
		goto exit_cmp;
    };

    // systeminfo
	// informa��es sobre o sistema.
	if( strncmp( prompt, "systeminfo", 10 ) == 0 )
	{
	    printf("~@todo: system info.\n");
		shellShowSystemInfo();
        goto exit_cmp;
    };	
	
	
    // t1 - Test file
	if( strncmp( prompt, "t1", 2 ) == 0 )
	{	
		// Carrega e exibe um arquivo.
		shellTestLoadFile();
		
		// teste.
		// Escreve no buffer de sa�da e mostra o buffer de saida.
		screen_buffer[0] = (char) 'F';
		screen_buffer[1] = 7;
		
        screen_buffer[2] = (char) 'N';	
        screen_buffer[3] = 7;
		
		screen_buffer[4] = (char) '\0';			
		screen_buffer[5] = 7;
		
		printf( (const char *) stdout->_base );
		
		printf("%s \n",stdout->_base);
		printf("%s \n",stdout->_tmpfname);
		printf("%d \n",stdout->_cnt);
		printf("%d \n",stdout->_bufsiz);
		printf("done \n");
		
        goto exit_cmp;
    };
	
	
	// t2 - Test bmp
	if( strncmp( prompt, "t2", 2 ) == 0 )
	{
		shellTestDisplayBMP();
        goto exit_cmp;
    };	
	
	
	// t3 - Test thread
	if( strncmp( prompt, "t3", 2 ) == 0 )
	{
	    shellTestThreads();
        goto exit_cmp;
    };
	
	
	// topbar
	// Cria uma top bar.
    if( strncmp( prompt, "topbar", 6 ) == 0 )
	{
	    enterCriticalSection();    
	    shellCreateTopBar();
	    exitCriticalSection();    
		
		goto exit_cmp;
    };			
	
	
	// tree
	// Desenha uma pequena �rvore.
    if( strncmp( prompt, "tree", 4 ) == 0 )
	{
		shellTree();
		goto exit_cmp;
    };			
	
	
	// version
    if( strncmp( prompt, "version", 7 ) == 0 )
	{
	    printf("\n Gramado version %s \n", OS_VERSION );
        goto exit_cmp;
    };

	// window
    if( strncmp( prompt, "window", 6 ) == 0 )
	{
		//
		// #bugbug.
		// Testando a estrutura de janela.
		// A janela foi criada pelo kernel e copiamos o ponteiro 
		// da estrutura para um ponteiro em user mode.
		// Podemos ter erros de mem�ria com essas opera��es.
		
	    printf("\n testing main window ... \n");
		
		// esse printf funcionou.
		printf("mainWindow={%x}", shell_info.main_window );
		
		//#bugbug 
		//temos um problema aqui.
		// provavelmente o erro � por acessar um endere�o que est� 
		// em kernel mode.
		//if( shell_info.main_window->left > 0 && shell_info.main_window->top > 0  )
		//{
		//    shellSetCursor( (shell_info.main_window->left/8), (shell_info.main_window->top/8) );
		//}
		
        printf("rect: l={%d} t={%d} w={%d} h={%d}\n", terminal_rect.left,
		                                              terminal_rect.top,
													  terminal_rect.width,
													  terminal_rect.height );

													  
		if( terminal_rect.left > 0 && terminal_rect.top > 0 )
		{											  
            shellSetCursor( (terminal_rect.left/8), (terminal_rect.top/8) );													  
		};
		
        goto exit_cmp;
    };


	//coment�rio
	// a linha � um coment�rio, podemos ignorar.
    if( strncmp( prompt, "//", 2 ) == 0 ){
		goto exit_cmp;
	};	
 
 
    //
    // Se apertamos o enter e n�o encontramos um comando v�lido
    // ent�o damos um aviso de comando inv�lido e reiniciamos o prompt 
    // na pr�xima linha.
    //
 
palavra_nao_reservada:
    printf(" Unknown command!\n");
	shellPrompt();
	//Mostrando as strings da rotina de compara��o.
	refresh_screen(); 	
	return (unsigned long) 1;
	
exit_cmp:
	shellPrompt();
	//Mostrando as strings da rotina de compara��o.	
	refresh_screen(); 
    return (unsigned long) 0;
};


/*
 ******************************************
 * shellShell:
 *     Constructor.
 */
void shellShell()
{
	int i=0;
	
	//
	// Internas.
	//
	
	//
	// Obs: Configurar cursor para tela pequena.
    // Screen 320x480.(Celular).
	// Client Area = (Altura - 24)
    //	
	
    shellStatus = 0;
    shellError = 0;
	
	//window position
	//shell_window_x = DEFAULT_WINDOW_X;
	//shell_window_y = DEFAULT_WINDOW_Y;
	shell_window_x = (800/2);
	shell_window_y = (600/8);

	
	//screen sizes
	shellScreenWidth  = (800/2);
    shellScreenHeight = (600 - (600/8) );   // 600;
	
	//window height
	//shellWindowWidth = (DEFAULT_MAX_COLUMNS*8);
    //shellWindowHeight = (DEFAULT_MAX_ROWS*8);
	
	shellWindowWidth = (800/2);
	shellWindowHeight = (600 - (600/8) );
	
    shellMaxColumns = DEFAULT_MAX_COLUMNS;  // 80;
    shellMaxRows    = DEFAULT_MAX_ROWS;     // 25;

    //...	

	//
	// Setup buffers.
	//
	
    // reiniciando as vari�veis na estrutura do output
	stdout->_base = &screen_buffer[0];
	stdout->_ptr = stdout->_base;
	stdout->_cnt = PROMPT_MAX_DEFAULT;
	stdout->_file = 1;
	stdout->_tmpfname = "shell_stdout";
	//...	
	//
	// Obs:
	// shell_buffer[] = Aqui � o buffer de output. 
	// prompt[] - Aqui ficam as digita��es. 
	//
	shellClearBuffer();

	
	//shellBufferMaxColumns = DEFAULT_BUFFER_MAX_COLUMNS;
	//shellBufferMaxRows    = DEFAULT_BUFFER_MAX_ROWS;
	
	//buffersize = (shellBufferMaxColumns * shellBufferMaxRows);
	

	
	//
	// @todo: E o fluxo padr�o. Quem configurou os arquivos ???
	//        o kernel configuroru???
	//
	
	// N�mero m�ximo de colunas e linhas.
	g_columns = shellMaxColumns;  // 80;
	g_rows = shellMaxRows;        // 25;
    //...
	
	
	//...
	
done:	
    //Nossa refer�ncia � a moldura e n�o a �rea de cliente.
	//@todo:usar a �rea de cliente como refer�ncia
	//shellSetCursor(0,0);
    shellSetCursor(0,4);
    
	//@todo
	//tentando posicionar o cursor dentro da janela
	//shellSetCursor( (shell_info.main_window->left/8) , (shell_info.main_window->top/8));	
	
	shellPrompt();
    return;	
};



/*
 ********************************************************************
 * shellInit:
 *     Inicializa o Shell.  
 *
 *     #bugbug: 
 *     Essa rotina come�a escrever na janela com o foco de entrada. Mas um 
 * outro aplicativo solicitou o foco de entrada e essa rotina esta terminando 
 * de escrever mas agora na janela do outro aplicativo.
 *
 * @todo: Inicializar globais e estruturas.
 *
 */
int shellInit( struct window_d *window )
{
	int PID;
	int PPID;
	int ActiveWindowId = 0;
	int WindowWithFocusId = 0;
	void *P;
	
	//stream status
	shell_info.stream_status = 0;
		
	//cursor
	shellSetCursor(0,4);
	
	//pointer
	shellPrompt();
    
	// message.
    printf("shellInit: Running tests ...\n");	


	//
	// Window support.
	//

	//window
	if( (void*) window == NULL ){
	    printf("shellInit: window fail.\n");    
	}else{
		APISetFocus( window );
	};
	
	
	//Active window
	ActiveWindowId = (int) APIGetActiveWindow();
	
	//valor de erro
	if( ActiveWindowId == (-1)){
	    printf("shellInit: ERROR getting Active window ID\n");	
	}	
	printf("ActiveWindowId={%d}\n", ActiveWindowId );


	//
	// Focus.
	WindowWithFocusId = (int) APIGetFocus();
	
	//valor de erro
	if( WindowWithFocusId == (-1)){
	    printf("shellInit: ERROR getting Window With Focus ID\n");	
	}	
	printf("WindowWithFocusId={%d}\n", WindowWithFocusId );	
	
	
	
	//columns and rows
	printf("shellMaxColumns={%d} \n", shellMaxColumns );
	printf("shellMaxRows={%d} \n", shellMaxRows );	
		
	
	
	//
	// Process support.
	//
	
	//
	// @todo: 
	// Essa mensagem est� aparecendo fora da �rea de trabalho do shell
	// pois ainda n�o temos um ponteiro para a mem�ria que representa essa �rea.
	// Talvez as mensagens devessem ir para um buffer de linha antes de irem 
	// para a mem�ria de v�deo.
	// #Impotante:
	// Devemos utilizar as configura��es de terminal virtual, respeitar a estrutura 
	// de terminal, que indicar� qual � a janela de terminal onde os caracteres 
	// devem ser escritos. Na verdade � um ponteiro para um ret�ngulo e n�o para 
	// uma janela. Obs: Esse ret�ngulo do terminal deve esr configur�vel e uma rotina 
	// deve dar suporte a essa configura��o.
	//
	
	//
	// @todo: 
	// O que tevemos fazer aqui � pegar informa��es sobre o processo Shell
	// e coloca-las na tela.
	//
	
	//
	// @todo: Criar na API uma rotina de inteface que use essa chamada.
	// ex: APIGetPID().
	//
	
	//PID = (int) APIGetPID();
	
    PID = (int) system_call( SYSTEMCALL_GETPID, 0, 0, 0);
	if( PID == (-1)){
	    printf("ERROR getting PID\n");	
	}
  
    PPID = (int) system_call( SYSTEMCALL_GETPPID, 0, 0, 0);
	if( PPID == (-1)){
	    printf("ERROR getting PPID\n");	
	}
  
	printf("Starting SHELL.BIN ... PID={%d} PPID={%d} \n", PID, PPID );
	
	

	
	//
	// @todo: Criar processos processos:
	//     E.BIN, F.BIN, G.BIN, A.BIN, B.BIN, C.BIN, D.BIN
 	//     (Mermaids) Usados para testes.
	//

	printf("Creating processes ...\n");
	
	//D.:)
	P=(void*)apiCreateProcess(0x400000,PRIORITY_HIGH,"D");
	if((void*)P==NULL){printf("Fail creating process D :)\n");};
	//C.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_HIGH,"C");
	if((void*)P==NULL){printf("Fail creating process C.\n");};
	//B.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_HIGH,"B");
	if((void*)P==NULL){printf("Fail creating process B.\n");};
	//A.(Balance).
	P=(void*)apiCreateProcess(0x400000,PRIORITY_NORMAL,"A");	
	if((void*)P==NULL){printf("Fail creating process A.\n");};
	//G.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_LOW,"G");	
	if((void*)P==NULL){printf("Fail creating process G.\n");};
	//F.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_LOW,"F");
	if((void*)P==NULL){printf("Fail creating process F.\n");};
	//E.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_LOW,"E");	
	if((void*)P==NULL){printf("Fail creating process E :( \n");};
	
	printf("Created!\n");
	//...
	
	
	
	//
	//@todo: 
	// Devemos implementar uma rotina de inicializa��o dessas threads 
	// criadas atrav�s de chamadas ao sistema.
	//
	
	//printf("Initializing a thread...\n");
	//...
	
	//Testando bibliotecas.
	//printf("Testing libraries:\n");	
	
	//stdio.h
	//printf("Testing stdio:\n");
	//app_clear(0);  //N�o fez nada.
    //...

	//stdlib.h
	printf("Testing stdlib:\n");
	
	//
	// *Importante:
	//     Isso inicializa o gerenciamento de mem�ria oferecido pela 
	// biblioteca C99 em user mode. Coisas como inicializar o 'heap'
	// oferecido pela biblioteca.
	// Agora poderemos alocar mem�ria dentro do heap oferecido pela biblioteca.
	//
	
//initRT:	
	//libcInitRT(); 
	
	//Obs: Sempre inicia com o mesmo n�mero.
	int rand_value;
	rand_value = (int) rand();
	printf("RandValue1={%d}\n", rand_value);
	//rand_value = (int) rand();
	//printf("RandValue2={%d}\n", rand_value);
    //rand_value = (int) rand();
	//printf("RandValue3={%d}\n", rand_value);
	//...
	

	//stddef.h
	//printf("Testing stddef:\n");	
	
	//Testing COLUMNS
	
	//Testing ROWS
	
	
	//testando api.h
	//...
	
	
	//Funcionou...
	//sprintf( shell_buffer,"Testando String dentro do buffer\n");
	//printf("%s",shell_buffer);
	

	//
	// Get process info.
	//
	
	
	//
	// Get thread info.
	//
	

	
	//
	// Testing commands.
	//

	//Lib C.
	//libC. (stdlib.c)
	system("test");       
	system("ls");
	system("start");
	system("xxfailxx");
	//...
	
	//API.
	apiSystem("test");    
    apiSystem("ls");
	apiSystem("start");
	apiSystem("xxfailxx");
	//...
	
	//Ok funcionando ...
	//@todo: Testar outros comandos.
	//...
	
	
	//Testing message box.
	//MessageBox(3, "Shell message box","Testing message box...");
	
	//
	// @todo: Clear client area.
	//        System call redraw client area.
	

	
done:
    //
	// Testing welcome message.
	//
	
	/*
	printf("...\n");
	test_operators();
	*/
	
	printf("...\n");
	printf("Welcome to Gramado Operating System.\n");
	printf("Done!");
	
	
	//
	// @todo:
	// Gerenciamento do heap do processo. ??
	//
	
	
	//
	// @todo:
	// Chamar rotinas da biblioteca que ofere�am informa��es sobre 
	// o heap oferecido pela biblioteca.
	// Obs: A rt foi inicializada logo acima.
    //	
	
	
    //heapTest:
    /*	
	printf("\n...\n");
	printf("Testing C99 RT ...\n");
	
	unsigned long hStart, hEnd, hPointer, hAvail;
	
	hStart   = (unsigned long) rtGetHeapStart();
	hEnd     = (unsigned long) rtGetHeapEnd();
	hPointer = (unsigned long) rtGetHeapPointer();
	hAvail   = (unsigned long) rtGetAvailableHeap();
	
	printf("heapStart{%x} heapEnd{%x} heapPointer{%x} heapAvailable={%x}\n", 
	     hStart, hEnd, hPointer, hAvail);
	
	// resultados do teste:
	// os valores parecem satisfat�rios pois est�o realmente dentro da �rea 
	// de mem�ria do aplicativo.
	// @todo: Confirmar no memorymap gerado pelo compilador se essa �rea de mem�ria 
	// � apropriada. #bugbug
	// observando o mmmap pelo jeito o compilador colocou o buffer do heap 
	// no bss do arquivo e do tamanho certo.
	// tudo indica que � saud�vel aumentar o tamanho do buffer usado pelo heap.
	//
	*/
	
	shellPrompt();
    refresh_screen();
	
    return (int) 0;
};


/*
 * shellSetCursor:
 *     Configurando o cursor. (stdio.h).
 *
 * @todo: Aqui quando definimos os valores o cursor no shell 
 * devemos considerar que a janela com o foco de entrada tambem tem um cursor...
 * Temos que atualizar o cursor da janela com foco de entrada se quizermos 
 * escrever corretamente dentro dela.
 * e isso se faz atrav�s de uma chamada ao kernel.
 */
void shellSetCursor(unsigned long x, unsigned long y)
{	
    //setando o cursor usado pelo kernel base.	
    apiSetCursor(x,y);
	
//Atualizando as vari�veis globais usadas somente aqui no shell.
setGlobals:	
    g_cursor_x = (unsigned long) x;
    g_cursor_y = (unsigned long) y;	
	return;
};


/*
 * shellThread:
 *     Um thread dentro para testes.
 */
void shellThread()
{
    printf("shellThread: This is a thread for test!\n");
    refresh_screen();
	
    while(1){	
	    //printf("$");
    }	
};


//help message
void shellHelp(){
    printf(help_banner);	
	return;
};

//drawing a tree
void shellTree(){
    printf(tree_banner);	
	return;
};

//
// C function to demonstrate the working of arithmetic operators
//#include <stdio.h>
int test_operators()
{
    int a = 9,b = 4, c;
    
	printf("Testing operators ...\n");
	
    c = a+b;
    printf("a+b = %d \n",c);

    c = a-b;
    printf("a-b = %d \n",c);
    
    c = a*b;
    printf("a*b = %d \n",c);
    
    c=a/b;
    printf("a/b = %d \n",c);
    
    c=a%b;
    printf("Remainder when a divided by b = %d \n",c);
    
    return 0;
};


/*
 * shellPrompt:
 *     Inicializa o prompt.
 *     Na inicializa��o de stdio, prompt foi definido como stdin->_base.
 */
void shellPrompt()
{	
	int i;
	for(i=0; i<PROMPT_MAX_DEFAULT;i++){
		prompt[i] = (char) '\0';
	};
	
    prompt[0] = (char) '\0';
	prompt_pos = 0;
    prompt_status = 0;
	prompt_max = 250;  //??	PROMPT_MAX_DEFAULT

    printf("\n");	
	//printf("\n %s", ++ user name  ++);
	printf(SHELL_PROMPT);
	return;
};


/*
 * shellClearBuffer:
 *     Limpa o buffer da tela.
 */
void shellClearBuffer()
{
	int i;
	
	// Shell buffer.
	for( i=0; i<SCREEN_BUFFER_SIZE; i++)
	{
		screen_buffer[i] = 0;
	}
	
	screen_buffer[0] = (char) '\0';
	screen_buffer[1] = 7;
	
    screen_buffer_pos = 0;  //?? posi��o dentro do buffer do shell.	
};


/*
 * shellTestLoadFile:
 *     Carrega um arquivo de texto no buffer e mostra na tela.
 */
void shellTestLoadFile()
{
	shellClearBuffer();
	shellClearScreen();
    shellSetCursor(0,0);
	shellPrompt();
	//@todo: reposicionar o cursor.
	
	//
	// *Testando carregar um arquivo.
	// Ok, isso funcionou.
	//
	
	int fRet;
	printf("...\n");
	printf("Testing buffer ... Loading file...\n");
	
	//A QUEST�O DO TAMANHO PODE SER UM PROBLEMAS #BUGBUG ;... SUJANDO ALGUMA �REA DO SHELL
	fRet = (int) system_call( SYSTEMCALL_READ_FILE, 
	                          (unsigned long) init_file_name, 
					          (unsigned long) &screen_buffer[0], 
							  (unsigned long) &screen_buffer[0] );
							  
	printf("ret={%d}\n",fRet);
	
	printf("...\n\n");
	printf(&screen_buffer[0]);		
};


/*
 * shellTestThreads:
 *     Cria um thread e executa.
 *     #bugbug ...j� funcionou uma vez.
 */
void shellTestThreads()
{
    void *T;	
	//
	// Obs: As threads criadas aqui et�o sendo atribu�das ao processo PID=0.
	//      @todo: No kernel, quando criar uma thread ela deve ser atribu�da
    //      ao processo que chamou a rotina de cria��o.	
	//
	printf("Creating threads...\n");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread1");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread2");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread3");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread4");
	//...
	
	//
	// Tentando executar um thread.
	//
	
	
	// *******************************
     //OBS: ISSO FUNCIONOU. ESTAMOS SUSPENDENDO PORQUE PRECISAMOS AUMENTAR O TAMANHO DO 
     //     HEAP USADO PELO PROCESSO PARA ALOCA��O DIN�MICA, ELE N�O TA DANDO CONTA 
     //     DE TODA A DEMANDA POR MEM�RIA.		  
	
	//>>dessa vez pegaremos o retorno, que deve ser o ponteiro para a estrutura da thread.
	//>>chamaremos a systemcall que executa essa thread que temos o ponteiro da estrutura.
    void* ThreadTest1;	
	
	//#bugbug, n�o temos mais epapo no heap do preocesso para alocar mem�ria 
	//pois gastamos o heap com a imagem bmp.
	//
	unsigned long *threadstack1;
	threadstack1 = (unsigned long *) malloc(30*1024);
	threadstack1 = ( threadstack1 + (30*1024) - 4 ); //Ajuste para o in�cio da pilha.
	ThreadTest1  = (void*) apiCreateThread( (unsigned long)&shellThread, 
	                                        (unsigned long) threadstack1, 
										    "ThreadTest1" );
										   
	
	printf("shell: Tentando executar um thread ...\n");
	refresh_screen();
	
	if( (void*) ThreadTest1 == NULL ){
	    printf("shell: Tentando executar um thread FAIL NULL ...\n");	
	    refresh_screen();
		while(1){}
	}
	//L� no kernel isso deve selecionar a thread para execuss�o colocando ela no estado standby
	apiStartThread(ThreadTest1);
	printf("shell: Tentando executar um thread [ok] hang...\n");
	refresh_screen();
	
	//while(1){}
	// **************************
	return;	
};


/*
 * shellClearScreen:
 *     Limpar a tela do shell.
 */
void shellClearScreen()
{
	int i;
	
	shellSetCursor(0,0);
	
	// Shell buffer. (80*25) ??
	for( i=0; i<SCREEN_BUFFER_SIZE; i++){
		printf("%c", ' '); //pinta um espa�o.
	}
	shellSetCursor(0,0);
};


/*
 * shellRefreshScreen:
 *     Copia o conte�do do buffer para a tela. (dentro da janela).
 *
 */
void shellRefreshScreen()
{
	int i;

	//cursor apontando par ao in�cio da janela.
	shellSetCursor(0,0);
	
	// Shell buffer.
	for( i=0; i<SCREEN_BUFFER_SIZE; i++){
		printf("%c", stdout->_ptr[i]);
	};
	
    //shell_buffer_pos = 0;  //?? posi��o dentro do buffer do shell.	
};


/*
 * shellScroll:
 *     @todo:
 */
void shellScroll()
{
	int i;
    int end = (SCREEN_BUFFER_SIZE+80);
	
	//cursor apontando par ao in�cio da janela.
	shellSetCursor(0,0);
	
	// Shell buffer.
	for( i=80; i<end; i++){
		printf("%c", stdout->_ptr[i]);
	};
	
    //screen_buffer_pos = 0;  //?? posi��o dentro do buffer do shell.		
};



static void save_cur(void)
{
	saved_x = screen_buffer_x;
	saved_y = screen_buffer_y;
};


static void restore_cur(void)
{
	x=saved_x;
	y=saved_y;
	//pos = origin + ( (y * columns + x) << 1 );
	screen_buffer_pos = origin + (screen_buffer_y * columns + screen_buffer_x);
};


static void lf(void)
{
	if (screen_buffer_y+1 < bottom) {
		screen_buffer_y++;
		screen_buffer_pos += columns;  //pos += columns<<1;
		return;
	}
	//scrup();
};


static void ri(void)
{
	if (screen_buffer_y>top) {
		screen_buffer_y--;
		screen_buffer_pos -= columns; //screen_buffer_pos -= columns<<1;
		return;
	}
	//scrdown();
};


static void cr(void)
{
	screen_buffer_pos -= screen_buffer_x; //pos -= x<<1;
	screen_buffer_x=0;
};


static void del(void)
{
	if (screen_buffer_x) {
		screen_buffer_pos -= 2;
		screen_buffer_x--;
		//*(unsigned short *) shell_buffer_pos = 0x0720;
		//@todo: printchar
	}
};


//insere um caractere entro do buffer
void shellInsertCharXY(unsigned long x, unsigned long y, char c)
{
	unsigned long offset = (unsigned long) ((y*80) + x); 
	
	if(x>=80){
		return;
	}
	
	if(y>=25){
		return;
	}

	screen_buffer[offset] = (char) c;
};


//insere um caractere no buffer de output 
//#bugbug, talvez o buffer seja stdout. 
void shellInsertCharPos(unsigned long offset, char c)
{
	unsigned long offsetMax = (unsigned long)(80*25); 
		
	if(offset >= offsetMax){
		return;
	}
	
	screen_buffer[offset] = (char) c;
};


/*
//inserindo uma string em uma posi��o do buffer de sa�da.
void shellInsertStringPos( unsigned long offset, char *string );
void shellInsertStringPos( unsigned long offset, char *string )
{
    //@todo
};
*/

/*
 preenche todo o buffer de sa�da com char ou atributo
void shellFillOutputBuffer( char element, int element_type )
{
	
}
*/

//coloca um char na pr�xima posi��o do buffer
void shellInsertNextChar(char c)
{
	screen_buffer[screen_buffer_pos] = (char) c;	
};


void shellInsertCR()
{
   screen_buffer[screen_buffer_pos] = (char) '\r';	
};


void shellInsertLF()
{
    screen_buffer[screen_buffer_pos] = (char) '\n';	
};


void shellInsertNullTerminator()
{
    screen_buffer[screen_buffer_pos] = (char) '\0';	
};



/*
 * shellTestMBR:
 *     Testar a leitura de um setor do disco.
 *     Testaremos o setor do mbr.
 *
 */
void shellTestMBR()
{
	unsigned char buffer[512];
	
	enterCriticalSection(); 
	
	//read sector
	system_call( SYSTEMCALL_READ_LBA, 
	             (unsigned long) &buffer[0],  //address 
				 (unsigned long) 0,           //lba
				 (unsigned long) 0);
				 
    shellRefreshScreen();	
	exitCriticalSection();   
};



/*
 * move_to:
 *    Posicionamento dentro do buffer.
 */
void move_to( unsigned long x, unsigned long y )
{
	if( x > DEFAULT_BUFFER_MAX_COLUMNS ){
		return;
	}

	if( y > DEFAULT_BUFFER_MAX_ROWS ){
		return;
	}
	
	screen_buffer_x = x;
	screen_buffer_y = y;
	return;
};


//show shell info
void shellShowInfo()
{
	int PID, PPID;
	
    PID = (int) system_call( SYSTEMCALL_GETPID, 0, 0, 0);
	if( PID == (-1)){
	    printf("ERROR getting PID\n");	
	}
  
    PPID = (int) system_call( SYSTEMCALL_GETPPID, 0, 0, 0);
	if( PPID == (-1)){
	    printf("ERROR getting PPID\n");	
	}
  
	printf("Process info: PID={%d} PPID={%d} \n", PID, PPID );
	
	
	printf("shellMaxColumns={%d} \n", shellMaxColumns );
	printf("shellMaxRows={%d} \n", shellMaxRows );	
	
};

//metrics
void shellShowMetrics()
{
	unsigned long screen_width;
	unsigned long screen_height;
	unsigned long cursor_width;
	unsigned long cursor_height;
	unsigned long mouse_pointer_width;
	unsigned long mouse_pointer_height;
	unsigned long char_width;
	unsigned long char_height;	
	//...

	screen_width         = apiGetSystemMetrics(1);
	screen_height        = apiGetSystemMetrics(2);
	cursor_width         = apiGetSystemMetrics(3);
	cursor_height        = apiGetSystemMetrics(4);
	mouse_pointer_width  = apiGetSystemMetrics(5);
	mouse_pointer_height = apiGetSystemMetrics(6);
	char_width           = apiGetSystemMetrics(7);
	char_height          = apiGetSystemMetrics(8);
	//...
	
	
	printf("shellShowMetrics:\n");
	printf("screenWidth={%d} screenHeight={%d}\n",screen_width,screen_height);
	printf("cursorWidth={%d} cursorHeight={%d}\n",cursor_width,cursor_height);
	printf("mousepointerWidth={%d} mousepointerHeight={%d}\n", 
	        mouse_pointer_width,mouse_pointer_height);
	printf("charWidth={%d} charHeight={%d}\n",char_width,char_height);	
	//...
	
	
done:	
    printf("Done\n");	
	return;
};

//show system info
void shellShowSystemInfo()
{
	int ActiveWindowId;
	int WindowWithFocusId;
	
	//
	//Active
	ActiveWindowId = (int) APIGetActiveWindow();
	
	//valor de erro
	if( ActiveWindowId == (-1)){
	    printf("ERROR getting Active window ID\n");	
	}	
	printf("ActiveWindowId={%d}\n", ActiveWindowId );


	//
	// Focus.
	WindowWithFocusId = (int) APIGetFocus();
	
	//valor de erro
	if( WindowWithFocusId == (-1)){
	    printf("ERROR getting Window With Focus ID\n");	
	}	
	printf("WindowWithFocusId={%d}\n", WindowWithFocusId );	
	
};



/*
 * shell_write_to_screen:
 *     refresh de um ret�ngulo ??    
 */
/* 
void shell_write_to_screen( struct shell_screen_d *screen, 
                            struct shell_rect_d *region )
{
    //@todo	
};
*/

/*
 * @todo: Criar rotina de sa�da do shell.
void shellExit(int code);
void shellExit(int code){
	exit(code);
}
*/


/*
void die(char * str);
void die(char * str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}
*/



