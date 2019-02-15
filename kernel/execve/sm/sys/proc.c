/*
 * File: sm/sys/proc.c
 *
 *     System Procedure.
 *
 *     ****   Central de di�logo.   ****
 *
 * A qui ficar�o todos os di�logos para conversar com os m�dulos do kernel base.
 * Eles ser�o chamados de 'dialog' ou 'procedure'. 
 *
 * O PROCEDIMENTO NUNCA DEVE SER TROCADO, SEMPRE SER� O PROCEDIMENTO DO SISTEMA
 * QUANDO HOUVER UMA MENSAGEM ENVIADA PARA OUTRO PROCEDIMENTO, A MENSAGEM DEVE SER
 * COLOCADA NA ESTRUTURA DA JANELA ATIVA E AVISAR A THREAD E O PROCESSO SOBRE A MENSAGEM.
 * A THREAD PRINCIPAL DO PROGRAMA PEGAR� AMENSAGEM E PASARA PARA O SEU PROCEDIMENTO.
 *
 * Obs: As teclas de F1 � F5 poderiam testar rotinas referentes �s camadas
 *      respectivas. F1 testa rotinas de K1 e asem por diante.
 *      As rotinas de K0 seriam testadas de outra forma.
 *
 * Descri��o:
 *     Procedimento de janela default, em kernel mode.
 *     Trata mensagens de sistema, como WIN KEY.
 *     Todas as tarefas s�o tratadas por esse procedimento padr�o, 
 *     ao menos que inicializem seu pr�prio procedimento, ou um mix das 
 * duas coisas.
 *
 * OBS: 
 *     procedure.c est� em hal, porque recebe interven��es de hardware
 * n�o importando a arquitetura.
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2016 - 2018 New services.
 *     //...
 */ 
 

#include <kernel.h>


extern unsigned long key_status;
extern unsigned long escape_status;
extern unsigned long tab_status;
extern unsigned long winkey_status; 
extern unsigned long ctrl_status;
extern unsigned long alt_status;
extern unsigned long shift_status;
extern unsigned long capslock_status;
extern unsigned long numlock_status;
extern unsigned long scrolllock_status;


//
// Vari�veis internas.
//

//unsigned long procedureCurrentProcedure;
//...

//
// Prot�tipos de fun��es internas.
//

//Realiza testes diferentes usando o procedimento do sistema.
void procedureMakeTests();
void procedureLinkDriverTest(); // testando linkar um driver ao sistema operacional
void procedureWindowWithFocusTest();
void procedureGrid();





//=========================================
//f5
//para testar recursos da libc.
void
xxxtestlibc()
{
	FILE *f1;
	int ch_test;
	
		printf("\n xxxtestlibc: Open init.txt \n");
        
		//f1 = (FILE *) fopen ("init.txt","rb");   	
	      f1 = (FILE *) fopen ("GRAMADO TXT","rb");  
        if( f1 == NULL )
		{
			printf("fopen fail\n");
		}else{
			printf("fopen ok\n");
			
			//#bugbug
			//isso funcionou, entao fopen funciona, entao o problema esta' em fgetc
			//printf("show: %s @\n", f1->_base );
			
		    //Isso mostra que o arquivo foi carregado corretamente 
		    //na base esperada.
		    //printf("Show f1->_base: %s\n",f1->_base);
		
		    printf("stream info:\n");
		    printf("f1->_base: %x\n",f1->_base);
		    printf("f1->_ptr: %x\n",f1->_ptr);
		    printf("f1->_cnt: %d\n",f1->_cnt);			
			
		}
		

		
		//
		// #bugbug ... o fgetc n�o l� na estrutura esperada.
		//
		printf("Testing fgetc ... \n\n");
		while(1)
		{
			//#bugbug: page fault quando chamamos fgetc.
			//printf("1");
			ch_test = (int) fgetc (f1);
			//ch_test = (int) getc (f1); 
			
			if( ch_test == EOF )
			{
				printf("\n\n");
				printf("EOF reached :)\n\n");
				return;
				//goto exit_cmp;
				
			}else{
				//printf("+");
			    printf ("%c", ch_test);	
			};
		};
}
//====================================





void procTestF6()
{	 
	void *address = (void *) kmalloc (1024);
	
	unsigned char *buffer = (unsigned char *) address;

    /*
    //tentando carregar setores espec�ficos pra saber se a fun��o est� 
	//carregando quanquer setor.
	
	//read test (buffer, lba, rw flag, port number )
    pio_rw_sector ( (unsigned long) buffer, (unsigned long) 559, (int) 0x20, (int) 0 );	
	 
	printf("%s",buffer); 
	*/
	
	
	pio_rw_sector ( (unsigned long) buffer, (unsigned long) 559, (int) 0x30, (int) 0, (int) 1 );	
	 
	//printf("Signature: [ %x %x ] \n" , buffer[0x1FE], buffer[0x1FF] ); 
};


