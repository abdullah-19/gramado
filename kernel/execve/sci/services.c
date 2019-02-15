/*
 * File: execve/sci/services.c 
 *
 *       (SCI) = SYSTEM CALL INTERFACE
 *
 * Esse � o arquivo principal da sci.
 *
 *       GERENCIA OS SERVI�OS OFERECIDOS PELAS INTERFACES /sys.h E /system.h.   
 *
 * Obs: Todas as rotinas chmadas por esse m�dulo devem vir apenas das interfaces
 * /sys.h e /system.h.
 *
 *
 * In this file:
 *   + services: (public)
 *   + servicesChangeProcedure: (private)
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2016 - Revision.
 *     2017 - Revision.
 *     //...
 */


#include <kernel.h>


//
// Constantes internas.
//

//#BUGBUG
// Esse m�ximo de servi�os para a interrup��o 200 n�o existir� mais.
// um n�mero grande de servi�os poder� ser atendido por essa interup��o,
// esses 255 servi�os s�o os servi�os que ser�o atendidos em kernel mode
// aqui no kernel base.
#define SERVICE_NUMBER_MAX 255


//
// Vari�veis internas.
//

//int servicesStatus;
//int servicesError;
//...

// Create Window support.
int cwFlag;                //flag (alerta que os argumentos est�o dispon�veis)
unsigned long cwArg1;      //WindowType
unsigned long cwArg2;      //WindowStatus
unsigned long cwArg3;      //WindowView
char *cwArg4;              //WindowName
unsigned long cwArg5;      //WindowX
unsigned long cwArg6;      //WindowY
unsigned long cwArg7;      //WindowWidth
unsigned long cwArg8;      //WindowHeight
struct window_d * cwArg9;  //gui->screen Parent window
int  cwArg10;               // desktopID 
unsigned long cwArg11;     // WindowClientAreaColor
unsigned long cwArg12;     // WindowColor



//
// Prot�tipos de fun��es internas.
//

void servicesPutChar ( int c );
void servicesChangeProcedure();
unsigned long serviceCreateWindow ( char * message_buffer );

//
//...
//

/*
 * services:
 *     Rotina que atende os pedidos feitos pelos aplicativos em user mode 
 *     via int 200. Ou ainda o kernel pode chamar essa rotina diretamente.
 *     S�o v�rios servi�os.
 *
 *
 * Obs: 
 * TODOS OS SERVI�OS DESSA ROTINA PODEM CHAMAR ROTINAS DE CLASSES 'system.x.x'.
 *
 * @todo: 
 *    Pode haver algum tipo de autoriza��o para essa rotina.
 *    Ou ainda, autoriza��o por grupo de servi�os. Sendo os servi�os
 *    de i/o os de maior privil�gio.
 *
 *    +Identificar o processo que fez a chamada e i/o e
 *    configurar a vari�vel que identifica esse processo.
 *    'caller_process_id' � uma vari�vem encapsulada em process.c
 *    'set_caller_process_id(int pid)' configura a vari�vel.
 *
 *    Uma chamada como OpenDevice pode liberar o acesso
 *    para um determinado processo e CloseDevice cancela o acesso.  
 * 
 *    @todo: O Nome da fun��o, para pertencer � essa pasta, deveria ser:
 *           servicesSystemServices(....).
 *
 *@todo: *importante: Essa rotina deve identificar quem est� chamando, PID TID.
 *
 *
 * *IMPORTANTE:
 *  DEPENDEND DO N�MERO DO SERVI�O ELE PODER� SER TRATADO EM OUTRO ARQUIVO
 *  E N�O NO KERNEL BASE.
 *
 * 
 */

