/*
 * File: ldisc.c
 *
 * Descri��o:
 *    Esse ser� o gerenciador de Line Discipline.
 *    Ficar� dentro do kernel base e receber� as entradas 
 * dos dispositivos de caractere e enviar� para as filas apropriadas.
 *    Por enquanto os scancodes de teclado s�o tratados e enviados 
 * para a fila de mensagem da janela apropriada. Principalmente a janela 
 * com o foco de entrada. 
 *
 */


// #todo
// A disciplina de linhas poderia chamar o procedimento de janelas padr�o
// do gramado x-server ao inv�s de chamar o procedimento de janelas do sistema.


// #importante
// Essas rotinas est�o ligadas as formas de input.
// seus poss�veis lugares s�o: 
// + line discipline
// + x server
// + terminal virtual.

#include <kernel.h>


/*
 * LINE_DISCIPLINE
 *     Uma disciplina de linha gen�rica, para mensagens de 
 * dispositivos de entrada diferentes.
 */

int 
LINE_DISCIPLINE ( struct window_d *window, 
                  int msg, 
				  unsigned long long1, 
				  unsigned long long2 )
{

	struct thread_d *t;		
	struct window_d *w;  
	
	//
	// ## window ##
	//
	
	// #importante
    // +Pegamos a janela com o foco de entrada, pois ela 
    // ser� um elemento da mensagem.	
	// Mas enviaremos a mensagem para a fila da thread atual.

	//#todo mensagem de erro.	
	
	if (window_with_focus < 0)
		return -1;
	
	w = (void *) windowList[window_with_focus];
	
	if ( (void *) w == NULL )
	{
		printf ("LINE_DISCIPLINE: w");
		die();
		
	}else{
		
		if ( w->used != 1 || w->magic != 1234 )
		{
			printf("LINE_DISCIPLINE: w validation");
			die();
		}		

		//
		// ## thread ##
		//
		
		//#importante:
		//Pegamos a thRead de input associada com a janela 
		//que tem o foco de entrada.
		
		t = (void *) w->control;
		
		if ( (void *) t == NULL )
		{
		    printf("LINE_DISCIPLINE: t");
		    die();			
		}
		
		if ( t->used != 1 || t->magic != 1234 )
		{
			printf ("LINE_DISCIPLINE: t validation \n");
			die ();
		}        
			
		//#importante:
		//??
		
		//a janela com o foco de entrada deve receber input de teclado.
		//ent�o a mensagem vai para a thread associada com a janela com o foco de 
		//entrada.
		//#importante: a rotina que seta o foco dever� fazer essa associa��o,
		//o aplicativo chama a rotina de setar o foco em uma janela, 
		//o foco ser� setado nessa janela e a thread atual ser� associada 
		//a essa janela que est� recebendo o foco.
		
		//??
		//ja o input de mouse deve ir para a thread de qualquer janela.
		
		t->window = window;
		t->msg = (int) msg;
		t->long1 = long1;
		t->long2 = long2;
		
		t->newmessageFlag = 1;
		
		
        //
		// system procedure
		//
		
		system_procedure ( window, (int) msg, 
		    (unsigned long) long1, (unsigned long) long2 );
		
	};	
 	
    return 0;	
}


/*
 ***************
 * KEYBOARD_LINE_DISCIPLINE
 *     Uma disciplina de linha apenas para as digita��es de teclado.
 * Funciona como um filtro.
 * Obs: Essa � a rotina principal desse arquivo, todo o resto poder� 
 * encontrar um lugar melhor.
 */
	
// #importante
// A disciplina de linha referente ao input de teclado pode
// estar presente no x server. Ainda mais que lida com janelas.