// ??
unsigned long 
ldisc_dialog( struct window_d *window, 
              int msg, 
			  unsigned long long1, 
			  unsigned long long2 ) 
{
    //# cancelada !!!
    return 0;	
}


// ??
unsigned long 
terminal_dialog( struct window_d *window, 
                 int msg, 
			     unsigned long long1, 
				 unsigned long long2 ) 
{
    //# cancelada !!!
    return 0;	
}


/*
 ***********************************************************************
 * system_procedure:
 *     Procedimento de janela da janela com o foco de entrada ... (edit box.)
 *     Procedimento de janela default.
 *     
 * OBS: Existe uma rela��o grande entre control menus e o procedimento do
 * do sistema. Pois o procedimento do menu deve passar boa parte das mensagens
 * para serem tratadas pelo procedimento do sistema. 
 *
 * ?? QUAL JANELA � AFETADA POR ESSE PROCEDIMENTO ??
 *    � IMPORTANTE OBSERVAR O HANDLE DE JANELA PASSADO VIA ARGUMENTO.
 *
 * OBS: ESSE PROCEDIMENTO � INVOCADO POR 'ldisc.c' NA ROTINA LINEDISCIPLINE(..)
 *      POR ENQUANTO EST� PASSANDO UM HANDLE DE JANELA NULO. 
 *
 */