void *services ( unsigned long number, 
                 unsigned long arg2, 
                 unsigned long arg3, 
                 unsigned long arg4 )
{
	//
	// Declara��es.
	//
	
    //Args. (strings)
	unsigned char *stringZ = (unsigned char *) arg2;
    unsigned char *stringZZ = (unsigned char *) arg3;	
	
	unsigned long *a2 = (unsigned long*) arg2;
	unsigned long *a3 = (unsigned long*) arg3;
	unsigned long *a4 = (unsigned long*) arg4;
	

	char *aa2 = (char *) arg2;
	char *aa3 = (char *) arg3;
	char *aa4 = (char *) arg4;

	
    char *tokenList[8];
	
	*tokenList = (char*) arg3;
	
	//Char and string support.
	char *argChar;
	unsigned char* argString;

	//Retorno.
	void* Ret;
	
	//Function(CreateWindow)
	//arg2 =Type
	//arg3=Status
	//arg4=Name
	
	//Window.
	unsigned long WindowType;      //Tipo de janela.
	unsigned long WindowStatus;    //Status, ativa ou n�o.
	unsigned long WindowRect;
	unsigned long WindowView;      //Min, max.
	char *WindowName;            
	
	//
	// Window stuffs:
	//
	
	int ActiveWindow;          //Current id.
	
	struct window_d *hWnd;
	//struct window_d *cWnd;     //Current window.
	//struct window_d *aWnd;     //Active Window. 	
	struct window_d *focusWnd;   //Janela com foco de entrada.
    struct window_d *NewWindow;  //Ponteiro para a janela criada pelo servi�o.
	
	
	//void* kHWND;
	void* kMSG;
	//void* kLONG1;
	//void* kLONG2;
	//...
	
	//Para identificarmos qual processo e qual thread est� chamando.
	//struct window_d *Thread;
	struct process_d *p;
	struct thread_d *t;
	
	
	//
	// Setup.
	//
	
	//Window.
	hWnd = (void*) arg2;


	// #todo: 
	// Antes de utililizar as dimens�es da �rea de trabalho
	// vamos atribuir valores negligenciados para as dimens�es.
	
	//Inicializando dimens�es.
	//Obs: Emulando telas de celulares.
	// 320x480.
	// Terminal abaixo dos di�logos.
	unsigned long WindowX = (2*(800/20));  //100;   >          
	unsigned long WindowWidth  = 320;               
	
    unsigned long WindowY = (2*(600/20)); //100;   V                
    unsigned long WindowHeight = 480;  
	
	unsigned long WindowColor = COLOR_WINDOW;  
	unsigned long WindowClientAreaColor = COLOR_WINDOW;  

    struct rect_d *r;	
	
	
	// ## message support ##
	
	//o endere�o do array passado pelo aplicativo
	//usaremos para enviar uma mensagem com 4 elementos.
	unsigned long *message_address = (unsigned long *) arg2;
	
	unsigned char SC;
	//struct window_d *wFocus;
	
	int desktopID;
	
	
	// *Importante: 
	// Checando se o esquema de cores est� funcionando.
	
	if ( (void *) CurrentColorScheme == NULL )
	{
		printf("StatusBar: CurrentColorScheme");
		die();
	
	}else{
		
		if ( CurrentColorScheme->used != 1 || 
		     CurrentColorScheme->magic != 1234 )
		{
		    printf("StatusBar: CurrentColorScheme validation");
		    die();		
		};
		//Nothing.
	};	
	
	//Configurando cores.
	WindowColor = CurrentColorScheme->elements[csiWindowBackground];  
	WindowClientAreaColor = CurrentColorScheme->elements[csiWindow]; 	
	
	 
	desktopID = (int) get_current_desktop_id();		
	
	// Se a �rea de trabalho for v�lida, usaremos suas dimens�es, 
	// caso n�o seja, temos um problema.
	
	if (gui->main == NULL)
	{
		//#debug
		printf ("services: main");
		die();
	};		
	
	// ## Limits. ## 
	// (Limites do n�mero do servi�o).
	
	// #bugbug
	// obs: Estamos checando se uma vari�vel unsigned long � menor que zero.
	//     Isso  n�o � necess�rio.
	
	if ( number < 0 || number > SERVICE_NUMBER_MAX )
	{
	    return NULL;	
	};
	
	//
	// ## Create Window ##
	//
	
	// Servi�o especial. Antes dos outros.
	if ( number == SYS_118 )
	{
	    return (void *) serviceCreateWindow ( (char *)  arg2 );
	}
	
	//
	// ## Switch ##
	//
	
	//Number.
	switch (number)
	{
	    //0 - Null, O processo pode ser malicioso.
	    case SYS_NULL: 
	        systemRam (0,0,0,0,0); 
	        break; 	   
		
		//Disco: 1,2,3,4
		
		//1 (i/o) Essa rotina pode ser usada por um driver em user mode.
		case SYS_READ_LBA: 
		    //@todo: chamar hal
			//systemDevicesUnblocked(36,arg2,arg3,0,0); 
		    //read_lba( (unsigned long) arg2, (unsigned long) arg3 );
			my_read_hd_sector ( (unsigned long) arg2 , (unsigned long) arg3, 0 , 0 ); 
			break;
			
		//2 (i/o) Essa rotina pode ser usada por um driver em user mode.
		case SYS_WRITE_LBA: 
		    //@todo: chamar hal
			//systemDevicesUnblocked(35,arg2,arg3,0,0); 
			//write_lba( (unsigned long) arg2, (unsigned long) arg3 );
			my_write_hd_sector ( (unsigned long) arg2 , (unsigned long) arg3, 0 , 0 ); 
		    break;

		//3 fopen (i/o)
		//Carregar um arquivo, dado o nome e o endere�o.
		//#obs: chamar rotinas de interface sys_ simplificam 
        //o atendimento �s system calls.			
		//IN: name. address
		case SYS_READ_FILE:
			//funciona, nao mexer nesse
		    //return (void *) sys_read_file ( (unsigned long) a2, (unsigned long) arg3 );
			
			//testando novos recursos,
			return (void *) sys_read_file2 ( (unsigned long) a2, (unsigned long) arg3 );
				
			break;


		//4 (i/o)
		case SYS_WRITE_FILE:
		    taskswitch_lock();
	        scheduler_lock();	
		    
			fsSaveFile ( (char *) message_address[0],         //name
			             (unsigned long) message_address[1],  //3, //@todo: size in sectors 
						 (unsigned long) message_address[2],  //255, //@todo: size in bytes
						 (char *) message_address[3],         //arg3,//address
						 (char) message_address[4] );         //,arg4 ); //flag
						
			scheduler_unlock();
	        taskswitch_unlock();
		    break;
			
		//Gr�ficos:5,6,7,8,9,10,11.
		
		//5 Vertical Sync.(N�o sei se � necess�rio isso via interrup��o.); 
        case SYS_VSYNC:
		    sys_vsync();       		
			break;
			
		//6
		//Put pixel. 
        //Coloca um pixel no backbuffer.
        //Isso pode ser usado por um driver. 
        //cor,x,y,0		
        case SYS_BUFFER_PUTPIXEL:
            //my_buffer_put_pixel ( (unsigned long) a2, 
			//    (unsigned long) a3, (unsigned long) a4, 0 );   		
            backbuffer_putpixel ( (unsigned long) a2, 
			    (unsigned long) a3, (unsigned long) a4, 0 );   		
			break;

		//7
		//desenha um char no backbuffer.
		//Obs: Esse funciona, n�o deletar. :)
		// (x,y,color,char)
		// (left+x, top+y,color,char)
		//devemos usar o left e o top da janela com foco d3e entrada.
        //
		// Obs: A biblioteca c99 em user mode usa essa chamada para pintar um caractere
		// quando implementa a fun��o printf(.). Por�m esse caractere � pintado
		// na janela com o foco de entrada. A inten��o � que o carctere seja pintado 
		// na pr�pria janela do aplicativo.
		// Outro caso � a digita��o de caractere. Quando um caractere �
		// digitado desejamos que ele seja pintado na janela com o foco de entrada.
		//
		// ?? Quando um carctere � pintado em uma janela que n�o est� com o foco 
		//    de entrada ?? ex: shells, logs ...
		//
		//Supondo que os aplicativos escrever�o mais em terminal por enquanto 
		//a cor padr�o de fonte ser� a cor de terminal.
		
		// #importante	
		// Aqui est� pintando o caractere na janela com o foco de entrada.
		case SYS_BUFFER_DRAWCHAR:
			focusWnd = (void *) windowList[window_with_focus];
			if( (void *) focusWnd == NULL ){ 
			    break; 
			};
			
			//x, y, color, char.
			my_buffer_char_blt( (unsigned long) (focusWnd->left + arg2),             
			                    (unsigned long) (focusWnd->top + arg3),              
								CurrentColorScheme->elements[csiTerminalFontColor],   
								(unsigned long) arg4 ); 								 
			break;

		//8 @todo: BugBug, aqui precisamos de 4 par�metros.
        case SYS_BUFFER_DRAWLINE:
			my_buffer_horizontal_line ( (unsigned long) a2, 
			    (unsigned long) a3, (unsigned long) a4, COLOR_WHITE ); 		
			break;

		//9 @todo: BugBug, aqui precisamos de 5 par�metros.
        case SYS_BUFFER_DRAWRECT:
            drawDataRectangle( 0, (unsigned long) a2, (unsigned long) a3, 
				(unsigned long) a4, COLOR_WHITE );    		
			break;

		// 10 Create window.
        // # suspensa.
        case SYS_BUFFER_CREATEWINDOW: 
			//return (void *) serviceCreateWindow ( (char *)  arg2 );
            return NULL;
			break;
			
			
		//11, Coloca o conte�do do backbuffer no LFB.
        case SYS_REFRESHSCREEN: 
		    //systemRam ( 3, 0, 0, 0, 0 ); 
			refresh_screen ();
			break;			
			
        //rede: 12,13,14,15			
		//i/o:  16,17,18,19	
        

        //24-28 WINDOW SUPPORT		
        
		//24
		// window.c	
		case 24:
		    return (void *) show_window_rect ( (struct window_d *) arg2 );
		    break;
	 
		//34	
        case SYS_VIDEO_SETCURSOR: 
		    systemRam(86,arg2,arg3,0,0); 
			//set_up_cursor( (unsigned long) arg2, (unsigned long) arg2);
			break;              

		//35 - Configura o procedimento da tarefa atual.
        case SYS_SETPROCEDURE:  
            g_next_proc = (unsigned long) arg2;		
            break;
			
		//36
        //O teclado envia essa mensagem para o procedimento ativo.
        case SYS_KSENDMESSAGE: 
            systemRam ( 2, (unsigned long) arg2, (unsigned long) arg3, 
			    (unsigned long) arg4, 0 ); 
            break;    
      
	
		//37 - Chama o procedimento procedimento default. @todo return.
		case SYS_CALLSYSTEMPROCEDURE: 
            system_procedure(NULL,arg2,arg3,arg4);	
            break;    
        
	
	    // ## CANCELADA! ##
        //42 Load bitmap 16x16.
		case SYS_LOADBMP16X16 :       
            return NULL;			
            break;


        //45 Message Box. 
        case SYS_MESSAGEBOX:		
            //MessageBox(gui->screen, 1, (char *) a3, (char *) a4 );        
            return NULL;
			break;
		
        //47, Create Window support.		
        //envia argumentos de controle.
		case SYS_BUFFER_CREATEWINDOWx:
		    //cwFlag  = 1234;  //Aciona a flag.
            //cwArg2  = arg2;  //WindowStatus 
			//cwArg11 = arg3;  //cor da area de cliente.
			//cwArg12 = arg4;  //cor da janela.
			return NULL;
			break;
		
		//48, Create Window support.
		//envia argumentos de posicionamento.
		case SYS_BUFFER_CREATEWINDOW1:
		    //cwArg5 = arg2;  //x
			//cwArg6 = arg3;  //y
			//cwArg9 = gui->screen;  //@todo: O argumento arg4 est� enviando parent window.    
			return NULL;
			break;
		
		//49, Create Window support.
		//envia argumentos de dimens�es.
		case SYS_BUFFER_CREATEWINDOW2:
		    //cwArg7 = arg2; //width
			//cwArg8 = arg3; //height
			//cwArg10 = arg4;  //desktop ID 
			return NULL;
			break;	
			
		//50~59 Window suppot, manipula��o de janelas	

		//50 resize window (handle,x,y)
		case SYS_BUFFER_RESIZEWINDOW:		
		    return (void *) resize_window( (struct window_d*) arg2, arg3, arg4);
		    break;
		
		//51 redraw window. (handle)
		case SYS_BUFFER_REDRAWWINDOW:
		    return (void *) redraw_window ( (struct window_d*) arg2, arg3 );
		    break;
		
		//52  replace window. (handle,x,y)
		case SYS_BUFFER_REPLACEWINDOW:
		    return (void *) replace_window( (struct window_d*) arg2, arg3, arg4);
		    break;
		
		//53 maximize window 
		case SYS_BUFFER_MAXIMIZEWINDOW:
		    //(handle)
		    MaximizeWindow((struct window_d*) arg2);
		    break;
		
		//54 minimize window
		case SYS_BUFFER_MINIMIZEWINDOW:
		    //(handle)
		    MinimizeWindow( (struct window_d *) arg2);
		    break;
		
		//55 Get foreground window.
		case SYS_BUFFER_GETFOREGROUNDWINDOW:
		    return (void *) windowGetForegroundWindow();
		    break;
		
		//56 set foreground window.
		case SYS_BUFFER_SETFOREGROUNDWINDOW:
		    return (void *) windowSetForegroundWindow((struct window_d*) arg2);
		    break;
		
		
		//57.	
		case SYS_REGISTERWINDOW: 
		    systemRam(41,(unsigned long) hWnd,0,0,0); 
			break;
			
		//58.	
		case SYS_CLOSEWINDOW: 
		    systemRam(53,(unsigned long) hWnd,0,0,0); 
			break;
			
        //60
		case SYS_SETACTIVEWINDOW:			
			set_active_window(hWnd);
			break;

        //61
		case SYS_GETACTIVEWINDOW:
            return (void *) get_active_window();    //Id. (int).		
			break;

        //62
		case SYS_SETFOCUS: 
		    systemRam(71,(unsigned long) hWnd,0,0,0); 
			break;
			
        //63 id
		case SYS_GETFOCUS: 
		    return (void *) window_with_focus;  
			break;
			
        //64
		case SYS_KILLFOCUS:
            //KillFocus(window);		
		    systemRam(73,(unsigned long) hWnd,0,0,0); 
			break;

		//65	
		//putchar usando o cursor gerenciado pelo kernel.
		//a biblioteca em user mode, altera o cursor do kernel e 
		//usa essa rotina para imprimir.
		//obs: #importante: Como printf � uma fun��o 
		//usada pelo terminal virtual, deve-se considerar as cores 
		//usadas no terminal virtual.
		//@todo: implementar a configura��o de cores no terminal virtual 
		//usado pelo aplicativo.
		//obs: estamos improvisando as cores por enquanto.
		//
		// Obs: ?? Como faremos para pintar dentro da janela do terminal.
        // Obs: a rotina de configura��o do terminal dever� ajustar 		
		// as margens usadas pela rotina de impress�o de caracteres.
        // ent�o nesse momento devemos considerar que as margens ja est�o 
        // ajustadas.		

        // #importante:
		// putchar pertence a libc e todo o sistema tem obedecido 
		// a sua maneira de imprimir chars ... n�o podemos mudar 
		// putchar assim t�o facilmente.
		// refresh_rectangle obedece os deslocamentos usados 
		// por putchar.
 		
		// Coloca um char usando o 'terminal mode' de stdio.
		// selecionado em _outbyte.
		// stdio_terminalmode_flag = n�o transparente.
		// Chama fun��o interna.
		
		case SYS_PUTCHAR: 
			servicesPutChar ( (int) arg2 );		
			break;

		//66 - reservado pra input de usu�rio.
		//case 66:
        //    systemDevicesUnblocked( (int)1, (unsigned long) arg1, 
		//       (unsigned long) arg2, (unsigned long) arg3, (unsigned long) arg4);  		
		//	break;

		//67- reservado pra input de usu�rio.	
		//case 67:
        //    systemDevicesUnblocked( (int)1, (unsigned long) arg1, 
		//        (unsigned long) arg2, (unsigned long) arg3, (unsigned long) arg4);  		
		//	break;

		//68- reservado pra input de mouse.	
		//case 68:
        //    systemDevicesUnblocked( (int)1, (unsigned long) arg1, 
		//        (unsigned long) arg2, (unsigned long) arg3, (unsigned long) arg4);  		
		//	break;

		//	
		//69 - Driver de teclado enviando mensagem de digita��o.	
		//     N�o somente um driver, mas qualquer processo pode enviar
		// uma mensagem pra janela com o foco de entrada e chamar o procedimento.
		// >>>> Enviando para a classe certa.
        // arg2=msg, arg3=ch, arg4=ch 0		
		//
		case 69:
			systemDevicesUnblocked ( (int) 3, (unsigned long) arg2, (unsigned long) 
			    arg3, (unsigned long) arg4, 0);  		
			break;
			
		//
        // ## EXIT ##
        //		
			
		// 70 - Exit. Torna zombie a thread atual.
		//isso foi chamado pela libc em user mode.
		//quando essa interrup��o retornar, a libc para num while(1)
		case SYS_EXIT:
			
			//0 = Estamos saindo de aplicativo normalmente, sem erros.
			//isso n�o se aplica aos aplicativos que desejamos reaproveitar estrutura.
			if (arg2 == 0)
			{							
				sys_exit_thread (current_thread); 
				break; 
			}; 	 		
			
			//9= sa�da anormal,
			//temos a inten��o de reaproveitarmos a estrutura 
			//da thread, e isso realmente n�o � normal.
			//Isso far� a thread ficar presa num while da fun��o exit na libc.
			if (arg2 == 9)
			{
				//do_thread_initialized(current_thread);
				break;
			}
			
			//...
			
			break;
		
        //71 		
		case SYS_FORK: 
		    return (void *) do_fork_process ();
			break;	

		//72 - Create thread.	
        case SYS_CREATETHREAD:			
			return (void *) create_thread( 
			                NULL,             // w. station 
							NULL,             // desktop
							NULL,             // w.
							arg2,             // init eip
							arg3,             // init stack
							current_process,  // pid (determinado)(provis�rio).
							(char *) a4 );    // name
			break; 

		//73 - Create process.
        //@todo; Ok, nesse momento, precisamos saber qual � o processo pai do processo 
        //que iremos criar. Esse deve ser o processo atual ...  		
		// PPID = 0. Nesse momento todo processo criado ser� filho do processo n�mero 0.
		// mas n�o � verdade. @tpdp: Precisamos que o aplicativo em user mode 
		// nos passe o n�mero do processo pai, ou o proprio kernel identifica qual � o 
		//processo atual e determina que ele ser� o processo pai.        
		case SYS_CREATEPROCESS:
            return (void *) create_process ( NULL, NULL, NULL, 
			                    arg2, arg3, 0, (char *) a4, 
								RING3, (unsigned long ) CreatePageDirectory() ); 		
            break;
			
		//80 Show current process info.
		//#todo: Mostrar em uma janela pr�pria.
		//#todo: Devemos chamar uma fun��o que mostre informa��es 
		//apenas do processo atual. 
		case SYS_CURRENTPROCESSINFO:
		    show_currentprocess_info ();
		    break;
			
		//81
		//#bugbug Isso est� retornando o ID do processo atual.
		//O que queremos � o ID do processo que est� chamando
		case SYS_GETPPID: 
		    return (void *) sys_getppid ();
			break;
		
		//82
		//
		case SYS_SETPPID: 
			break;
			
			
		//83
        case SYS_WAIT4PID: 
            //schedi.c
			return (void *) do_wait ((int *) arg2 );
			
			// TID, PID 		
			// TID � a thread atual.
			// PID veio via argumento.
			//block_for_a_reason ( (int) current_thread, (int) arg2 ); //suspenso
		    break;
			
		//85
		//#bugbug Isso est� retornando o ID do processo pai do processo atual.
		//O que queremos � o ID do processo pai do processo que est� chamando.
		case SYS_GETPID: 
		    return (void *) sys_getpid();
			break;
		
		//86
		//
		case SYS_SETPID: 
		    //
			break;
		
		//Down. 87
		case SYS_SEMAPHORE_DOWN:
		    return (void *) Down ( (struct semaphore_d *) arg2);
		    break;
			
		//Testa se o processo � v�lido
        //se for v�lido retorna 1234		
		//testando...
		case SYS_88:   
            return (void *) processTesting (arg2);			
			break;
			
		//Up. 89	
		case SYS_SEMAPHORE_UP:
		    return (void *) Up ( (struct semaphore_d *) arg2 );
		    break;
		
		//90 Coletor de threads Zombie. (a tarefa idle pode chamar isso.)		
		case SYS_DEADTHREADCOLLECTOR: 
		    systemIoCpu ( 1, 0, 0, 0, 0 ); 
			break;
			
		//94	
		//REAL (coloca a thread em standby para executar pela primeira vez.)
		// * MOVEMENT 1 (Initialized --> Standby).
		case SYS_STARTTHREAD:
		    t = (struct thread_d *) arg2;
            SelectForExecution (t);    	
			break;		
			
	    //
        // 99,100,101,102 = Pegar nas filas os par�metros hwnd, msg, long1, long2.
        //

        // *importante: 
		// #bugbug SYS_GETKEYBOARDMESSAGE (44) est� pegando a mensagem de teclado,
		// mas na verdade deveria apenas pegar a mensagem, sem se preocupar em 
		// qual foi o dispositivo gerador do evento. ??!!

			
		// 99,  Pega 'hwnd' na fila da janela com o foco de entrada.
		case SYS_GETHWINDOW:
		    return (void *) systemDevicesUnblocked ( 43, 
			                    arg2, arg2, arg2, arg2 );
		    break;
			
		//#bugbug
		//**** 44, Pega 'msg' na fila da janela com o foco de entrada.
		//Pegando a mensagem na fila da janela com o foco de entrada.
		case SYS_GETKEYBOARDMESSAGE:
		    return (void *) systemDevicesUnblocked ( 44, 
			                    (unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus );
			break;
			
		//**** 45,  Pega 'long1' na fila da janela com o foco de entrada.	
		case SYS_GETLONG1:
		    return (void *) systemDevicesUnblocked ( 45, 
			                    (unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus );
			break;
			
		//**** 46,  Pega 'long2' na fila da janela com o foco de entrada.	
		case SYS_GETLONG2:
		    return (void *) systemDevicesUnblocked ( 46, 
			                    (unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus, 
								(unsigned long) WindowWithFocus);
			break;


 
		//103, SYS_RECEIVEMESSAGE	
        //Um processo consumidor solicita mensagem deixada em seu PCB.
        //Argumentos: servi�o, produtor, consumidor, mensagem.		
        //@todo: 
		case SYS_RECEIVEMESSAGE:
			break;
			
		//104, SYS_SENDMESSAGE
		//Um processo produtor envia uma mensagem para o PCB de outr processo.
		//Argumentos: servi�o, produtor, consumidor, mensagem.
		//@todo:		
		case SYS_SENDMESSAGE:			
			break;

			
		//110 Reboot.
        //(Teclado � legado, � desbloqueado)		
	    case SYS_REBOOT: 
			systemReboot();
			break;
			
		
		//
		// #### Get Message ####
		//
		
		// #importante:
		// Esse � o get message usado pelos aplicativos.
		// O aplicativo envia um endere�o de array e devemos colocar 4 longs 
		// como mensagem.
		// Isso funcionou. 
		// Esse ser� o padr�o at� usarmos ponteiro para estrutura.
		
		// A inten��o agora � pegar a mensagem na estrutura de thread atual.
		// Desse modo cada aplicativo, quando estiver rodando, pegar� as 
        // suas mensagens em sua pr�pria fila.  		
		// Se n�o houver mensagem na estrutura da thread, ent�o tentaremos colocar alguma.
		// Vamos colocar alguma coisa do buffer de teclado.
			
		case 111:
		    if ( &message_address[0] == 0 )
			{
				printf ("services: 111, null pointer");
				die();
				
			}else{
				
			    t = (void *) threadList[current_thread];
			    
	            if ( (void *) t == NULL )
	            {
		           return NULL;
	            }
								
				if ( t->newmessageFlag != 1 )
				{
					SC = (unsigned char) get_scancode ();
	                KEYBOARD_LINE_DISCIPLINE ( SC );						
				}
	
				//pegando a mensagem.
			    message_address[0] = (unsigned long) t->window;
			    message_address[1] = (unsigned long) t->msg;
			    message_address[2] = (unsigned long) t->long1;
			    message_address[3] = (unsigned long) t->long2;
                    
				//sinalizamos que a mensagem foi consumida.
                t->newmessageFlag = 0; 					
				    
				//sinaliza que h� mensagem
				return (void *) 1; 
				
			};
		    break;
		
		//Envia uma mensagem PAINT para o aplicativo atualizar a �rea de trabalho.
		case 113:
		    windowUpdateWindow( (struct window_d *) arg2 );
			break;
			
		// 114	
        // ## ENVIA UMA MENSAGEM PARA UMA JANELA ##
		
		//enviar uma mensagem para a thread atual.
		//
		case SYS_SENDWINDOWMESSAGE:
		    if ( &message_address[0] == 0 )
			{
				printf("114: null pointer");
				die();
			}else{
				
				//hWnd = (struct window_d *) message_address[0];
				//hWnd = (void *) windowList[window_with_focus];
				t = (void *) threadList[current_thread];
				//if ( (void *) == NULL )
				//{
				//	return NULL;
				//}
				//temos que checar a validade.
				if ( (void *) t != NULL )
                {
                    if ( t->used == 1 && t->magic == 1234 ){					
				        
						t->window = (struct window_d *) message_address[0];
				        t->msg = (int) message_address[1];
				        t->long1 = (unsigned long) message_address[2];
				        t->long2 = (unsigned long) message_address[3];
				
				        //sinalizando que temos uma mensagem.
				        t->newmessageFlag = 1; 
					};
			    };
			};
		    break;
			
		// 115 - ## IMPORTANTE ## 
		// Usado por servidores do sistema para se comunicarem 
		// com o kernel.
		// >> magic 1234: Acoplar taskman.
		// >> magic 4321: Desacoplar taskman
		// >> magic 12345678: Pegar mensagem
		// #todo: Chamar uma fun��o que trate o argumento com um switch.
		case 115:
		
			if ( arg3 == 1234 )
			{
				current_taskman_server = (int) arg4;
				//printf("115: acoplando ...\n");
				//refresh_screen();
                return NULL; 				
			}; 
			
			if ( arg3 == 1234 )
			{
				if( current_taskman_server == arg4 )
				{
				    current_taskman_server = (int) 0;
				    //printf("115: desacoplando ...\n");
				    //refresh_screen();
				    return NULL;
				} 
			};
			
			if ( arg3 == 12345678 )
			{
				if( current_taskman_server == arg4 )
				{
					if( gui->taskmanWindow->newmessageFlag == 0 )
					{
			            message_address[0] = (unsigned long) 0;
			            message_address[1] = (unsigned long) 0; //*importante: mensagem nula.
			            message_address[2] = (unsigned long) 0;
			            message_address[3] = (unsigned long) 0;
					    gui->taskmanWindow->newmessageFlag = 0;
                        return NULL;						
					}
					//se existe uma mensagem na janela do servidor taskman.
					if( gui->taskmanWindow->newmessageFlag == 1 )
					{
			            message_address[0] = (unsigned long) gui->taskmanWindow->msg_window;
			            message_address[1] = (unsigned long) gui->taskmanWindow->msg; 
			            message_address[2] = (unsigned long) gui->taskmanWindow->long1;
			            message_address[3] = (unsigned long) gui->taskmanWindow->long2;
					    gui->taskmanWindow->newmessageFlag = 0;
						return NULL;
					};
				};
			};
			break;
			
		//Envia uma mensagem de teste para o servidor taskman	
		case 116:
	        gui->taskmanWindow->msg_window = NULL;
		    gui->taskmanWindow->msg = (int) arg2;             //123; //123=temos uma mensagem. 
		    gui->taskmanWindow->long1 = (unsigned long) arg3;    //0;
		    gui->taskmanWindow->long2 = (unsigned long) arg4;    //0;
            gui->taskmanWindow->newmessageFlag = 1;				
		    break;
			
		//119
		case SYS_SELECTCOLORSCHEME:
		    return (void *) windowSelectColorScheme ( (int) arg2 );
			break;
			
			
		//124	
		// defered system procedure call.
		// #todo: precisamos armazenasr os argumentos em algum lugar.
		case 124:
		    kernel_request = KR_DEFERED_SYSTEMPROCEDURE;
			break;
			
		//125	
		// system procedure call.
        case 125:
            return (void *) system_procedure ( NULL, arg2, arg3, arg4 );
			break;		
		
		//
		// 129, Um driver confirmando que foi inicializado.
		// Efetuaremos a rotina de liga��o do driver com o sistema.
		// O sistema linka o driver com o sistema operacional.
	    //@todo: Essa rotina precisa ser aprimorada. Mas a chamada deve ser 
		// essa mesma.
		//
		case SYS_DRIVERINITIALIZED: 
		    return (void *) systemLinkDriver(arg2,arg3,arg4); 
			break;
			
		//130
		case SYS_DRAWTEXT:
		    
			//@todo: Poderia pintar na janela atual.
			argString = (unsigned char *) arg4;
		    draw_text ( (struct window_d *) message_address[0], //window
			    (unsigned long) message_address[1],  //x
				(unsigned long) message_address[2],  //y
				(unsigned long) message_address[3],   //color
				(unsigned char *) message_address[4] ); //string
			break;
			
		//131
		// Pintar o caractere especificamente na janela com o foco de entrada.          
		case SYS_BUFFER_DRAWCHAR_WWF: 
			focusWnd = (void *) windowList[window_with_focus];
			if ( (void *) focusWnd == NULL )
			{
			    break;	
			};
			
			my_buffer_char_blt( (unsigned long) (arg2 + focusWnd->left), //x.
			                    (unsigned long) (arg3 + focusWnd->top),  //y.
								COLOR_BLACK,                             //color. 
								(unsigned long) arg4);                   //char.
    		break;
			
		case 132:
		    //#bugbug: pagefault
			//save_rect (  message_address[0],  message_address[1],  message_address[2],  message_address[3] );
			break;
			
        case 133: 		
		    //#bugbug: pagefault
			//show_saved_rect (  message_address[0],  message_address[1],  message_address[2],  message_address[3] );
			break; 
			
		// 134
		// Pegar informa��es sobre a �rea de cliente de uma janela;
		//#bugbug: temos que checar mais validade de estrutura.
		//obs: No come�o dessa fun��o, colocamos o arg3 como ponteiro para a3.
		//um buffer de longs.
		case 134:
				hWnd = (struct window_d *) arg3;

				if ( (void *) hWnd != NULL )
				{	
					a3[0] = (unsigned long) hWnd->rcClient->left;
					a3[1] = (unsigned long) hWnd->rcClient->top;     
					a3[2] = (unsigned long) hWnd->rcClient->width;
					a3[3] = (unsigned long) hWnd->rcClient->height;
					a3[4] = (unsigned long) hWnd->rcClient->bg_color;
				}
			break;
			

		// 135
		// Coloca caracteres na estrutura de terminal, para aplciativos pegarem
        case SYS_FEEDTERMINAL:
            //@todo:
			// Colocar o caractere enviado no argumento para 
			//a estrutura de terminal do terminal indicado no argumento.
			//terminalFeed( (int) arg2, (int) arg3 );
			
			//arg2 = n�mero do terminal
			//arg3 = ch 
			//arg4 - ch 
			
			//terminal.h
			//teminalfeedCH = (char) arg3;
			//teminalfeedCHStatus = (int) 1;
		    break;	
			
		//136
		//fgetc
		case 136:
			return (void *) fgetc ( (FILE *) arg2 );
			break;
			
		//137
		// Isso � usado pela biblioteca stdio em user mode
		// na fun��o 'getchar()'
		// #bugbug: Est� pegando caracteres extras �s vezes.
		//#test
		//#bugbug: a partir de agora isso deve pegar mensagem na thread 
		//atual e n�o mais na janela com foco de entrada.			
        case SYS_GETCH:  
			return (void *) thread_getchar();
            break;

		//138 - get key state.	
		//#importante: 
		//#todo: isso precisa ir para a API.
        case 138:
		    return (void *) keyboardGetKeyState ( (unsigned char) arg2 );
            break;		
			
		//139
        case SYS_GETSCANCODE:
		    return (void *) get_scancode();
            break;		

        //140
        case SYS_SET_CURRENT_KEYBOARD_RESPONDER:
            set_current_keyboard_responder(arg2);		
		    break;
			
		//141	
		case SYS_GET_CURRENT_KEYBOARD_RESPONDER:
		    return (void *) get_current_keyboard_responder();
		    break;
			
		//142	
        case SYS_SET_CURRENT_MOUSE_RESPONDER:			
		    set_current_mouse_responder(arg2);
			break;
			
		//143	
		case SYS_GET_CURRENT_MOUSE_RESPONDER:
		    return (void *) get_current_mouse_responder();
			break;

			
		//144	
		//Pega o ponteiro da client area.	
		case SYS_GETCLIENTAREARECT:	
		    //#bugbug: pegamos o ponteiro mas n�o temos permiss�o para acessar a estrutura.
			return (void *) getClientAreaRect();	
			break;
		
		//145
        //configura a client area	
        //@todo: O ponteiro para estrutura de ret�ngulo � passado via argumento.		
		case SYS_SETCLIENTAREARECT:
			setClientAreaRect ( arg2, arg3, arg4, 0);
            break;

		//146	
		//#bugbug: isso n�o parece seguro, precismos checar a validade da estrutura antes,
        //mas vai ficar assim por enquanto.					
		case 146:	
		    //return (void *) gwsScreenWindow(); //#todo
		    return (void *) gui->screen;
			break;
			
		//147	
		//#bugbug: isso n�o parece seguro, precismos checar a validade da estrutura antes,
        //mas vai ficar assim por enquanto.		
        case 147:
		    //return (void *) gwsMainWindow(); //#todo
			return (void *) gui->main;
			break;
			
		//create grid and itens.
		//n, view 
		case 148:
		    return (void *) grid ( (struct window_d *) arg2, 
			                (int) arg3, (int) arg4 );
            break;		

        //test. menu.
		case 149:
            //essa � uma rotina de teste, qua chama v�rias fun��es.
			MainMenu ( (struct window_d *) arg2 );		
            break;		
			
		//152 - get user id	
		case SYS_GETCURRENTUSERID:
		    return (void *) current_user;
			break;			
		
		//154 - get group id	
		case SYS_GETCURRENTGROUPID:
		    return (void *) current_group;
			break;
			
		// 167
        // SYS_GRAMADOCORE_INIT_EXECVE
        // executa um novo programa dentro do processo INIT 
		// do ambiente Gramado Core.	
        //
        // #importante:
        // os argumentos recebidos aqui precisam ir para ipc/spawn.c 
        // que ser�o enviados via registradores para o aplicativo.
        // Obs: N�o adianta enviar ponteiros para o aplicativo, 
        // pois ele n�o pode pegar no kernel.	
		
		//167:
		//EXECUTA .BIN COM ENTRYPOINT EM 0x1000
		case SYS_GRAMADOCORE_INIT_EXECVE_BIN:
		    // Testar
			// execve.c
		    // Obs: � dever dessa rotina colocar a thread em estado standby,
			// onde ela fica selecionada para execuss�o.
			// pois um exit pode deixar o estado da thread reaproveitada 
			// de um jeito que n�o rode.
			return (void *) executive_gramado_core_init_execve( 0,  //servi�o 
			                (const char *) arg2,                //name
			                (const char *) arg3,                //(arg)(endere�o da linha de comando)
							(const char *) arg4 );              //env
			break;
			
		//168
        //EXECUTA .EXE COM ENTRYPOINT EM 0x400		
		case SYS_GRAMADOCORE_INIT_EXECVE_EXE:
		    // Testar
			//executive.c
		    return (void *) executive_gramado_core_init_execve_exe( 0,  //servi�o 
			                (const char *) arg2,                //name
			                (const char *) arg3,                //(arg)(endere�o da linha de comando)
							(const char *) arg4 );              //env
			break;			
			
		//157 - get user session id	
		case SYS_GETCURRENTUSERSESSION:
		    return (void *) current_usersession;
			break;
			
        //158 - get window station id		
		case SYS_GETCURRENTWINDOWSTATION:	
		    return (void *) current_room; 
			break;	
			
		//159 - get desktop id
        case SYS_GETCURRENTDESKTOP:		
		    return (void *) current_desktop; 
			break;
			
		//160
		//Criar um socket e retornar o ponteiro para a estrutura.
		case 160:
            return (void *) create_socket ( (unsigned long) arg2, (unsigned short) arg3 );
			break;
			
		//161
        //get socket IP		
		case 161:
		    return (void *) getSocketIP( (struct socket_d *) arg2 );
            break;		

		//162
        //get socket port		
		case 162:
		    return (void *) getSocketPort( (struct socket_d *) arg2 );
            break;		
			
		//163
        //update socket  
        //retorno 0=ok 1=fail		
        case 163:
            return (void *) update_socket ( (struct socket_d *) arg2, (unsigned long) arg3, (unsigned short) arg4 );
			break;		

		//#todo: a chamada est� no shell em net.c
		//netStream
        case 164:
            //IN: ( service, (unsigned long) stream, option, option )
			break;	

		//#todo: a chamada est� no shell em net.c	
		//netSocket
        case 165:
            //IN: ( service, (unsigned long) socket, option, option );	
			break;

		//#todo: a chamada est� no shell em net.c	
		//netBuffer
        case 166:
            //IN:  ( service, buffer_address, option, option );	
			break;		
			
		//170
        //pwd ...
        //Cada processo tem seu pr�prio pwd.
        //Essa rotina mostra o pathname usado pelo processo.	
		case SYS_PWD:
			fs_print_process_pwd (current_process);
			break;	
		
		//171 - retorna o id do volume atual.
		case SYS_GETCURRENTVOLUMEID:
		    return (void *) current_volume;
            break;	

		//172 - configura o id do volume atual.
		//#bugbug: Estamos modificando, sem aplicar nenhum filtro.
		case SYS_SETCURRENTVOLUMEID:
		    current_volume = (int) arg2;
            break;	

		//173 Lista arquivos de um diret�rio, dado o n�mero do disco,
        //o n�mero do volume e o n�mero do diret�rio,
        //args in: disk id, volume id, directory id		
        case SYS_LISTFILES:
            fsListFiles ( arg2, arg3, arg4 );  
			break;
			
			
		//174
		case SYS_SEARCHFILE:
		    return (void *) KiSearchFile ( (unsigned char *) arg2, 
                                (unsigned long) arg3 );
			break;
			
		//175
		// cd.
        //Atualiza o pathname na estrutura do processo atual.
        //Atualiza o pathname na string global.		
		case 175:
		    fsUpdateWorkingDiretoryString ( (char *) arg2 );
            fsLoadFileFromCurrentTargetDir ();
			break;
			
		//176	
        //Remove n nomes de diret�rio do pathname do processo indicado no argumento.
        //Copia o nome para a string global.
		case 176:	
		    fs_pathname_backup ( current_process, (int) arg3 );
			break;
			
		//177
		//'dir'
		//comando dir no shell.
		//Listando arquivos em um diret�rio dado o nome.	
		case 177:
		    fsList ( (const char *) arg2 );		
            break;

        case 178:
		    //#test
			//implementando esse servi�o.
		    taskswitch_lock();
	        scheduler_lock();	
			//name , address.
            Ret = (void *) fsGetFileSize ( (unsigned char *) arg2 ); 
			scheduler_unlock();
	        taskswitch_unlock();
			return (void *) Ret;		    
            break;		
			
		// # test #	
		// #bugbug: suspenso.
		// passando vetor de ponteiros.
		// executando um novo programa no processo init.
		//case 179:
		// contador, endere�o.
		//    return (void*) execve_execute_on_init( (int) arg2, (const char *) arg3 );
        //    break;		

		//184
		//pega o endere�o do heap do processo dado seu id.	
        case SYS_GETPROCESSHEAPPOINTER:
            return (void *) GetProcessHeapStart ( (int) arg2 );
			break;	

		// feof	
		case 193:	
		    return (void *) feof ( (FILE *) arg2 );
            break;
			
		//ferror	
		case 194:	
		    return (void *) ferror ( (FILE *) arg2 );
            break;
			
		//fseek	
		case 195:	
		    return (void *) fseek ( (FILE *) arg2, (long) arg3, (int) arg4 );
            break;
			
		//fputc	
		case 196:	
		    return (void *) fputc ( (int) arg2 , (FILE *) arg3 );
            break;			
			
        //197
		//scroll de �rea de cliente de uma janela;
        case 197:
		    //essa � uma boa rotina, mas precisamos tabalhar nisso,
			//est� dando page fault.
			//scroll_client_window ( (struct window_d *) arg2 );
            break; 		
			

		//199 - Garbage Collector.	
		//A ideia � que os utilit�rios de ger�ncia de mem�ria possam usar isso.
		case SYS_GC:
		    gc ();
			//return (void *) gc (); 
		    break;
		
		//200 - Envia um sinal para um processo.	
		//argumentos (process handle, signal number).
		case SYS_SENDSIGNAL:
		    signalSend ( (void *) a2, (int) arg3 );
		    break;
			
		//...

        //205
		//block for a reason.
        //bloqueamos uma thead por um dos motivos indicados em argumento.
        case SYS_GENERICWAIT:
		    //TID, reason.
            block_for_a_reason ( (int) arg2, (int) arg3 );
			break;		
			
		//210
		case SYS_CREATETERMINAL: 
            break;
			
		//211
        case SYS_GETCURRENTTERMINAL:
            return (void *) current_terminal;
			break;
			
	    //212
        case SYS_SETCURRENTTERMINAL:
            current_terminal = (int) arg2;
			break;
			
		//213
        case SYS_GETTERMINALINPUTBUFFER:	
            return NULL;
			break;
			
		//214
        case SYS_SETTERMINALINPUTBUFFER:		
		    break;

		// 215
		// Get terminal window.	
		// retorna o ID.
		// O ID fica em terminal_window.
		case SYS_GETTERMINALWINDOW: 
			return (void *) systemGetTerminalWindow(); 
			break;

		// 216
		// Set terminal window	
		// Configura qual vai ser a janela do terminal virtual. 
		// #obs: O refresh de stdout podera' ocorrer em ts.c
		// O ID fica em terminal_window.	
		case SYS_SETTERMINALWINDOW:	
		   systemSetTerminalWindow ( (struct window_d *) arg2 );	
		   break;
		   
		//217
		case SYS_GETTERMINALRECT:
			return NULL; 
			break;

		//218
        //configura na estrutura do terminal corrente,
        //qual vai ser o ret�ngulo a ser usado. 		
		case SYS_SETTERMINALRECT:
		
		   //#bugbug: Pagefault.
		   
		    // Estamos usando default por falta de argumento.
            //systemSetTerminalRectangle( DEFAULT_TERMINAL_LEFT, 
			//                            DEFAULT_TERMINAL_TOP, 
			//							arg3, 
			//							arg4 );		
		    break;
			
		//219
        case SYS_DESTROYTERMINAL:
            break; 	


		// 222 - create timer.
		//args: window e ms e tipo
        case 222:
		    return (void *) create_timer ( (struct window_d *) arg2, (unsigned long) arg3, (int) arg4 );
		    break;

		
        //223 - get sys time info.
        // informa��es variadas sobre o sys time.		
		case 223:
		    return (void *) get_systime_info ( (int) arg2 );
            break;		
			
		//224 - get time	
		case SYS_GETTIME:	
		    return (void *) get_time();
			break;
			
		//225 - get date
		case SYS_GETDATE:
		    return (void *) get_date();
            break;		
			
			
		//Obs: @todo: poderia ser uma chamada para configurar o posicionamento 
        //e outra para configurar as dimens�es.		
			
		//226 get
        case SYS_GET_KERNELSEMAPHORE:
            return (void *) __spinlock_ipc;
            break;
        
        //227 close critical section	
		case SYS_CLOSE_KERNELSEMAPHORE:
			__spinlock_ipc = 0;
			break;
			
		//228 open critical section
		case SYS_OPEN_KERNELSEMAPHORE:
		    __spinlock_ipc = 1;
            break;
			
			
		//232
		case 232:
			return (void *) fclose ( (FILE *) arg2);
			break;
			
		//233
		case 233:
			return (void *) fflush ( (FILE *) arg2);
			break;
			
		//234
		case 234:
			return (void *) fprintf ( (FILE *) arg2, (const char *) arg3 );
			break;
			
		//235
		case 235:
			return (void *) fputs ( (const char *) arg2, (FILE *) arg3 );
			break;
			
			
			
			
		//240
		case SYS_GETCURSORX:
		    return (void *) get_cursor_x();
		    break;

		//241
		case SYS_GETCURSORY:
		    return (void *) get_cursor_y();
		    break;
			
		//244 enable text cursor.	
		case 244:
		    timerEnableTextCursor ();
		    //timerShowTextCursor = 1;
		    //gwsEnableTextCursor ();
            break;		
		
		//245 disable text cursor.
		case 245:
		    timerDisableTextCursor ();
		    //timerShowTextCursor = 0;
		    //gwsDisableTextCursor ();
            break;		
			
		//Info. (250 ~ 255).
		
		//250
		//Get system metrics
		case SYS_GETSYSTEMMETRICS:
		    return (void *) systemGetSystemMetrics ( (int) arg2 );
		    break;
		
		//251
		//Informa��es sobre o disco atual.
		case SYS_SHOWDISKINFO:
		    diskShowCurrentDiskInfo();
			refresh_screen();
			break;

		//252
		//Informa��es sobre o volume atual.
		case SYS_SHOWVOLUMEINFO:
		    volumeShowCurrentVolumeInfo();
			refresh_screen();
			break;
		
		//253
		case SYS_MEMORYINFO:
		    memoryShowMemoryInfo();
			refresh_screen();
			break;
			
			
		//254 - Show PCI info.	
		case SYS_SHOWPCIINFO: 
			systemShowDevicesInfo();
			refresh_screen();
			break;
			
		//255 - Mostra informa��es sobre o kernel.	
		case SYS_SHOWKERNELINFO: 
			KiInformation ();
			//refresh_screen ();
			break;
			
		// @todo:
		// Need to check the system call ID to ensure it's valid 
		// If it s invalid, return ENOSYS Function not implemented error
		
		default:	
			printf ("services: Default {%d}\n", number );
			//#todo: retorno.
			//return NULL;
			break;
	};
	
	//Debug.
	//printf("SystemService={%d}\n",number);
    
	
	// * importante:
	//   Depois de esgotados os 'cases', saltamos para a sa�da da fun��o.
	
	goto done;	

// * Aviso: #Importante.
//
// Juntando os argumentos para a fun��o CreateWindow(.).
// Nesse caso, antes de chamarmos a fun��o CreateWindow, vamos juntar
// todos os argumentos que ela precisa. Mas o que acontece � que
// estamos na rotina de servi�os que o kernel oferece via API.
// Qual ser� a estrat�gia para conseguirmos os argumentos.
// >> Quando um aplicativo em user mode chamar CreateWindow, 
// a rotina da API deve fazer 4 system calls, as 3 primeiras passando 
// os argumentos e a �ltima criando a janela.
// >> do_create_window: � acionada depois de passados todos os argumentos. 
	
	
/*	
//
// ## Create window ##
//

do_create_window:	
	
	//?? @TODO
	//if(WindowType >= 0 || WindowType <= 5){
	//	
	//}	
	
	// @todo: bugbug.
	// Obs: A parent window � 'gui->screen'. Talvez essa estrutura nem esteja 
    // inicializada ainda no ambiente de logon.
	// Obs: O usu�rio s� pode criar janelas na �rea de trabalho do desktop atual.
    // @todo: A �rea de trabalho do desktop atual pode ser representada por
    // uma estrutura de ret�ngulo na estrutura do desktop e/ou uma estrutura de
    // ret�ngula na estrututra gui->.
    //      ...

	//
	// Importante: Checando se o esquema de cores est� funcionando.
	//
	
	if ( (void *) CurrentColorScheme == NULL )
	{
		printf("services: CurrentColorScheme");
		//refresh_screen();
		die ();
		
		//while(1){}
	
	}else{
		
		if ( CurrentColorScheme->used != 1 || 
		     CurrentColorScheme->magic != 1234 )
		{
		    
			printf("services: CurrentColorScheme validation");
		    //refresh_screen();
		    die();
			
			//while(1){}			
		};
		//Nothing.
	};
	

	// Nesse momento estamos atribuindo a cor padr�o de janelas 
	// para as janelas que criadas pelos aplicativos atrav�s de system calls.
	// N�o h� problema nisso por enquanto.
	
	// *Podemos aceitar as op��es de cores passadas por argumento.
	//  principalmente a cor da �rea de cliente.

	
	//*Importante
	//definimos as cores psdr�o caso a flag esteja desligada.
	WindowColor = CurrentColorScheme->elements[csiWindowBackground];  
	WindowClientAreaColor = CurrentColorScheme->elements[csiWindow];  

	// * Primeira coisa a fazer � ver se os argumentos est�o dispon�veis.
	//   Vamos conferir a flag que indica que argumentos foram enviados 
	//   previamente.
	
	// Se a flag tiver acionada, os argumentos usar�o os valores 
	// que foram previamente passados
	
	if (cwFlag == 1234)
	{		
        WindowType = cwArg1; 
		WindowStatus = cwArg2; 
		WindowView = cwArg3; 
		WindowName = (char *) cwArg4; 
	    
		WindowX = cwArg5; 
		WindowY = cwArg6; 
		
		WindowWidth = cwArg7; 
        WindowHeight = cwArg8;									  
		
		//#todo
		//gui->screen  = cwArg9; 
		//desktopID = cwArg10; 
		
		WindowClientAreaColor = (unsigned long) cwArg11;  //Obs: A cor da �rea de cliente ser� escolhida pelo app.   
		WindowColor = (unsigned long) cwArg12;            //a cor da janela escolhida pelo app. 
	};	
	

    // Importante:
	// Nesse momento � fundamental saber qual � a parent window da janela que vamos criar 
	// pois a parent window ter� as margens que nos guiam.
	// Essa parent window pode ser uma aba de navegador por exemplo.
	// >> O aplicativo deve enviar o ponteiro da janela m�e.
	
	//Criando uma janela, mas desconsiderando a estrutura rect_d passada por argumento.
	//@todo: #bugbug a estrutura rect_d apresenta problema quando passada por argumento
	//com um endere�o da �rea de mem�ria do app.
	
    NewWindow = (void *) CreateWindow ( WindowType, WindowStatus, 
	                        WindowView, WindowName, 
	                        WindowX, WindowY, WindowWidth, WindowHeight,									  
							cwArg9, desktopID, 
							(unsigned long) WindowClientAreaColor, 
							(unsigned long) WindowColor );
	
	if ( (void *) NewWindow == NULL )
	{ 
        //?? Mensagem.
	    return NULL; 
		
	}else{	
        
		
		//se a janela foi criada com sucesso, podemos desativar a flag.
		//*importante, nesse momento precisamos desativar a flag.
		
		cwFlag = 0;                  
		
        // Obs: 
		// Quem solicitou a cria��o da janela pode estar em user mode
        // por�m a estrutura da janela est� em kernel mode. #bugbug
		// Obs:
		// Para preencher as informa��es da estrutura, a aplica��o
		// pode enviar diversas chamadas, Se n�o enviar, ser�o considerados
		// os valores padr�o referentes ao tipo de janela criada.
		// Cada tipo tem suas caracter�sticas e mesmo que o solicitante
		// n�o seja espec�fico nos detalhes ele ter� a janela do tipo que deseja.	
		
        //  
        //@todo: Pode-se refinar os par�metros da janela na estrutura.
		//NewWindow->
		//...
		
		//@todo: N�o registrar, quem criou que registre a janela.
		//RegisterWindow(NewWindow);
		
		//
		// Se a tarefa atual est� pintando, vamos melhorar a sua prioridade.
		//
		
		t = (void *) threadList[current_thread];
		
		set_thread_priority ( t, PRIORITY_HIGH4 );
		
	    return (void *) NewWindow; 
	};
	
	*/

//More ...
//xxx:
//yyy:
//zzz:

    //    No caso de um aplicativo ter chamado essa rotina, 
	// o retorno ser� para o ISR da int 200, feito em assembly.
    //    No caso do kernel ter chamado essa rotina, apenas retorna.
	
//Done.
done:
    //Debug.
    //printf("Done\n",number);
	//refresh_screen();
    return NULL;	
}


unsigned long serviceCreateWindow ( char *message_buffer ){
	
	unsigned long *message_address = (unsigned long *) message_buffer;
	
	//Ponteiro para a janela criada pelo servi�o.
    struct window_d *NewWindow;  
		
	cwArg1 = message_address[0];             // Type. 		
	cwArg2 = message_address[1];             // WindowStatus 
	cwArg3 = message_address[2];             // view
	cwArg4 = (char *) message_address[3];    // a4 Window name.
	cwArg5 = message_address[4];             // x
	cwArg6 = message_address[5];             // y
	cwArg7 = message_address[6];             // width
	cwArg8 = message_address[7];             // height
		
	//parent window.
	//message_address[8];
	//cwArg9 = gui->screen;  //@todo: O argumento arg4 est� enviando parent window. 		
	cwArg9 = (struct window_d *) message_address[8];  //parent
		
	//onde?
	//message_address[9];
	//cwArg10 = arg4;  //desktop ID 		
		
	cwArg11 = message_address[10];    //cor da area de cliente.
	cwArg12 = message_address[11];    //cor da janela.	
	
	   //==========
	
	//Window.
	unsigned long WindowType;      //Tipo de janela.
	unsigned long WindowStatus;    //Status, ativa ou n�o.
	//unsigned long WindowRect;
	unsigned long WindowView;      //Min, max.
	char *WindowName;  
	
	unsigned long WindowX = (2*(800/20));  //100;   >          
	unsigned long WindowWidth  = 320;               
	
	unsigned long WindowY = (2*(600/20)); //100;   V                
	unsigned long WindowHeight = 480;  
	
	unsigned long WindowClientAreaColor = COLOR_WINDOW;  
	unsigned long WindowColor = COLOR_WINDOW;  
	
	
	//#todo: Checar a validade da esturtura,
	//WindowClientAreaColor = CurrentColorScheme->elements[csiWindow];  
	//WindowColor = CurrentColorScheme->elements[csiWindowBackground];  
	
	WindowType = cwArg1; 
	WindowStatus = cwArg2; 
	WindowView = cwArg3; 
	WindowName = (char *) cwArg4; 

	WindowX = cwArg5; 
	WindowY = cwArg6; 

	WindowWidth = cwArg7; 
	WindowHeight = cwArg8;

	//#todo
	//gui->screen  = cwArg9; 
	//desktopID = cwArg10; 

	WindowClientAreaColor = (unsigned long) cwArg11;  //Obs: A cor da �rea de cliente ser� escolhida pelo app.   
	WindowColor = (unsigned long) cwArg12;     


	struct thread_d *t;
	int desktopID;
	
	
	desktopID = (int) get_current_desktop_id();	
	
    // Importante:
	// Nesse momento � fundamental saber qual � a parent window da janela que vamos criar 
	// pois a parent window ter� as margens que nos guiam.
	// Essa parent window pode ser uma aba de navegador por exemplo.
	// >> O aplicativo deve enviar o ponteiro da janela m�e.
	
	//Criando uma janela, mas desconsiderando a estrutura rect_d passada por argumento.
	//@todo: #bugbug a estrutura rect_d apresenta problema quando passada por argumento
	//com um endere�o da �rea de mem�ria do app.
	
    NewWindow = (void *) CreateWindow ( WindowType, WindowStatus, 
	                        WindowView, WindowName, 
	                        WindowX, WindowY, WindowWidth, WindowHeight,									  
							cwArg9, desktopID, 
							(unsigned long) WindowClientAreaColor, 
							(unsigned long) WindowColor );
	
	if ( (void *) NewWindow == NULL )
	{ 
        //?? Mensagem.
	    //return NULL; 
		return 0;
		
	}else{	
        
		
		//se a janela foi criada com sucesso, podemos desativar a flag.
		//*importante, nesse momento precisamos desativar a flag.
		
		cwFlag = 0;                  
		
        // Obs: 
		// Quem solicitou a cria��o da janela pode estar em user mode
        // por�m a estrutura da janela est� em kernel mode. #bugbug
		// Obs:
		// Para preencher as informa��es da estrutura, a aplica��o
		// pode enviar diversas chamadas, Se n�o enviar, ser�o considerados
		// os valores padr�o referentes ao tipo de janela criada.
		// Cada tipo tem suas caracter�sticas e mesmo que o solicitante
		// n�o seja espec�fico nos detalhes ele ter� a janela do tipo que deseja.	
		
        //  
        //@todo: Pode-se refinar os par�metros da janela na estrutura.
		//NewWindow->
		//...
		
		//@todo: N�o registrar, quem criou que registre a janela.
		//RegisterWindow(NewWindow);
		
		//
		// Se a tarefa atual est� pintando, vamos melhorar a sua prioridade.
		//
		
		t = (void *) threadList[current_thread];
		
		set_thread_priority ( t, PRIORITY_HIGH4 );
		
		return (unsigned long) NewWindow;
	    //return (void *) NewWindow; 
	};	
}


// Coloca um char usando o 'terminal mode' de stdio.
// selecionado em _outbyte.
// stdio_terminalmode_flag = n�o transparente.
// Chama fun��o interna.

void servicesPutChar ( int c ){
	
	int cWidth = get_char_width ();
	int cHeight = get_char_height ();
	
	if ( cWidth == 0 || cHeight == 0 )
	{
		//#debug
		printf ("servicesPutChar: fail w h ");
		die();
	}
	
    stdio_terminalmode_flag = 1;  
	
	putchar ( (int) c );
	refresh_rectangle ( g_cursor_x * cWidth, g_cursor_y * cHeight, 
		cWidth, cHeight );
	
	stdio_terminalmode_flag = 0;  
}
 
 
/*
 * servicesChangeProcedure:
 *     Fun��o interna. 
 *     Atende a interrup��o 201, mudando o procedimento de janela atual.
 *     @todo: Passar argumento via registrador. ??
 *     @todo: Outra fun��o j� esta fazendo isso, deletar essa.
 */

void servicesChangeProcedure (){	
	//return;
}


//
// Fim.
//