int KEYBOARD_LINE_DISCIPLINE ( unsigned char SC ){
	
	struct thread_d *t;		
	struct window_d *w;  	
	
    //
    // Step 0 - Declara��es de vari�veis.
    //

    //Vari�veis para tecla digitada.
    unsigned char scancode;
    unsigned char key;         //Tecla (uma parte do scancode).  
	
	int message;               //arg2.
    unsigned long ch;          //arg3 - (O caractere convertido para ascii).
    unsigned long status;      //arg4.  

    //
    // Step1 - Pegar o scancode.
    //

    //O driver pegou o scancode e passou para a disciplina de linha 
    //atrav�s de par�metro.	
	
	scancode = SC;
	

	//Obs: Observe que daqui pra frente todas as rotinas poderiam estar
	//     em user mode.


	//Debug stuff.
    //Show the scancode if the flag is enabled.	
	
	if (scStatus == 1){
		
	    kprintf ("{%d,%x} ", scancode, scancode );
	}
	
    //
    // Step 2 - Tratar as mensagens.
    //

    //Se a tecla for (liberada).
	//D� '0' se o bit de paridade for '0'.
	
    if( (scancode & LDISC_KEY_RELEASED) == 0 )
	{
		//Desativando o bit de paridade caso esteja ligado.
		
	    key = scancode;
		key &= LDISC_KEY_MASK;    

		//Configurando se � do sistema ou n�o.
		//@todo: Aqui podemos chamar uma rotina interna que fa�a essa checagem.
		
		switch (key)
		{
			//Os primeiros 'case' � quando libera tecla do sistema.
			//O case 'default' � pra quando libera tecla que n�o � do sistema.
			
			//left Shift liberado.
			case VK_LSHIFT:
				shift_status = 0;
				message = MSG_SYSKEYUP;
			    break;

			//Left Control liberado.			
			case VK_LCONTROL:
				ctrl_status = 0;
				message = MSG_SYSKEYUP;
				break;

			//left Winkey liberada.
			case VK_LWIN:
			    winkey_status = 0;
                message = MSG_SYSKEYUP;
				break;

			//Left Alt liberado.
            case VK_LMENU:
				alt_status = 0;
				message = MSG_SYSKEYUP;
			    break;				
			
			//right winkey liberada.
			case VK_RWIN:
			    winkey_status = 0;
                message = MSG_SYSKEYUP;
				break;

			//control menu.
			case VK_CONTROL_MENU:
			    //controlmenu_status = 0; //@todo
			    message = MSG_SYSKEYUP;
			    break;

            //right control liberada.
			case VK_RCONTROL:
				ctrl_status = 0;
				message = MSG_SYSKEYUP;
				break;
				
			//right Shift liberado.
			case VK_RSHIFT:
				shift_status = 0;
				message = MSG_SYSKEYUP;
			    break;

			//Fun��es liberadas.
            case VK_F1:
            case VK_F2:
            case VK_F3:
            case VK_F4:
            case VK_F5:
            case VK_F6:
            case VK_F7:
            case VK_F8:
            case VK_F9:
            case VK_F10:
            case VK_F11:
            case VK_F12:
			    message = MSG_SYSKEYUP;
			    break;

			//...
				
			//A tecla liberada N�O � do sistema.
			default:
			    message = MSG_KEYUP;
				break;
		};

		//Selecionando o char para os casos de tecla liberada.

		//Analiza: Se for tecla normal, pega o mapa de caracteres apropriado.
		//min�scula
		//Nenhuma tecla de modifica��o ligada.
		if (message == MSG_KEYUP)
		{
			ch = map_abnt2[key];
			goto done;
		};
		
        //Analiza: Se for do sistema usa o mapa de caracteres apropriado. 
		//Normal.
   		if (message == MSG_SYSKEYUP)
		{
			ch = map_abnt2[key];
		};
		
        //Nothing.
		goto done;
	};
	
	
	// * Tecla (pressionada) ...........	
	if( (scancode & LDISC_KEY_RELEASED) != 0 )
	{ 
		key = scancode;
		key &= LDISC_KEY_MASK; //Desativando o bit de paridade caso esteja ligado.

		//O �ltimo bit � zero para key press.
		//Checando se � a tecla pressionada � o sistema ou n�o.
		//@todo: Aqui podemos chamar uma rotina interna que fa�a essa checagem.
		
		switch (key)
		{
			//back space ser� tratado como tecla normal
			
			//@todo: tab,
            //?? tab ser� tratado como tecla normal por enquanto.
			
			//caps lock keydown
			case VK_CAPITAL:
			    //muda o status do capslock n�o importa o anterior.
				if (capslock_status == 0)
				{ 
				    capslock_status = 1; 
					message = MSG_SYSKEYDOWN; 
					break; 
				};
				if (capslock_status == 1)
				{ 
				    capslock_status = 0; 
					message = MSG_SYSKEYDOWN; 
					break; 
				};
				break; 

			//Left shift pressionada.
			case VK_LSHIFT:
			//case KEY_SHIFT:
				shift_status = 1;
				message = MSG_SYSKEYDOWN;
			    break;

			//left control pressionada.
			//case KEY_CTRL:
			case VK_LCONTROL:
				ctrl_status = 1;
				message = MSG_SYSKEYDOWN;
				break;

			//Left Winkey pressionada.
			case VK_LWIN:
			    winkey_status = 1;
				message = MSG_SYSKEYDOWN;
				break;

            //left Alt pressionada.
            case VK_LMENU:
				alt_status = 1;
				message = MSG_SYSKEYDOWN;
			    break;

			//@todo alt gr.	

			//Right Winkey pressionada.
			case VK_RWIN:
			    winkey_status = 1;
				message = MSG_SYSKEYDOWN;
				break;
			
            //@todo: Control menu.
            
			//Right control pressionada.
			case VK_RCONTROL:
				ctrl_status = 1;
				message = MSG_SYSKEYDOWN;
				break;

			//Right shift pressionada.
			case VK_RSHIFT:
				shift_status = 1;
				message = MSG_SYSKEYDOWN;
			    break;


            case VK_F1:
            case VK_F2:
            case VK_F3:
            case VK_F4:
            case VK_F5:
            case VK_F6:
            case VK_F7:
            case VK_F8:
            case VK_F9:
            case VK_F10:
            case VK_F11:
            case VK_F12:
			    message = MSG_SYSKEYDOWN;
			    break;


			//Num Lock.	
		    case VK_NUMLOCK:
			    //muda o status do numlock n�o importa o anterior.
				if (numlock_status == 0)
				{
		            numlock_status = 1;
					message = MSG_SYSKEYDOWN;
					break;
				};
				if (numlock_status == 1)
				{ 
				    numlock_status = 0;
                    message = MSG_SYSKEYDOWN; 					
					break; 
				};
			    break;
				
			//Scroll Lock.	
		    case VK_SCROLL:
			    //muda o status do numlock n�o importa o anterior.
				if(scrolllock_status == 0)
				{
		            scrolllock_status = 1;
					message = MSG_SYSKEYDOWN;
					break;
				};
				if(scrolllock_status == 1){ 
				    scrolllock_status = 0;
                    message = MSG_SYSKEYDOWN; 					
					break; 
				};
			    break;

            //...

			//A tecla pressionada n�o � do sistema.
			
			default:
			    message = MSG_KEYDOWN;
				//printf("keyboard debug: default: MSG_KEYDOWN\n");
				break;
		};

		//uma tecla normal foi pressionada.
		//mensagem de digita��o.				
		if (message == MSG_KEYDOWN)
		{
            //mai�sculas.			
			if (capslock_status == 1)
			{
			    ch = shift_abnt2[key];
			    goto done;
			}

			//min�sculas.
			if (capslock_status == 0)
			{
		        ch = map_abnt2[key];
			    goto done;
			};
			
			//#todo
			// fomos avisados que se trata de uma scape sequence para teclas extras
			// do teclado estendido. Temos que pegar o scancode de outro mapa.
			
			//if ( ESCAPE_E0 == 1 && numlock_status == 1 )
			//{
		    //    ch = numlock_abnt2[key];
			//    ESCAPE_E0 = 0;
			//	goto done;			
			//}
			
            //Nothing.
		};
		
		//uma tecla do sistema foi pressionada.
		if (message == MSG_SYSKEYDOWN)
		{
			ch = map_abnt2[key];
            goto done;
		};		
		
		//Nothing.
		goto done;
	};//fim do else

    //
	// ## Done ##
	//
	
	// Para finalizar, vamos enviar a mensagem para fila certa.
	
done:

	//
	// Control + Alt + Del.
	//

		//Op��es:
		//@todo: Chamar a interface do sistema para reboot.
		//@todo: Op��o chamar utilit�rio para gerenciador de tarefas.
		//@todo: Abre um desktop para opera��es com usu�rio, senha, logoff, gerenciador de tarefas.

		//Chamando o m�dulo /sm diretamente.
		//mas n�o � o driver de teclado que deve chamar o reboot.
		//o driver de teclado deve enviar o comando para o console, /sm,
		//e o console chama a rotina de reboot do teclado.
		//Uma mensagem de reboot pode ser enviada para o procedimento do sistema.
		//Pois o teclado envia mensagens e n�o trata as mensagens.
	
	    //Um driver n�o deve chamar rotinas de interface. como as rotians de servi�o.	
		//A inten��o � que essa mensagem chegue no procedimento do sistema.
		//Porem o sistema tambem deve saber quem est� enviando esse pedido.@todo.
		//@todo: podemos O reboot pode ser feito atrav�s de um utilit�rio em user mode.
			
	
		//Uma op��o aqui, � enviar para o aplicativo uma mensagem de reboot.
		//como o aplicativo n�o trata esse tipo de mensagem ele apenas reecaminha 
		//para o procedimentod e janelas do sistema.
	
		
		//system_procedure ...
		// @todo: Chamar o aplicativo REBOOT.BIN.
	
	if ( (ctrl_status == 1) && (alt_status == 1) && (ch == KEY_DELETE) )
	{
		// #todo: 
		// Chamar outra rotina de reboot.
		// Se o driver estiver em user mode, tem que fazer uma chamada ao sistema.
		
		//gde_services ( SYS_REBOOT, 0, 0, 0 );
	}

	
	// Nesse momento temos duas op��es:
	// Devemos saber se a janela com o foco de entrada � um terminal ou n�o ...
	// se ela for um terminal chamaremos o porcedimento de janelas de terminal 
	// se ela n�o for um terminal chamaremos o procedimento de janela de edit box. 
	// que � o procedimento de janela do sistema.
	// *IMPORTANTE: ENQUANTO O PROCEDIMENTO DE JANELA DO SISTEMA TIVER ATIVO,
	// MUITOS COMANDOS N�O V�O FUNCIONAR ATE QUE SAIAMOS DO MODO TERMINAL.
	
	// *importante:
	// Passamos a mensagem de teclado para o procedimento de janela do sistema.
	// que dever� passar chamar o procedimento de janela da janela com o focod eentrada.
		
	// *importante:
	// Quem � o 'first responder' para evento de teclado.
	// A janela com o foco de entrada � o first responder para 
	// eventos de teclado, mas n�o para todo tipo de envento.		

	
	// ## window ##
	
	// #importante
	// +Pegamos a janela com o foco de entrada, pois ela ser� um elemento 
	// da mensagem.	
	// Mas enviaremos a mensagem para a fila da thread de input associada
	// a essa janela.
	
	// ### super importante ###
    // Estamos enviando esse input para a thread de input de uma janela.
	// Mas poder�amos por padr�o mandar para uma thread espec�fica do
	// servidor de recursos gr�ficos, x server, ele por sua vez
	// envia a mensagem para o servidor de janelas que mandar� para
	// thread de input associada � janela com o foco de entrada.
	
	// >> kbd driver >> ldisc >> x server >> wm >> thread.

	
	// #importante
	// Caso tenhamos um servidor x-server carreg�vel e funcionando
	// ent�o podemos mandar a mensagem para a thread de controle dele.
	// Essa mensagem ser� armazenada. O wm chamar� o servidor e pegar� essa
	// mensagem.
	
	//if ( x_server_status == 1)
	//{
	//	t = x_server_thread;
	//}
	
	
check_WindowWithFocus:	
	
	// #importante
	// Mas como o wm j� est� presente aqui no kernel, ent�o
	// j� podemos enviar para o window manager. Ou melhor
	// para a thread de input associada a janela com o foco.
	
	
	w = (void *) windowList[window_with_focus];
	
	if ( (void *) w == NULL )
	{
		printf ("KEYBOARD_LINE_DISCIPLINE: w");
		die ();
		
	}else{
			
		if ( w->used != 1 || w->magic != 1234 )
		{
			printf ("KEYBOARD_LINE_DISCIPLINE: w validation");
			die ();
		}		
		
		// #importante:
		// Aqui significa que temos uma janela com o foco de entrada v�lida.
		
		// #importante:
		// Pegamos a THREAD de input associada com a janela que tem o 
		// foco de entrada.
		
		// #bugbug
		// Quando um shell executa outro programa, esse novo
		// programa, ent�o a thread de controle desse novo programa
		// n�o est� associada a nenhuma janela com foco de entrada.
		// Ent�o essa nova thread de controle n�o recebe mensagens
		// pois precisamos de uma janela com foco de entrada.
		
		// talvez seja trabalho da libc inicialziar p input
		// talvez utilizando a janela do terminal
		// sem terminal, sem input.
		
		t = (void *) w->control;
		
		if ( (void *) t == NULL )
		{
		    printf ("KEYBOARD_LINE_DISCIPLINE: t \n");
		    die ();			
		}
		
		if ( t->used != 1 || t->magic != 1234 )
		{
			printf ("KEYBOARD_LINE_DISCIPLINE: t validation \n");
			die ();			
		}        

		//
		//  ## Message ##
		//

		
		// #importante:
		// A janela com o foco de entrada deve receber input de teclado.
		// Ent�o a mensagem vai para a thread associada com a janela que tem 
		// o foco de entrada.		
		// Como o scancode � um char, precisamos converte-lo em unsigned long.
		
		unsigned long tmp;
				
		tmp = (unsigned long) scancode;
		tmp = (unsigned long) ( tmp & 0x000000FF );
		
		t->window = w;
		t->msg = message;
		t->long1 = ch;     // Key.
		t->long2 = tmp;    // Scan code.
			
		t->newmessageFlag = 1;

		// F5 F6 F7 F8		
		// Teclas para teste.
		// Teclas usadas exclusivamente pelo 
		// procedimento de janelas do sistema.
		// Os aplicativos n�o devem usar essas teclas por enquanto.
		// Ent�o Essas teclas funcionar�o mesmo que os aplicativos estejam com problema.
		
	    switch (message)
		{		
			case MSG_SYSKEYDOWN:  
			    switch (ch){
					case VK_F5:	
					case VK_F6:	
					case VK_F7:	
					case VK_F8:	
						system_procedure (  w, (int) message, 
					        (unsigned long) ch, (unsigned long) tmp );					
						break;
				}
			    break;
		};
	};	
 
    return 0;
}

 
// Mouse #todo
// A entrada � um ponteiro para um buffer que contenha os 
// 3 chars usados pelo mouse.