unsigned long 
system_procedure ( struct window_d *window, 
                   int msg, 
				   unsigned long long1, 
				   unsigned long long2 ) 
{ 	
	// @todo: *importante:
	//        N�o rpecisa dar refresh_screen para todos os casos.
	//        Cada caso � diferente ... 
	//        ?? Quem deve chamar esse refresh dos elentos gr�ficos ??
	//        O aplicativo ?? acionando a flag atrav�s de ShowWindow por exemplo??
	
	//debug!
	//printf("system_procedure: msg={%d} long1={%d}\n", msg, long1);  
	
	void *buff;
	
	int AltStatus;
	int CtrlStatus;
	int ShiftStatus;
	//...
	
	//usado no refresh_rectangle
	//unsigned long saveX, saveY;
	

	//Get status.
	AltStatus = (int) get_alt_status(); 
	CtrlStatus = (int) get_ctrl_status();	
	//ShiftStatus = (int) get_shift_status();
	//...
	

	// Lidando com a janela com o foco de entrada.
	unsigned long left; 
	unsigned long top;   
	unsigned long width; 
	unsigned long height; 		

	
	//janela de teste.
	struct window_d *xxxx;
	
	//struct window_d *w;
	

	//
	// Checamos se a janela com o focod e entrada � v�lida.
	//
	
	//#bugbug isso est� errado ... devemos pegar a janela passada por argumento 
	//poi o line discipline selecionou a janela com o foco de entrada.
	
	//w = (void *) windowList[window_with_focus];
	
	// Window With Focus !
	//window � a janela com o foco de entrada, obtita pelo ldisc.c 
	//e passada via argumento.
	if ( (void *) window == NULL )
	{
	    
		printf("sm-sys-system_procedure: window");
        die();		
	
	}else{
	
	    //
	    // Configurando o cursor para ficar de acordo com a janela com o foco de entrda.
	    //
		
		//
		// @todo:
		// Aqui dever�amos apenas pegar o ponteiro para a estrutura 
		// de cursor que pertence a janela com o foco de entrada.
		//
	
	    //pegando as dimens�es da janela com o foco de entrada.

		//left   = window->left;
	    //top    = window->top;
	    //width  = window->width;
	    //height = window->height;		
	
	    //
		// simular valores aqui para teste ... como 80 25
		//
		
		//g_cursor_left   = (window->left/8);
		//g_cursor_top    = (window->top/8) + 4;   //Queremos o in�cio da �rea de clente.
		
		//g_cursor_right  = g_cursor_left + (width/8);
		//g_cursor_bottom = g_cursor_top  + (height/8);

		//100
		//a linha deve ser grande.
		//A linha pode ser maior que a janela.
		//g_cursor_right  = (800/8);
		//g_cursor_bottom = (600/8);

		
		//if( g_cursor_right == 0 ){
		//	g_cursor_right = 1;
		//}
		
        //if( g_cursor_bottom == 0 ){
		//	g_cursor_bottom = 1;
		//}		
		
		//cursor (0, mas com margem nova).
		//#bugbug ... isso reiniciaria o cursor a cada tecla pressionada.
		//g_cursor_x = g_cursor_left; 
		//g_cursor_y = g_cursor_top;  		
           
	
	    //...
	};

    //
    //*importante:
	// Desejamos que as teclas de controle sejam tratadas por esse 
	// procedimento mesmo que a janela seja do tipo terminal ...
	// pois as teclas de comtrole permite trocarmos a jenala com 
	// o foco de entrada sem fecharmos o terminal.
    //	
    
	switch (msg)
    { 
		//teclas de digita��o.
		case MSG_KEYDOWN:
            switch (long1)
            {	
				// [ ENTER ]
				case VK_RETURN:
				    
					//#test
					//printf("\r");
					//printf("\n");
					
					//input esta em stdio.c
					//#test
					//input( (unsigned long) long1);  
					goto done;
					break;
					
				//??isso deveria ser do sistema ??
				//usando o scape para fechar a janela com o foco de entrada.
                case VK_ESCAPE:
				    alt_status = 0;
					ctrl_status = 0;
					shift_status = 0;
					//Mensagem para fechar a janela passada via argumento.
					//SendMessage(window,MSG_DEVELOPER,1,0);
					//CloseWindow(window);
                    KillFocus(window);
					goto done;				
				    break;
					
				//Obs: N�o deve ser preocupa��o do desenvolvedor de aplica��o 
				//incrementar o cursor quando pressiona a seta.
				//por isso vou trat�las aqui.
				
				//# bugbug problemas no 'i'.
				
				//case VK_RIGHT:	
				//    g_cursor_x++;
				//	break;
				
				//case VK_LEFT:
				//    g_cursor_x--;
				//    break;
					
				//case VK_DOWN:
				//    g_cursor_y++;
				//    break;
					
				//case VK_UP:
				//    g_cursor_y--;
				//	break;
				
				

                //??quem deve tratar essa tecla ?? 
                //o tab deve fazer parte das teclas de difita��o. ??
                case VK_TAB:
					//if(AltStatus == 1){
                    //    //@todo: Chama uma rotina que muda a janela com o foco de entrada.						
					//	break;
					//};
					
					//#test
					//printf("\t");
					
					//#test
					//input( (unsigned long) long1);  
					goto done;
				    break;
                
				//??quem deve tratar essa tecla ??
				case VK_BACK:
				    
					//#test
					//g_cursor_x--;
					//printf(" ");
					
					//#test
					//input( (unsigned long) long1);  
					goto done;
                    break;
					
				case VK_PAUSE:
                    //#test
					//if(CtrlStatus == 1){
					//    KiInformation();	
					//}
					goto done;
					break;				
				   					
					//Se for do tipo terminal as teclas de digita��o se 
					//ser�o tratadas pelo procedimento de janelas do terminal.
					//para isso � s� ir para o fim desse procedimento.
					//if( window->terminal == 1 ){
					//    goto do_terminal;	
					//}
						
					//
					// *importante:
					// Podemos imprimir nesse momento, pois a impress�o est� correta e 
					// deixarmos o input sem imprimir. O que parece ser normal, input
					// apenas por dentro do buffer.
					//
						
					//printf deve imprimir no caso de tab ou espa�o...
					//input s� vai mexer com o buffer
	
	                // Isso funciona, porque printf incrementa o cursor antes 
					// de imprimir o char no backbuffer. Tem caso que ele 
					// manipula o cursor e n�o imprime nada.
						
					// printf deveria imprimir na janela com o foco de entrada.
					// obs: printf usa o cursor do sistema. Para imprimir na janela temos 
					// que auterar o cursor do sistema para ficar com as dimens�es da 
					// janela com o foco de entrada. shell.bin agradece.
					
					//#bugbug: Problemas com o cursor. Est� saltando a cada 
					//caractere digitado. Mas o refresh funciona bem.
					
					//teste
					//#IMPORTANTE: ISSO FOI CANCELADO PORQUE A LIB EM USER MODE 
					//AGORA IMPRIME OS CARACTERES E FAZ O REFRESH DO CHAR.
					
					//printf("%c", (char) long1);
		            //refresh_rectangle( g_cursor_x*8, g_cursor_y*8, 8, 8 );
						
			        //
					// input:
					// Devemos nos certificar que input n�o imprima nada.
					//
								//teclas de digita��o para o editbox.   
                
				default:	
					//#test
					//input( (unsigned long) long1);      //Coloca no stdin
					goto done;
                    break; 
            };		
			break;
          
		/* ## Teclas do sistema interceptadas pelo kernel ## */  
        case MSG_SYSKEYDOWN:                 
            switch (long1)	       
            {   
                //
				// As fun��es F1 � F12 s�o op��es para o desenvolvedor.
				//
				
				// Obs: Essa vari�veis de estatus ser�o vari�veis encapsuladas
				//      no driver de teclado. Para saber o valor delas
				//      tem que chamar uma fun��o do driver. keyboard.c
				//Obs: 
				// *Importante: Tem que chamar m�todo pra pegar vari�vel dentro de driver.
				
				//Help. 
				case VK_F1:	
					// #importante 
					// Mostraremos a ajuda do aplicativo.
					
					
					//1 = initialize.
					diskATADialog ( 1, FORCEPIO, FORCEPIO );
					show_ide_info ();
                    //rotina provis�ria de teste de read.
					//procTestF6();
					break;
					
				
				//Kernel info.	
                case VK_F2:
					//KiInformation();
					break;
				
	
                //CPU info.
                //intel? deveria ser independente. 				
                case VK_F3: 
					
					//show_cpu_intel_parameters();
					break;
					
				//Window tests.	
                case VK_F4:
					if(AltStatus == 1){ 
					    closeActiveWindow(); 
						alt_status = 0;
						goto done;
						break;
					};
					windowShowWindowList();  
					break;
				
                //#tests
				//Vamos usar F5 para testar as funcionalidades dos drivers apenas.	
				case VK_F5:

					printf("F5:\n"); 
                    
					//interna
					//no inci'cio do arquivo
					xxxtestlibc();
					
					//show_ide_info ();		 //execve/dd/disk1.c			
	                //show_ideports_info();    //execve/sm/disk/diskvol.c
					
					//#test testando inicializa��o do IDE, #funcionou.
					//ata_initialize();
				
					
					//
					// NIC test
					//
					//testNIC();					
					
					//init_nic ();  //intel.c (obs: essa rotina tambem reseta o controlador.)
					//refresh_screen();
					//testSend();
					//vamos testar a rotina de configura��o da transmiss�o.
					//nic_i8254x_transmit();
					//refresh_screen(); 
					//vamos mostrar informa��es antes obtidas pelo sistema.
					//show_network_info ();
					refresh_screen();

					//pci_info();     //PCI information.
				    					
					//systemShowDevicesInfo();
										
					break;
				
                //#testes
				//Testes diversos.	
				case VK_F6:
							
					printf("\n"); 
					testNIC();
					//refresh_screen();
					
					//#test
					//fsList("volume1");
					
					//testando listar os arquivos de um diret�rio que est� na mem�ria.
					//obs: isso funcionou.
					//fsFAT16ListFiles ( "directory default name:\n", 
					//    (unsigned short *) VOLUME1_ROOTDIR_ADDRESS,
					//	256 );
						
					//memoryShowMemoryInfo();
					
				    //vfsListFiles();
					//vfsShowVFSInfo();
					
					//habilitando o cursor.
					//timer_cursor_used = 1;
					//timer_cursor_status = 0;
					
					
				    //testingPageAlloc();
					
				    //init_clock(); //clock information
					//get_cmos_info();
					//printf( (const char*) stdout->_ptr );
				    
					//printf("F6: Testando carregar arquivo ...\n");
					//procedureMakeTests();
					
					//Obs: N�o usa janelas, isso n�o mudar� o foco.
					//windowShowWWFMessageBuffers();
					
					//printf("F6: Testando linkar um driver ...\n");
					//procedureLinkDriverTest();
					
					break;
				
                //Testing Message Box.				
				case VK_F7:
					break;
					
				// Cls. 
				// (reiniciar as configura��es originais)
                // Seta o foco na janela principal.				
				case VK_F8:
                    ldisc_init_modifier_keys();
					ldisc_init_lock_keys();
					videoInit();
					SetFocus( gui->main );
					
					//#test
					//init_mouse();
					//g_mousepointer_x = (unsigned long) 1;
		            //g_mousepointer_y = (unsigned long) 1;
					//mouse_x = 0;
					//mouse_y = 0;
					
					refresh_screen();
					goto done;	
					break;
				
                //Reboot.  				
				case VK_F9:
					systemReboot();
					break;				

				//Task Manager.	
				case VK_F10:
				    kill_all_threads();
					KiInformation();
					//current_idle_thread = 3;
					//if(AltStatus == 1){ window_with_focus = 10; break;};
					//if(CtrlStatus == 1){ active_window = 10; break;};
			        //if(ShiftStatus == 1){ printf("shift_F10\n"); break;};
				    
					//KiShowThreadList(); //threadi.c
					//mostra_slots();  //threadi.c
					
					//@todo: abrir o gerenciador de tarefas.
					//volumeShowVolumeInfo(0);
					//show_process_information();
					//show_thread_information();					
				    
					//aumentando gradativamente uma janela de pouco em pouco.
					//xxxx = (void*) windowList[6];
					//xxxx->width  -= 20;
					//xxxx->height -= 20;
					//resize_window(xxxx, xxxx->width, xxxx->height);
					//redraw_window(xxxx);					
					//refresh_screen();
					break;
					
				//Program manager.
                //@todo: usar F11 para FULL SCREEN.				
				case VK_F11:
				    windowSwitchFullScreen();
					goto done;
					break;
					
					
				//case VK_F12:
					//if(AltStatus == 1){ window_with_focus = 12; break;};
					//if(CtrlStatus == 1){ active_window = 12; break;};
				    //procedureGrid();
					
					//testando informa��e sobre os processos.
					//show_process_information();
					
					//testando informa��es sobre as threads.
					//show_thread_information();
					
					//Testando repintar todas as janelas seguindo a ordem list.
					//redraw_screen();
					
					//Teste de GC. 
					//Funcionando bem.
					//printf("F12: alocando ...\n");
					//buff = (void*) malloc(4*1024);
					//printf("F12: liberando ...\n");
					//free(buff);
					//printf("F12: testando GC ...\n");
					//gc();
					//printf("F12: OK\n");
					
 	
					//SystemMenu();
					//printf("@todo Control menu.\n");
					//@todo:control menu da area de trabalho.
					//MainMenu();
								
					//printf("time={%d}\n", get_time());
					//printf("date={%d}\n", get_date());
					
                    //Visualizando informa��es de mem�ria obtidas atrav�s do rtc.
					//memoryShowMemoryInfo();
					
					//Testando limites na cria��o de componentes do /microkernel.
					//Ok, funcionou bem.
					//microkernelTestLimit();
					
					//Testando Limites na utiliza��o de recursos do m�dulo pages.c.
					//??

					//Testando Limites na utiliza��o de recursos do m�dulo memory.c.
					//??
					
					//Testando Limites na utiliza��o de recursos do m�dulo window.c.
					//??
					
					//Testando imprimir mensagem na janela com o foco de entrada.
					//que no caso � a janela do desenvolvedor.
					//procedureWindowWithFocusTest();
					
					//drawScreenGrid();
					
					
					//repintar uma janela qualquer.
					
					
					//xxxx = (void*) windowList[4];
					//MaximizeWindow(xxxx);
					//redraw_window(xxxx);

					//xxxx = (void*) windowList[5];
					//replace_window(xxxx, 100, 100);
					//redraw_window(xxxx);

					//xxxx = (void*) windowList[6];
					//resize_window(xxxx, 100, 100);
					//redraw_window(xxxx);
				//	break;
				
				
				//#IMPORTANTE
				//caps lock keydown
				//#test testando a utiliza��o de mai�sculas,
				//j� que estamos usando o Windos como host e 
				//ele escreve no diret�rio com mai�sculas.
				case VK_CAPITAL:
				    break;
					
                // Nothing for now!  				
                case VK_LMENU:
				    break;
					
				// Nothing for now!	
                case VK_LCONTROL: 
					break;
                
				// Nothing for now!
				case VK_LSHIFT:   
				    break;
					
			    //Num Lock.	
		        case VK_NUMLOCK:
				    if(numlock_status == 1){
					    keyboard_set_leds(LED_NUMLOCK);  
					    break;
				    };
			        break;
					
			    //Scroll Lock.	
		        case VK_SCROLL:
				    if(scrolllock_status == 1){
		                keyboard_set_leds(LED_SCROLLLOCK);
					    break;
				    };
			        break;
					
                default: 
				    break;				
		    };              
        break;
		
		/* ## Teclas do sistema interceptadas pelo kernel ## */  
		
		case MSG_SYSKEYUP: 
            switch (long1)  
            {
				//0x5B.
                //Left WinKey system keyup. 
                //#super.
				//case VK_LGRAMADO:
                //    procedureGrid();  //Grid de bot�es usado pelo kernel.				
				//break;

                case VK_LWIN:
                case VK_RWIN:
				    if ( winkey_status == 0 )
					{
						//printf ("winkey\n");
					    //MessageBox (gui->screen, 1, "Win Key:","MSG_SYSKEYUP VK_LWIN or VK_RWIN");
                    };
					break;
					
				//test.
				//control menu. (application)
				//#importante:
				// ESSA MENSAGEM DEVE SER INTERCEPTADA PELO APLICATIVO.
				// ex: o shell.bin est� intereptando bem essa mensagem.
                case VK_CONTROL_MENU:
				    //control menu.
					//MessageBox(gui->screen, 1, "Control menu:","MSG_SYSKEYUP.VK_CONTROL_MENU");
                    break;
					
                default:
                    break;				
            };          
        break;
		

		// Essa categoria � para receber mensagens
        //enviadas para o console para gerenciamento do sistema.
        //como desligamentos, inicializa��es, reboot ...		
        //case MSG_CONSOLE:
		//case MSG_CONSOLE_COMMAND:
            //goto do_terminal;		
		//	break;
		 
        //Continua ... Create ... Close ...		
        //case MSG_CREATE:
		//    break;
		//case MSG_DESTROY:
	    //    break;
		//case MSG_CLOSE:
		    //#debug.
		    //Vamos tentar invalidar as janelas de di�logo.
			//Certamente n�o queremos fechar a janela gui->main.
	        //CloseWindow(window);
			//break;
		//case MSG_SETFOCUS:
	    //    break;
		//case MSG_KILLFOCUS:
	    //    break;
		//case MSG_PAINT:
	    //    break;
		
		//Dialogo especial:
		//Serve para fun��es provis�rias usadas pelo desenvolvedor 
		//do sistema operacional.
		case MSG_DEVELOPER:
		    switch(long1)
			{
				//fechar message box em kernel mode.
				case 1:
				    CloseWindow(window);
				break;
				
				default:
				break;
			}
			break;

			
	    //
		// Aqui provavelmente estamos com teclas de digita��o.
		// ent�o n�o precisamos efetuar o refresh_screen() deixando isso 
		// para o procedimento de janela do aplicativo
		//
		
	    //Nothing.
	    default:    
		    //return (unsigned long) 0;
			break;
	};
	
	
    
do_terminal:
	//if(VideoBlock.useGui == 1){
	//    refresh_screen();
	//};
    //return (unsigned long) 0;	
	
	//
	// Chama o procedimento da janela terminal.
	// Se ajanela n�o for uma janela do tipo terminal isso ir� retornar imediatamente.
	//
	//return (unsigned long) terminal_procedure( window, (int) msg, (unsigned long) long1, (unsigned long) long2 );
done:

   //
   // *importante:
   // Aqui devemos chamar o procedimento de janela da janela com o foco de entrada.
   // Pois bem, j� mandamos a mensagem para fila de mensagens da janela com o foco
   // de entrada, ent�o quando o aplicativo receber tempo de processamento ele ir�  
   // processar a mensagem.
   //
   

    return (unsigned long) 0;
};  


/*
 * registra_procedimento:
 * @todo: Essa fun��o deve ser modificada.
 *        O prop�sito dos argumentos � 
 *        receber o endere�o de algum procedimento
 *        que precisa ser selecionado como atual.
 *        o novo nome pode ser algo como:
 *        xxx_SetProcedure(,,,)   
 *
 * arg1 - next procedure.
 * arg2 - 0
 * arg3 - 0
 *
 */
unsigned long registra_procedimento( unsigned long arg1, 
                                     unsigned long arg2, 
									 unsigned long arg3, 
									 unsigned long arg4 )
{
	SetProcedure(arg1);
	return (unsigned long) 0;
};


/*
 * SetProcedure:
 *     Determina qual vai ser o procedimento atual.
 *     Esse deve ser o procedimento da janela com o foco de
 *     entrada. N�o necessariamente precisa jer a janela ativa,
 *     pois pode ser uma janela filha que esteja com o foc de entrada.
 *     Ou um �cone. Pois �cone tamb�m tem procedimento e recebe mensagens.
 *     Mas as mensagens enviada para �cones s�o diferentes das mensagens
 *     enviadas para janelas.
 */
void SetProcedure (unsigned long proc){
	
	//@todo: Checar limites.
	if( (unsigned long) proc == 0 ){
		return; 
	};
	

	// A estrutura da janela com o foco de entrada pode ser
	// atualizada nesse momento. Ou ao menos conferir
	// se o endere�o enviado por argumento corresponde
	// com o endere�o do procedimento da janela com o foco de entrada.
	
    g_next_proc = (unsigned long) proc;	
	
	//return;
};