// #importante
// A disciplina de linha referente ao input de teclado pode
// estar presente no x server. Ainda mais que lida com janelas.

int MOUSE_LINE_DISCIPLINE ( void *buffer ) {
	
	if ( (void *) buffer == NULL )
	    return (int) -1;
    
	unsigned char *chars = (unsigned char *) buffer;

	char char0 = (char) chars[0];
	char char1 = (char) chars[1];
	char char2 = (char) chars[2];
	char char3 = (char) chars[3];
	//...
	
	//
	// #todo
	//
	
    return (int) -1;   
}


//ldisc
//inicializando a fila de mensagens do sistema
//com ponteiros para estruturas de mensagens ...
//es estruturas ser�o reutiliz�veis.

void initialize_system_message_queue (void){
	
	struct message_d *m;
	
	int i;
	
	for ( i=0; i<SYSTEM_MESSAGE_QUEUE_MAX; i++ )
	{
		
		m = (void *) malloc ( sizeof(struct message_d) );
		
		if ( (void *) m == NULL )
		{
			printf("unblocked-ldisc-initialize_system_message_queue:");
			die();
		}else{
			
			m->objectType = ObjectTypeMessage;
			m->objectClass = ObjectClassKernelObjects;
		    m->used = 1;
			m->magic = 1234;
			m->empty = 1;
            system_message_queue[i] = (unsigned long) m;		
		}; 
	};
	
	system_message_write = 0;
	system_message_read = 0;
};



unsigned long 
ldisc_dialog ( struct window_d *window, 
              int msg, 
              unsigned long long1, 
              unsigned long long2 ) 
{
    //# cancelada !!!
    return 0;	
}



//
// End.
//