/*
 * SendMessage:
 *     Envia mensagem para o procedimento da janela
 *     indicada no argumento.
 *     � bem prov�vel que a janela enviada por argumento
 *     seja a janela com o foco de entrada.
 *     
 *     @todo: Esse pode ser o formato padr�o e envio de mensagens.
 *            Os processos podem trocar mensagens desse tipo
 *            atrav�z de uma estrutura que contenha esses quatro 
 *            argumentos na forma de par�metros. 
 */
unsigned long SendMessage( struct window_d *window, 
                  int msg, 
				  unsigned long long1, 
				  unsigned long long2 )
{
	return (unsigned long) system_procedure(window,msg,long1,long2);
};




/*
 *********************************************
 * procedureHelp:
 *     Mensagem de ajuda ao usu�rio.
 */
void procedureHelp (){
	
	struct window_d *hWindow; 		
	
	unsigned long left;
	unsigned long top;    
	unsigned long width;  
	unsigned long height; 	
	
	if( VideoBlock.useGui != 1 )
	{
		printf("procedureHelp: no GUI\n");
		goto fail;
	}	
	
	//Parent window.
	
	if( (void*) gui->main == NULL )
	{
		printf("procedureHelp: parent window fail\n");
	    goto fail;
	}else{
		

	    left = 20;  // gui->main->left;
	    top = 20;  // gui->main->top;
	    
		width = 240; // gui->main->width;
	    height = 200; // gui->main->height;		
		
		//...

	    g_cursor_x = (left/8);
	    g_cursor_y = (top/8); 
		
	};
	

	//backgroundDraw(COLOR_BACKGROUND);	
	
	//
	// ## Window ##
	//	
	
	//VIEW_MAXIMIZED
	//Create.
	hWindow = (void*) CreateWindow( 3, 0, VIEW_NORMAL, "procedureHelp", 
	                  left, top, width, height, 
					  gui->main, 0, 
					  KERNEL_WINDOW_DEFAULT_CLIENTCOLOR, 
					  KERNEL_WINDOW_DEFAULT_BGCOLOR );
					  
	if( (void*) hWindow == NULL )
	{
	    printf("procedureHelp: hWindow\n");
		goto fail;
    }else{
		RegisterWindow(hWindow);
        
		//Coloca as mensagens na janela.	
	    //Somente se tivermos uma janela v�lida. 
	
	    draw_text( hWindow, 8,  2*(height/20), 
	        COLOR_WINDOWTEXT, "F1 Help.");
        draw_text( hWindow, 8,  3*(height/20), 
	        COLOR_WINDOWTEXT, "F2 Kernel info.");
	    draw_text( hWindow, 8,  4*(height/20), 
	        COLOR_WINDOWTEXT, "F3 CPU info.");
	    draw_text( hWindow, 8,  5*(height/20), 
	        COLOR_WINDOWTEXT, "F4 Window tests.");
	    draw_text( hWindow, 8,  6*(height/20), 
	        COLOR_WINDOWTEXT, "F5 Device info.");
	    draw_text( hWindow, 8,  7*(height/20), 
	        COLOR_WINDOWTEXT, "F6 Clock info.");
	    draw_text( hWindow, 8,  8*(height/20), 
	        COLOR_WINDOWTEXT, "F7 MessageBox.");
	    draw_text( hWindow, 8,  9*(height/20), 
	        COLOR_WINDOWTEXT, "F8 Cls.");
	    draw_text( hWindow, 8, 10*(height/20), 
	        COLOR_WINDOWTEXT, "F9 Reboot.");
	    draw_text( hWindow, 8, 11*(height/20), 
	        COLOR_WINDOWTEXT, "F10 Task Manager.");
	    draw_text( hWindow, 8, 12*(height/20), 
	        COLOR_WINDOWTEXT, "F11 Program manager.");
        draw_text( hWindow, 8, 13*(height/20), 
	        COLOR_WINDOWTEXT, "F12 Tests");	
	
	    //...
	};

		
    //#bugbug:
    //O que queremos � apenas atualizar as strings
    //na status bar existente en�o criar uma status bar.    
	//UpdateStatusBar( hWindow, "Esc=EXIT", "Enter=?" );
	
	
	//
	// @todo: Habilitar o procedimento de janela.
	//
	
	
	
	goto done;
		
		
fail:		
    printf("fail\n");
done:
    SetFocus(hWindow);
   
	//@todo: 
	//Devemos dar o refresh somente da janela.
	refresh_screen();
    return;
};



/*
 * procedureLinkDriverTest:
 *     Testando linkar um driver ao sistema operacional.
 *     Obs: Essa rotina � um tipo de callout. Foi implementada usando um
 * iret, mas poderia bem ser um jmp.
 * Obs: Isse test  funcionou bem, implementar essa rotina definitivamente.
 */
void procedureLinkDriverTest()
{
	//
	// Saltando para a thread idle:
	//
	
	//
	// Obs: Estamos for�ando. estamos atropelando o escalonamento
	// e thread e saltando para a thread idle para testar a nossa tentativa
	// de inicializar o processo idle tambem com driver, alem de ser um processo cliente.
	//
	
	//
	// estamos usando o mesmo m�todo que saltamos para a thread idle pela primeira vez.
	//
	
	
	//
	// @todo: Poderiamos ter aqui op��es de filtro, que faria a rotina falhar e retornar 
	// no caso de reprovar alguma coisa.
	//
	//goto: failReturn;
	
	printf("iret\n");
	
    asm volatile(" cli \n"   
                 " mov $0x23, %ax  \n" 
                 " mov %ax, %ds  \n"
                 " mov %ax, %es  \n"
                 " mov %ax, %fs  \n"
                 " mov %ax, %gs  \n"
                 " pushl $0x23            \n"    //ss.
                 " movl $0x0044FFF0, %eax \n"
                 " pushl %eax             \n"    //esp.
                 " pushl $0x3200          \n"    //eflags.
                 " pushl $0x1B            \n"    //cs.
                 " pushl $0x00401000      \n"    //eip. 
				 
				 " mov $1234, %edx  \n"          //Obs: Acrescentamos essa flag.
												 
                 " iret \n" );
				 
				 
//failReturn:
    return;	
};


// Fun��o interna para realiza testes usando o procedimento do sistema.
void procedureMakeTests()
{
	
     //
	 // Outro teste.
	 //
	 
	/*
    printf("Testando desalocar..\n");	
	void *buff;
	buff = (void*) malloc( sizeof(32) );
    //~~~~
	//show_memory_structs();//mostra apos alocar
	//~~~~
	free(buff); //zera o penultimo indice da lista.
	//~~~~
	buff = (void*) malloc( sizeof(32) );
	//...
	show_memory_structs();//mostra apos desalocar
	//~~~~	
	return;
	*/
	
	
	//
	// @todo: #bugbug: A interruo��o de timer bagun�a tudo.
	//
	
	
	//unsigned long buff;
	// Desabilita taskswitch
	
	taskswitch_lock();
	scheduler_lock();	
	
	printf("sm-sys-procedureMakeTests:\n");
	
	//Fluxo padr�o. (file structure)
	stdout = (void *) malloc( sizeof(FILE) );
	
	if ( (void *) stdout != NULL )
	{
		//File size.
		//@todo: Fazer assim.
		//stdout->_ptr = (char *) malloc( 4096 );
		stdout->_ptr = (char *) malloc( sizeof(4096) );
		
		if(stdout->_ptr == 0)
		{
			printf("erro 1\n");
			goto done;
		}
				
		fsLoadFile ( VOLUME1_FAT_ADDRESS, VOLUME1_ROOTDIR_ADDRESS, 
		    "INIT    TXT", (unsigned long) stdout->_ptr );
			
		printf("%s\n", (const char*) stdout->_ptr);		
		
		//printf("~~%s \n",stdout->_ptr);
		//free(stdout->_ptr);
		goto done;
		//refresh_screen();
		//return;
		//while(1){}
	
	} else {
		
		//printf("fail.\n");
		printf("erro2\n");
		goto done;
		//refresh_screen();
		//return;
		//while(1){}
	};
	
	

	
done:

    //
	// Testando listar os arquivos.
	//
    
	//fsFAT16ListFiles(,,);

	printf("sm-sys-procedureMakeTests: done\n");
    //Reabilita task switch.
	scheduler_unlock();
	taskswitch_unlock();



	
	//refresh_screen();
	//while(1){}
	return;
}

/*
 * Configura o procedimento atual.
void procedureSetProcedure(unsigned long address)
{};
*/

/*
 * Obtem o procedimento atual.
void procedureGetProcedure()
{
    return (unsigned long) x;	
};
*/

/*
 * Invocar um procedimento que est� em user mode
 e pertence � um processo cliente.
 Invocar da mesma forma que o kernel inicializa um driver,
 atrav�s de iret diretamente na rotina.
 o procedimento de janel retornar� utilizando uma system call.
 
void *procedureInvokeUserModeProcedure()
void *procedureInvokeUserModeProcedure()
{
	
}
*/


void procedureWindowWithFocusTest()
{
	//
	// ****  TESTANDO A JANELA DO desenvolvedor ****
	// ****  SER� A �NICA JANELA EM PRIMEIRO PLANO POR ENQUANTO ****
	// **** A �NICA COM O FOCO DE ENTRADA  ****
	//
					
	//
	// #bugbug ... essa rotina est� travando ...
	//
					
	//escrevendo fora da janela
	printf("F12:Fora.\n");
	//refresh_screen();
					
	if( (void*) WindowWithFocus == NULL ){
	    printf("F12: WindowWithFocus");
	    refresh_screen();
		while(1){}
	}

	if( (void*) WindowWithFocus != gui->DEVELOPERSCREEN ){
		printf("F12: WindowWithFocus != gui->DEVELOPERSCREEN");
		refresh_screen();
		while(1){}
	}
					
	//Esperamos que a ultima janela a receber o foco tenha sido
	//a janela do desenvolvedor...
	//
	WindowWithFocus->cursor_x = 0;
	WindowWithFocus->cursor_y = 0;
	//WindowWithFocus->left  = 0;
	//WindowWithFocus->top = 0;
					
	g_cursor_x = (unsigned long) (WindowWithFocus->left);
	g_cursor_y = (unsigned long) (WindowWithFocus->top);
	printf("F12: [left/top]\n");

	//g_cursor_x = (unsigned long) (WindowWithFocus->right);
	//g_cursor_y = (unsigned long) (WindowWithFocus->top);
	//printf("F12: [right/top]\n");

	//g_cursor_x = (unsigned long) (WindowWithFocus->left);
	//g_cursor_y = (unsigned long) (WindowWithFocus->bottom);
	//printf("F12: [left/bottom]\n");

	//g_cursor_x = (unsigned long) (WindowWithFocus->right);
	//g_cursor_y = (unsigned long) (WindowWithFocus->bottom);
	//printf("F12: [right/bottom]\n");
	
	//...
	
	return;				
};


/* @todo; Criar um grid parecido com o outro, mas que ser� gerenciado pelo
 procedimento de janelas do sistema.
*/ 
void procedureGrid (){
	
	int Status;
	
	Status = grid( (struct window_d*) gui->main ,(int) 4, (int) GRID_HORIZONTAL ); 
	
	if(Status == 1){
		printf("sm-sys-procedureGrid: grid\n");
	}
	
	//return;
};



//
// End.
//

