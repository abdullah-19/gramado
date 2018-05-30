/*
 * File: unblocked\ldisc.c
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


#include <kernel.h>


 

//=======================================================
//++ Usadas pelo mouse.
// hardwarelib.inc
//
#define MOUSE_X_SIGN	0x10
#define MOUSE_Y_SIGN	0x20

//Coordenadas do cursor.
extern int mouse_x;
extern int mouse_y;

//Bytes do controlador.
extern char mouse_packet_data;
extern char mouse_packet_x;
extern char mouse_packet_y;
//extern char mouse_packet_scroll;
 
extern void update_mouse();

//usado pelo mouse.
int mouse_buttom_1; 
int mouse_buttom_2;
int mouse_buttom_3;

//--
//=========================================================



//?? usado pelo mouse
#define outanyb(p) __asm__ __volatile__( "outb %%al,%0" : : "dN"((p)) : "eax" )


/*
 aqui n�o � o lugar disso.
char*  cursor[] =
{
    "1..........",
    "11.........",
    "121........",
    "1221.......",
    "12221......",
    "122221.....",
    "1222221....",
    "12222221...",
    "122222221..",
    "1222222221.",
    "12222211111",
    "1221221....",
    "121.1221...",
    "11..1221...",
    "1....1221..",
    ".....1221..",
    "......11..."
};

*/


/*
 aqui n�o � o lugar disso.
static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};

*/

//
// Imported functions.
//

//
// Defini��es para uso interno do m�dulo.
//





//
// Ports:
// =====
//     The entire range for the keyboard is 60-6F,
//     a total of 16 values (a 16bit range).
//
//  @todo:
//      As portas do controlador ainda est�o subutilizadas.
//      fazer um driver mais completo utilizando melhor o controlador.
//


//
//Command Listing:
//================
//Command	Descripton
//0xED	Set LEDs
//0xEE	Echo command. Returns 0xEE to port 0x60 as a diagnostic test
//0xF0	Set alternate scan code set
//0xF2	Send 2 byte keyboard ID code as the next two bytes to be read from port 0x60
//0xF3	Set autrepeat delay and repeat rate
//0xF4	Enable keyboard
//0xF5	Reset to power on condition and wait for enable command
//0xF6	Reset to power on condition and begin scanning keyboard
//0xF7	Set all keys to autorepeat (PS/2 only)
//0xF8	Set all keys to send make code and break code (PS/2 only)
//0xF9	Set all keys to generate only make codes
//0xFA	Set all keys to autorepeat and generate make/break codes
//0xFB	Set a single key to autorepeat
//0xFC	Set a single key to generate make and break codes
//0xFD	Set a single key to generate only break codes
//0xFE	Resend last result
//0xFF	Reset keyboard to power on state and start self test


//issso pertence � inicializa��o do teclado. deve ficar no driver de teclado.
/* Keyboard Commands */
#define KBD_CMD_SET_LEDS	    0xED	// Set keyboard leds.
#define KBD_CMD_ECHO     	    0xEE
#define KBD_CMD_GET_ID 	        0xF2	// get keyboard ID.
#define KBD_CMD_SET_RATE	    0xF3	// Set typematic rate.
#define KBD_CMD_ENABLE		    0xF4	// Enable scanning.
#define KBD_CMD_RESET_DISABLE	0xF5	// reset and disable scanning.
#define KBD_CMD_RESET_ENABLE   	0xF6    // reset and enable scanning.
#define KBD_CMD_RESET		    0xFF	// Reset.
//#define RESET  0xFE


/*
enum KYBRD_CTRL_STATS_MASK {
 
	KYBRD_CTRL_STATS_MASK_OUT_BUF	=	1,		//00000001
	KYBRD_CTRL_STATS_MASK_IN_BUF	=	2,		//00000010
	KYBRD_CTRL_STATS_MASK_SYSTEM	=	4,		//00000100
	KYBRD_CTRL_STATS_MASK_CMD_DATA	=	8,		//00001000
	KYBRD_CTRL_STATS_MASK_LOCKED	=	0x10,		//00010000
	KYBRD_CTRL_STATS_MASK_AUX_BUF	=	0x20,		//00100000
	KYBRD_CTRL_STATS_MASK_TIMEOUT	=	0x40,		//01000000
	KYBRD_CTRL_STATS_MASK_PARITY	=	0x80		//10000000
};

//! sets leds
void keyboard_set_leds( int num, int caps, int scroll) 
{ 
	char data = 0;
 
	//! set or clear the bit
	data = (char) (scroll) ? (data | 1) : (data & 1);
	data = (char) (num)    ? (num | 2)  : (num & 2);
	data = (char) (caps)   ? (num | 4)  : (num & 4);
 
	//! send the command -- update keyboard Light Emetting Diods (LEDs)
	kybrd_enc_send_cmd (KYBRD_ENC_CMD_SET_LED);
	kybrd_enc_send_cmd (data);
	
done:
    return;
};

//! send command byte to keyboard encoder
void kybrd_enc_send_cmd (uint8_t cmd) {
 
	//! wait for kkybrd controller input buffer to be clear
	while (1)
		if ( (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
			break;
 
	//! send command byte to kybrd encoder
	outportb (KYBRD_ENC_CMD_REG, cmd);
}
//! read status from keyboard controller
uint8_t kybrd_ctrl_read_status () {
 
	return inportb (KYBRD_CTRL_STATS_REG);
}
*/


//
// Vari�veis internas
//
//int keyboardStatus;
//int keyboardError;
//...


//Status
//@todo: Status pode ser (int).
//vari�veis usadas pelo line discipline para controlar o estado das teclas de controle.
unsigned long key_status;
unsigned long escape_status;
unsigned long tab_status;
unsigned long winkey_status;  // >> Winkey shotcuts. #super
unsigned long ctrl_status;
unsigned long alt_status;
unsigned long shift_status;
unsigned long capslock_status;
unsigned long numlock_status;
unsigned long scrolllock_status;
//...

//
// ** kernel Winkey shotcuts **
//

/*
 WINKEY+
 ...
 */

//
// Mouse support
//



//bytes do controlador.
char mouse_status;
char delta_x;
char delta_y;

//coordenadas.
int mouse_pos_x;
int mouse_pos_y;


//unsigned char *mousemsg;


//@todo: fazer rotina de get status algumas dessas vari�veis.


//Se h� uma nova mensagem de teclado. 
int kbMsgStatus;



//
// keyboardMessage
//     Estrutura interna para mensagens.
//
struct keyboardMessage 
{
    unsigned char scancode;

    //??
    //@todo: Na verdade todo driver usar� estrutura de janela descrita na API 
    //que o driver use.

    //? hwnd;  
    int message;
    unsigned long long1;
    unsigned long long2;
};


//Pega o status das teclas de modifica��o.
unsigned long keyboardGetKeyState(unsigned char key)
{
	unsigned long State = 0;
	
	switch(key)
	{   
		case VK_LSHIFT: 
		    State = shift_status; 
			break;

	    case VK_LCONTROL:
		    State = ctrl_status;
		    break;

	    case VK_LWIN:
		    State = winkey_status;
		    break;

	    case VK_LMENU:
		    State = alt_status;
		    break;

	    case VK_RWIN:
		    State = winkey_status;
		    break;

	    case VK_RCONTROL:
		    State = ctrl_status;
		    break;
			
	    case VK_RSHIFT:
		    State = shift_status;
		    break;

	    case VK_CAPITAL:
		    State = capslock_status;
		    break;

	    case VK_NUMLOCK:
		    State = numlock_status;
		    break;
			
		case VK_SCROLL:
            State = scrolllock_status;
            break;			
			
		//...
	};

	//Nothing.
	
Done:
    return (unsigned long) State;		
};


/*
 * keyboardEnable:
 *     Enable keyboard.
 */
void keyboardEnable()
{
	//Wait for bit 1 of status reg to be zero.
    while( (inportb(0x64) & 2) != 0 )
	{
		//Nothing.
	};
	
	//Send code for setting Enable command.
    outportb(0x60,0xF4);
    //sleep(100);

done:
	return;
};


/*
 * keyboardDisable:
 *     Disable keyboard.
 */
void keyboardDisable()
{
	//Wait for bit 1 of status reg to be zero.
    while( (inportb(0x64) & 2) != 0 )
	{
		//Nothing.
	};
	
	//Send code for setting disable command.
    outportb(0x60,0xF5);
    //sleep(100);
	
done:
	return;
};


/*
 * keyboard_set_leds:
 *     Set keyboard flags.
 *     ED = Set led.
 */
// void keyboardSetLEDs(cahr flag)
void keyboard_set_leds(char flag)
{
	//@todo: filtro.

	//Wait for bit 1 of status reg to be zero.
    while( (inportb(0x64) & 2) != 0 ){
		//Nothing.
	};
	//Send code for setting the flag.
    outportb(0x60,0xED);            
    sleep(100);

	//Wait for bit 1 of status reg to be zero.
	while( (inportb(0x64) & 2) != 0 ){
	    //Nothing.	
	};
    //Send flag. 
	outportb(0x60,flag);
	sleep(100);
	
	//@todo mudar o status.
    //switch(flag)
    //{
		
	//}	

done:
	return;
};


 
/*
void keyboard();
void keyboard()
{
	//@todo: Create global.
	if(gKeyboardType == 1){
		abnt2_keyboard_handler();
	}
	//...
	return;
}
*/


/*
 ***********************************************
 * LINE DISCIPLINE
 * Funciona como um filtro.
 * Obs: Essa � a rotina principal desse arquivo, todo o resto 
 * poder� encontrar um lugar melhor.
 *
 */
int LINE_DISCIPLINE(unsigned char SC, int type)
{
    //
    // Step 0 - Declara��es de vari�veis.
    //

    //Vari�veis para tecla digitada.
    unsigned char scancode;
    unsigned char key;         //Tecla (uma parte do scancode).  
    unsigned long mensagem;    //arg2.	
    unsigned long ch;          //arg3 - (O caractere convertido para ascii).
    unsigned long status;      //arg4.  

    // Text mode support.
	// Tela para debug em RING 0.
    // unsigned char *screen = (unsigned char *) 0x000B8000;   
    unsigned char *screen = (unsigned char *) SCREEN_START;    //Virtual.   
    //...
	
	//Window.
    struct window_d *wFocus;	


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
	if(scStatus == 1){
	    printf("{%d,%x}\n",scancode,scancode);
	};
	
	
    //
    // Step 2 - Tratar as mensagens.
    //

    //Se a tecla for (liberada).
	//D� '0' se o bit de paridade for '0'.
    if( (scancode & LDISC_KEY_RELEASED) == 0 )
	{
	    key = scancode;
		key &= LDISC_KEY_MASK;    //Desativando o bit de paridade caso esteja ligado.

		//Configurando se � do sistema ou n�o.
		//@todo: Aqui podemos chamar uma rotina interna que fa�a essa checagem.
		switch(key)
		{
			//Os primeiros 'case' � quando libera tecla do sistema.
			//O case 'default' � pra quando libera tecla que n�o � do sistema.
			
			//left Shift liberado.
			case VK_LSHIFT:
				shift_status = 0;
				mensagem = MSG_SYSKEYUP;
			    break;

			//Left Control liberado.			
			case VK_LCONTROL:
				ctrl_status = 0;
				mensagem = MSG_SYSKEYUP;
				break;

			//left Winkey liberada.
			case VK_LWIN:
			    winkey_status = 0;
                mensagem = MSG_SYSKEYUP;
				break;

			//Left Alt liberado.
            case VK_LMENU:
				alt_status = 0;
				mensagem = MSG_SYSKEYUP;
			    break;				
			
			//right winkey liberada.
			case VK_RWIN:
			    winkey_status = 0;
                mensagem = MSG_SYSKEYUP;
				break;

			//control menu.
			case VK_CONTROL_MENU:
			    //controlmenu_status = 0; //@todo
			    mensagem = MSG_SYSKEYUP;
			    break;

            //right control liberada.
			case VK_RCONTROL:
				ctrl_status = 0;
				mensagem = MSG_SYSKEYUP;
				break;
				
			//right Shift liberado.
			case VK_RSHIFT:
				shift_status = 0;
				mensagem = MSG_SYSKEYUP;
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
			    mensagem = MSG_SYSKEYUP;
			    break;


			//...
				
			//A tecla liberada N�O � do sistema.
			default:
			    mensagem = MSG_KEYUP;
				break;
		};

		//Selecionando o char para os casos de tecla liberada.

        //Analiza: Se for do sistema usa o mapa de caracteres apropriado. 
   		if(mensagem == MSG_SYSKEYUP)
		{
			//Normal.
			ch = map_abnt2[key];
		};

		//Analiza: Se for tecla normal, pega o mapa de caracteres apropriado.
		if(mensagem == MSG_KEYUP)
		{
		    //checar os mudificadores para mai�scula.
		    //if(shift_status == 1 || capslock_status == 1)
			//{	
			//    ch = shift_abnt2[key];	
			//	goto done;
			//};
			
			//min�scula
			//Nenhuma tecla de modifica��o ligada.
			ch = map_abnt2[key];
			goto done;
			
            //Nothing.
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
		switch(key)
		{
			//back space ser� tratado como tecla normal
			
			//@todo: tab,
            //?? tab ser� tratado como tecla normal por enquanto.
			
			//caps lock keydown
			case VK_CAPITAL:
			    //muda o status do capslock n�o importa o anterior.
				if(capslock_status == 0){ 
				    capslock_status = 1; 
					mensagem = MSG_SYSKEYDOWN; 
					break; 
				};
				if(capslock_status == 1){ 
				    capslock_status = 0; 
					mensagem = MSG_SYSKEYDOWN; 
					break; 
				};
				break; 

			//Left shift pressionada.
			case VK_LSHIFT:
			//case KEY_SHIFT:
				shift_status = 1;
				mensagem = MSG_SYSKEYDOWN;
			    break;

			//left control pressionada.
			case VK_LCONTROL:
			//case KEY_CTRL:
				ctrl_status = 1;
				mensagem = MSG_SYSKEYDOWN;
				break;

			//Left Winkey pressionada.
			case VK_LWIN:
			    winkey_status = 1;
				mensagem = MSG_SYSKEYDOWN;
				break;

            //left Alt pressionada.
            case VK_LMENU:
				alt_status = 1;
				mensagem = MSG_SYSKEYDOWN;
			    break;

			//@todo alt gr.	

			//Right Winkey pressionada.
			case VK_RWIN:
			    winkey_status = 1;
				mensagem = MSG_SYSKEYDOWN;
				break;
			
            //@todo: Control menu.
            
			//Right control pressionada.
			case VK_RCONTROL:
				ctrl_status = 1;
				mensagem = MSG_SYSKEYDOWN;
				break;

			//Right shift pressionada.
			case VK_RSHIFT:
				shift_status = 1;
				mensagem = MSG_SYSKEYDOWN;
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
			    mensagem = MSG_SYSKEYDOWN;
			    break;


			//Num Lock.	
		    case VK_NUMLOCK:
			    //muda o status do numlock n�o importa o anterior.
				if(numlock_status == 0){
		            numlock_status = 1;
					mensagem = MSG_SYSKEYDOWN;
					break;
				};
				if(numlock_status == 1){ 
				    numlock_status = 0;
                    mensagem = MSG_SYSKEYDOWN; 					
					break; 
				};
			    break;
				
			//Scroll Lock.	
		    case VK_SCROLL:
			    //muda o status do numlock n�o importa o anterior.
				if(scrolllock_status == 0){
		            scrolllock_status = 1;
					mensagem = MSG_SYSKEYDOWN;
					break;
				};
				if(scrolllock_status == 1){ 
				    scrolllock_status = 0;
                    mensagem = MSG_SYSKEYDOWN; 					
					break; 
				};
			    break;

            //...

			//A tecla pressionada n�o � do sistema.
			default:
			    //printf("keyboard debug: default: MSG_KEYDOWN\n");
			    mensagem = MSG_KEYDOWN;
				break;
		};

		if(mensagem == MSG_SYSKEYDOWN)
		{
			//uma tecla do sistema foi pressionada.
            //poderiamos ter op��es dependendo do status das modificadoras.
			ch = map_abnt2[key];
            goto done;
		};

		if(mensagem == MSG_KEYDOWN)
		{
			//uma tecla normal foi pressionada.
			//mensagem de diita��o.
			
			//Se os shift o capslock estiverem acionado ela vira mai�scula.
		    //if(shift_status == 1 || capslock_status == 1)
			//{
			//    ch = shift_abnt2[key];
			//    goto done;
			//};

			//min�sculas.
		    ch = map_abnt2[key];
			goto done;
			
            //Nothing.
		};
		//Nothing.
		goto done;
	};//fim do else

    //Nothing.
	
	
	//
	// ++ Para finalizar, vamos enviar a mensagem para fila certa ++
	//
		

//Done.
done:

	/*
	 * Debug:
     *     No caso de modo texto.
	 *
	 * Coloca o scancode na tela
     * set_up_cursor(0,4);
	 * printf("       "); 	
     * set_up_cursor(0,4);
	 * printf("%d ",(unsigned char) (key & 0xff) );
     * Coloca o caractere na tela
	 * screen[76] = (char) ch;
	 * screen[77] = (char) 0x09;  //azul no preto
	 */


	//Debug:
	//Canto direito da primeira linha.
	//screen[76] = (char) ch;
	//screen[77] = (char) 0x09;    //Azul no preto.

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
	
	if( (ctrl_status == 1) && (alt_status == 1) && (ch == KEY_DELETE) ){
		services( SYS_REBOOT, 0, 0, 0);
		//system_procedure ...
		// @todo: Chamar o aplicativo REBOOT.BIN.
	};

	
	//
	// Nesse momento temos duas op��es:
	// Devemos saber se a janela com o foco de entrada � um terminal ou n�o ...
	// se ela for um terminal chamaremos o porcedimento de janelas de terminal 
	// se ela n�o for um terminal chamaremos o procedimento de janela de edit box. 
	// que � o procedimento de janela do sistema.
	// *IMPORTANTE: ENQUANTO O PROCEDIMENTO DE JANELA DO SISTEMA TIVER ATIVO,
	// MUITOS COMANDOS N�O V�O FUNCIONAR ATE QUE SAIAMOS DO MODO TERMINAL.
	//
	//
	
		//
		// *importante:
		// Passamos a mensagem de teclado para o procedimento de janela do sistema.
		// que dever� passar chamar o procedimento de janela da janela com o focod eentrada.
		//
		
		//
		// *importante:
		// Quem � o 'first responder' para evento de teclado.
		// A janela com o foco de entrada � o first responder para 
		// eventos de teclado, mas n�o para todo tipo de envento.		
		//
	
	
    //
	// #importante
	// Ok. A ideia agora � enviar a mensagem para a fila de mensagens do sistema.
	//
	
	// Envia as mensagens para os aplicativos intercepta-las
	//so mandamos mensagem para um aplicativo no estavo v�lido.	

	
	//Apenas checando a validade da janela com o foco de entrada.
	
	struct window_d *w;    //janela com o foco de entrada.(first responder ??)
	w = (void *) windowList[window_with_focus];
	
	if( (void*) w == NULL ){
		printf("LINE_DISCIPLINE: w");
		die();
	}else{
		
		if( w->used != 1 || w->magic != 1234 ){
			printf("LINE_DISCIPLINE: w magic");
			die();
		}
	};
	
	//
	// @todo: (( IMPORTANTE ))
	//
	// TRABALHANDO PARA COLOCAR A MENSAGEM NA FILA.
	// A inten��o � que a mensagem fique na fila de mensagens da 
	// janela com o foco de entrada, para que depois o aplicativo 
	// possa desenfileirar as mensagens.
	// Obs: A fila deve ser circular.
	//

	//Colocando a mensagem na fila de mensagens do sistema.
	
	//struct message_d* m;	
	
	//Preenchendo a estrutura de mensagem. 
		
	//?? object ??	
	//m->used = 1;
	//m->magic = 1234;
	//m->empty = 0;
	//m->window = (struct window_d *) w;
	//m->msg    = (int) mensagem;
	//m->long1  = (unsigned long) ch;
	//m->long2  = (unsigned long) ch;
	//...
	
	//
	// Tentando enviar para fila ...
	//
	
	//Circulando o offset para write.
	//system_message_write++; //index ++
	//if( system_message_write < 0 || 
	//    system_message_write >= SYSTEM_MESSAGE_QUEUE_MAX )
	//{
	//	system_message_write = 0;
	//}
			
	// Colocando na fila.
    // Colocando o ponteiro da estrutura de mensagem na fila do sistema.	
	//system_message_queue[system_message_write] = (unsigned long) m;
	
	//#importante (teste)
	//Testando outra fila ...
	//Colocando o ponteiro da estrutura na fila.
	//xenqueue( (void *) m, ld_keyboard_queue);
		
	//Chamando o procedimento de janelas do sistema.
	//Talvez isso seja apenas por enquanto...
	//O motivo disso � para no caso de erro do mecanismo de fila
	//tenhamos ainda algum evento de teclado sendo tratado pelo 
	//procedimento do sistema.
	//O procedimento de janelas do sistema vai tratar poucas teclas.
	//Obs: #importante. Isso talvez deva ser chamado depois do 
	//procedimento de janelas do aplicativo. Ou seja, chamado em user mode.
	
	// sm\sys\procedure.c
	
	
	//o procedimento tratar� as mensagens de sistema, colocar� o cham no 
	//stdin e imprimir� o char na tela.
	system_procedure(  w, 
	      (int) mensagem, 
		  (unsigned long) ch, 
		  (unsigned long) ch ); 

		  
    //coloca na janela main.
    //if( type == 2 )
	//{
		
	//};
		  
    //colocar� a mensagem na estrutura de janle para que o aplicativo pegue.
    windowSendMessage( (unsigned long) w, (unsigned long) mensagem, (unsigned long) ch, (unsigned long) ch );		  
	
    return (int) -1;
};


/*
 * KdGetWindowPointer:
 *     Retorna o ponteiro da estrutura de janela que pertence a thread.
 *     Dado o id de uma thread, retorna o ponteiro de estrutura da janela 
 * � qual a thread pertence.
 */
void *KdGetWindowPointer(int tid)
{
	struct thread_d *t;

	//@todo: filtrar argumento. Mudar para tid.
	//if(thread_id<0){}

	// Structure.
	t = (void*) threadList[tid];

	if( (void*) t == NULL ){
        return NULL;        //@todo: fail;
	};
// Done.
done:
	return (void*) t->window;
};


/*
 * KbGetMessage:
 *     Pega a mensagem na fila de mensagens na estrutura da thread
 * com foco de entrada.
 *
 * Na estrutura da thread com foco de entrada tem uma fila de mensagens.
 * Pegar a mensagem.
 * 
 * Para falha, retorna -1.
 *
 * @todo: bugbug: A mensagem deve estar na fila do processo, na
 *                estrutura do proceso. (Talvez n�o na thread e nem na janela.)
 *
 */
int KbGetMessage(int tid)
{   
	int ret_val;
	struct thread_d *t;
	
	//
	// Structure.
	t = (void*) threadList[tid];

	if( (void*) t != NULL ){
        ret_val = (int) t->msg;
	}else{
	    ret_val = (int) -1;    //Fail.
	};

// Done.
done:
	WindowProcedure->msgStatus = 0;    //Muda o status.
	return (int) ret_val;              //Retorna a mensagem.
};


/*
 * KbGetLongParam1:
 *    Pega o parametro "long1" do procedimento de janela de uma thread.
 */
unsigned long KbGetLongParam1(int tid)
{   	
	struct thread_d *t;
	
	// Structure.
	t = (void*) threadList[tid];

	if( (void*) t == NULL){
        return (unsigned long) 0;    //@todo: fail;
	};

// Done.
done:
    return (unsigned long) t->long1;
};

/*
 * KbGetLongParam2:
 *     Pega o parametro "long2" do procedimento de janela de uma thread.
 */
unsigned long KbGetLongParam2(int tid)
{
	struct thread_d *t;
	
	// Structure.
	t = (void*) threadList[tid];

	if( (void*) t == NULL){
        return (unsigned long) 0;    //@todo: fail;
	};

// Done.
done:
    return (unsigned long) t->long2;
};


/*
 * reboot: 
 *     @todo: essa rotina poder� ter seu pr�prio arquivo.
 *     Reboot system via keyboard port.
 *     ?? #bugbug Por que o reboot est� aqui ??
 *
 * *IMPORTANTE: a interface fechou o que tinha qe fechar,
 * hal chamou essa hotina para efetuar a parte de hardware reboot apenas.
 * @todo: Atribui��es.
 *
 * Atribui��es: 
 *     + Desabilitar as interrup��es.
 *     + Salvar registros.
 *     + Salvar programas abertos e n�o salvos.
 *     + Fechar todas tarefas antes.
 *     + Efetuar o tipo de reboot especificado.
 *    + Outras ...
 *
 */
void reboot()
{
    //
    //@todo: 
	// +criar uma variavel global que especifique o tipo de reboot.
    // +criar um switch para efetuar os tipos de reboot.
	// +criar rota de fuga para reboot abortado.
	// +Identificar o uso da gui antes de apagar a tela.
	//  modo grafico ou modo texto.
	//
	
	//
	// Video.
	//
	

	
	/*
	sleep(2000);
	//kclear(9);
    set_up_cursor(0,0);	
    set_up_text_color(0x0f, 0x09);
	printf("\n\n REBOOTING ...\n\n");


	//
	// Scheduler stuffs.
	//
	
	sleep(1000);
	printf("locking scheduler ...\n");
	scheduler_lock();
	
	//
	// Tasks.
	//
	
	//@todo: fazer fun��o com while. semelhante ao dead task collector.
	
	sleep(1000);
	printf("killing tasks ...\n");
	//kill_thread(current_task); 
	
	//
	// Final message.
	//
	
	sleep(1000);
	printf("turning off ...\n");
    
	
	refresh_screen();
	
	//
	// Interruo��es.
	//
	
	sleep(7000);
	asm("cli");
	
	*/
	
	
	// @todo: disable();
	
//
// Done.
//

done:
    hal_reboot();
	die();
};


//Get alt Status.
int get_alt_status(){
    return (int) alt_status;
};

//Get control status.
int get_ctrl_status(){
    return (int) ctrl_status;
};

 

int get_shift_status(){
    return (int) shift_status;	
}
 



/*
 * init_keyboard:
 *     ??
 *     Inicializa o driver de teclado.
 *
 *  @todo: enviar para o driver de teclado o que for de l�.
 *         criar a vari�vel keyboard_type ;;; ABNT2 
 */
// void keyboardInit()
void init_keyboard()
{
	//user.h
	ioControl_keyboard = (struct ioControl_d*) malloc( sizeof(struct ioControl_d) );
	
	if( (void*) ioControl_keyboard == NULL )
	{
		printf("ldsic-init_keyboard: ioControl_keyboard fail");
		die();
	}else{
	    
	    ioControl_keyboard->id = 0;
	    ioControl_keyboard->used = 1;
	    ioControl_keyboard->magic = 1234;
	    
		ioControl_keyboard->tid = 0;  //qual threa est� usando o dispositivo.
	    //ioControl_keyboard->
	};
	
	
    //int Type = 0;

    //
    // @todo: 
	//     Checar se o teclado � do tipo abnt2.   
	//     � necess�rio sondar par�metros de hardware,
	//     como fabricante, modelo para configirar estruturas 
	//     e vari�veis.
	//


/*
    switch(Type)
	{
	    //NULL
		case 0:	
		    break;
			
	    //Americano.
		case 1:	
		    break;

		//pt-ABNT2	
	    case 2:	
		    break;
			
		//...
		
		//Modelo americano
		default:	
		    break;
	}
	
*/
	//
	// Set abnt2.
	//

	abnt2 = (int) 1;

    //Checar quem est� tentando inicializar o m�dulo.    

	//model.
	
	//handler.
	
	//...

    //Key status.
	key_status = 0;
    escape_status = 0;
    tab_status = 0;
    winkey_status = 0;
    ctrl_status = 0;
    alt_status = 0;
    shift_status = 0;
	capslock_status = 0;
	scrolllock_status = 0;
	numlock_status = 0;
	//...

	//AE    Enable Keyboard Interface: clears Bit 4 of command register
	//      enabling keyboard interface.
	kbdc_wait(1);
	outportb(0x64,0xAE);   // Activar a primeira porta PS/2
	
	//reset
	kbdc_wait(1);
	outportb(0x60,0xFF);


	//Leds.
	//LED_SCROLLLOCK 
	//LED_NUMLOCK 
	//LED_CAPSLOCK  	
	keyboard_set_leds(LED_NUMLOCK);
	
	//...
	
	
	//Debug support.
	scStatus = 0;

done:
    g_driver_keyboard_initialized = (int) 1;
    return;
};


/*
 * Constructor.
int keyboardKeyboard(){
	;
};
*/


/*
 obs: definido acima.
int keyboardInit(){
	;
};
*/


//
// ********************** Mouse ************************
//

//
// Obs: 
// Precisamos de um lugar para as rotinas de mouse. Elas n�o devem ficar aqui.
// @todo: mouse.c 
//



/*
 ********************************************************
 * init_mouse:
 *     Inicializando o mouse no controlador 8042.
 *     Carregando o bmp para o curso do mouse.
 */		
int init_mouse()
{
    unsigned char response = 0;
    unsigned char deviceId = 0;
    int i; 
	int bruto = 1;  //M�todo.
	int mouse_ret;
	
	
	//user.h
	ioControl_mouse = (struct ioControl_d*) malloc( sizeof(struct ioControl_d) );
	
	if( (void*) ioControl_mouse == NULL )
	{
		printf("ldsic-init_mouse: ioControl_mouse fail");
		die();
	}else{
	    
	    ioControl_mouse->id = 0;
	    ioControl_mouse->used = 1;
	    ioControl_mouse->magic = 1234;
	    
		ioControl_mouse->tid = 0;  //qual threa est� usando o dispositivo.
	    //ioControl_mouse->
	};	
	
	//
	// Estamos espa�o para o buffer de mensagens de mouse.
	// mousemsg = ( unsigned char *) malloc(32);

		
	//Inicializando as vari�veis usadas na rotina em Assemly
    //em hardwarelib.inc
    
	mouse_x = 0;
	mouse_y = 0;
	
	//#bugbug: Essa inicializa��o est� travando o mouse.
	//fazer com cuidado.
	
	//#bugbug. Cuidado com essa inicializa�ao.
	g_mousepointer_width = 16;
	g_mousepointer_height = 16;
	
	//Coordenadas do cursor.
    //mouse_x = (800/2);
    //mouse_y = (600/2);

    //Bytes do controlador.
   // mouse_packet_data = 0;
   // mouse_packet_x = 0;
    //mouse_packet_y = 0;
    //mouse_packet_scroll = 0;
	
	
	//Mostraremos essa mensagem somente no ambiente de debug.
	
#ifdef KERNEL_VERBOSE	
    MessageBox(gui->screen, 1, "init_mouse:","initializing!");
#endif   
	
	//
	// Poderemos tentar de mais de um modo.
	// Obs: O modo bruto est� funcionando. 
	//
	
	
tryModoBruto:	
	
	//Modo bruto.
	//Obs: Esse modo est� funcionando.
	/*
	if(bruto == 1){
	    mouse_write(0xFF);
	    mouse_write(0xF6); 
	    mouse_write(0xF4); 
		//while(!0xFA)mouse_read();
		while (mouse_read() != 0xfa);   // ACK
	};
	*/
	
 // Reseta mouse (reset ? lento!)...
  // Espero pelo byte 0xaa que encerra a sequ?ncia
  // de reset!
  kbdc_wait(1);
  mouse_write(0xff);
  while (mouse_read() != 0xaa);

  // Restaura defaults do PS/2 mouse.
  kbdc_wait(1);
  mouse_write(0xf6);
  while (mouse_read() != 0xfa);


// TODO: Pode ser interessante diminuir a sensibilidade do mouse
  // aqui!!!

  // Habilita o mouse streaming
  // Interessante notar que, no modo streaming,
  // 1 byte recebido do PS/2 mouse gerar  uma IRQ...
  // Talvez valha a pena DESABILITAR o modo streaming
  // para colher os 3 dados de uma s� vez na IRQ!
  kbdc_wait(1);
  mouse_write(0xf4);
  while (mouse_read() != 0xfa);         // ACK
  
	
	//
	// Aqui podemos tentar outros modos mais completos.
	//
	
done:

    // Reabilitando as duas portas.
	
	// Ativar a primeira porta PS/2.
	kbdc_wait(1);
	outportb(0x64,0xAE);   

	// Ativar a segunda porta PS/2.
	kbdc_wait(1);
	outportb(0x64,0xA8); 


	//
	// ## BMP ##
	//
	
	//
	// Carregando o bmp do disco para a mem�ria
	// e apresentando pela primeira vez.
	//
	
	
    mouse_ret = (int) load_mouse_bmp();	
	if(mouse_ret != 0)
	{
		printf("ldisc-init_mouse: load_mouse_bmp");
		die();
	}
	
	
	
#ifdef KERNEL_VERBOSE		
    MessageBox(gui->screen, 1, "init_mouse:","Mouse initialized!");   
#endif  

    //initialized = 1;
    //return (kernelDriverRegister(mouseDriver, &defaultMouseDriver));	
	return (int) 0;
};


/*
 * mouse_write:
 *     Envia um byte para a porta 0x60.
 *     (Nelson Cole) 
 */
void mouse_write(unsigned char write)
{
	kbdc_wait(1);
	outportb(0x64,0xD4);
	kbdc_wait(1);
	outportb(0x60,write);
};


/*
 * mouse_read:
 *     Pega um byte na porta 0x60.
 *     (Nelson Cole) 
 */
unsigned char mouse_read()
{
	kbdc_wait(0);
	return inportb(0x60);
};


/*
 * kbdc_wait:
 *     Espera por flag de autoriza��o para ler ou escrever.
 *     (Nelson Cole) 
 */
void kbdc_wait(unsigned char type)
{
	if(type==0){
        while(!inportb(0x64)&1)outanyb(0x80);
    }else{
        while(inportb(0x64)&2)outanyb(0x80);
	};
};


/*
 ***************************************************
 * mouseHandler:
 *     Handler de mouse. 
 *
 * *Importante: 
 *     Se estamos aqui � porque os dados dispon�veis no controlador 8042 
 * pertencem ao mouse.
 *
 * @todo: Essa rotina n�o pertence ao line discipline.
 * Obs: Temos externs no in�cio desse arquivo.
 * 
 */
 
void mouseHandler()
{
	// ?? #bugbug.
	// ?? Porque isso � local. ??
	// ?? N�o tem um contador global para isso ??
	// E o fato de serem 'static' ???
	// Obs: A fun��o entra com esse mouse count zerado.
    static int count_mouse=0;
	
	// Buffer.
    static char buffer_mouse[3];
	
    // Coordenadas do mouse.
    // Obs: Isso pode ser global.
    // O tratador em assembly tem as vari�veis globais do posicionamento.
    int posX=0;
    int posY=0;	

    //Char para o cursor provis�rio.
    static char mouse_char[] = "T";

    //
	// Lendo um char no controlador.
	//

	buffer_mouse[count_mouse++] = mouse_read();		
	
	//
	// Contagem de interru��es:
	// Obs: Precisamos esperar 3 interrup��es.
	//
	
	if( count_mouse >= 3 )
	{
		// Salvando os bytes obtidos.
		// Isso garante os dados que ser�o usados pelo assembly, mas n�o 
		// queremos que eles sejam modificados antes de chegarem l�.
		
        mouse_packet_data = buffer_mouse[0];
		mouse_packet_x = buffer_mouse[1];       
		mouse_packet_y = buffer_mouse[2];
		//mouse_packet_scroll = buffer_mouse[3];  // Suspenso.
		
		
		//
		//@todo:
		// Nessa hora podemos dividir os deltas.
		// Essa vari�vel divisora pode ser usada para configura��o.
		// @todo: Subistituir esse '2' por uma vari�vel global.
		//
		
		//#bugbug:
		// Isso n�o apresentou um bom resultado.
		// apresentando descontinuidade no tra�o.
		//mouse_packet_x = (mouse_packet_x/2);
		//mouse_packet_y = (mouse_packet_y/2); 



		//
        // Obs:
        // salvando o estado dos bot�es.
        // Isso n�o deve causar problemas.		
		// ?? Ser� que essas opera��es corrompem os dados ??
		// 
		
		if( ( mouse_packet_data & 0x01 ) == 0 )
		{
			//liberada.
			mouse_buttom_1 = 0;
		}else if( ( mouse_packet_data & 0x01 ) != 0 )
		      {
				  //pressionada.
				  mouse_buttom_1 = 1;
			  }
			  

	    if( ( mouse_packet_data & 0x02 ) == 0 )
		{
		    //liberada.
		    mouse_buttom_2 = 0;
		}else if( ( mouse_packet_data & 0x02 ) != 0 )
		      {
				 //pressionada.
				 mouse_buttom_2 = 1;
			  }
			  
	    if( ( mouse_packet_data & 0x04 ) == 0 )
		{
		    //liberada.
		    mouse_buttom_3 = 0;
		}else if( ( mouse_packet_data & 0x04 ) != 0 )
		      {
		          //pressionada.
		          mouse_buttom_3 = 1;
		      }			  
			  

			  
		//
		// Chamando assembly para calcular as coodenadas.
		// Salvando os valores em vari�veis globais.
		//
		
		update_mouse();	
		
        // 
		// Pegando os valores encontrados calculados pela rotina acima.
        // Obs: mouse_x e mouse_y s�o vari�veis globais.
		//
		
		posX = mouse_x;
	    posY = mouse_y;
		
		// #debug:
		// #importante:
		// Mostrando os resultados obtidos.
		// printf("X={%d} Y={%d} \n",posX,posY);
		// refresh_screen();
		
		// Limits.
        if( posX < 0 ){ posX = 0; }	
		if(	posY < 0 ){ posY = 0; }
		if(	posX > 800-8 ){ posX = 800-8; }
		if(	posY > 600-8 ){ posY = 600-8; }
		
		//
		// Atualizando o mesmo cursor usado pelo teclado.
		//
		
		g_cursor_x = (unsigned long) posX;
		g_cursor_y = (unsigned long) posY;
		
		//
		// Draw !
		//
		
		//Imprimindo o caractere que est� servindo de ponteiro provis�rio.
        printf("%c", (char) '0'); 
		refresh_rectangle( g_cursor_x*8, g_cursor_y*8, 8, 8 );		
		
		// @todo:
		// ?? Porque n�o estamos testando o bmp como ponteiro ??
		// Talvez porque o bmp � 16x16.
		//bmpDisplayBMP( mouseBMPBuffer, g_cursor_x*8, g_cursor_y*8, 0, 0 );
		//refresh_rectangle( g_cursor_x*8, g_cursor_y*8, 16, 16 );
		
		//
		// ?? #bugbug: Porque essa vari�vel � local, dentro da fun��o.
		// Zerando a contagem de interrup��es de mouse.
		//
		
		count_mouse=0;
    };
	
	
	//
	// *importante:
    // Esse � o memento em que enviamos a mensagem !!
    //	
	
    //
	// *importante:
	// Devemos enviar a mensagem de mouse para quem. ??
	// First responder ?? (wwf)
	//
	
	// O aplicativo poder� interceptar mensgens como mouse hover.
	// mousehover � flutuando por cima do objeto.
	
	//
	// FIRST RESPONDER
	// 
	
	struct window_d *w;
	w = (void *) windowList[window_with_focus];
	
	if( (void*) w != NULL )
	{
		// Envia as mensagens para os aplicativos intercepta-las.
		if( w->used == 1 && w->magic == 1234 )
		{
			// ?? O que sabemos sobre o evento de mouse at� aqui ??
			// sabemos que ele se movimentou e temos as coordenadas dele.
			
			// se o mouse alcan�ar as coordenadas da janela com o foco de 
			// entrada podemos enviar uma mensagem para ela. assim ela 
			// pode criar uma janelinha que alerta sobre o mouse hover.
			// essa mesagem pode ser de mouse hover.
			
	        //windowSendMessage( 0, 0, 0, 0);
		};			
		
		//Chama o procedimento de janelas do sistema.
		//O procedimento de janela do terminal est� em cascata.
		//system_procedure( w, (int) 0, (unsigned long) 0, (unsigned long) 0 );					
	};		
	
	
	//
	// #importante 
	// Por outro lado o mouse deve confrontar seu posicionamento com 
	// todas as janelas, para saber se as coordenadas atuais est�o passando 
	// por alguma das janelas. Se estiver, ent�o enviaremos mensagens para essa 
	// janela que o mouse passou por cima. Ela deve ser sinalizada como hover,
	// 
	// #importante:
	// Se houver um click, o elemento com mousehover ser� afetado, e enviaremos,
	// mesagens para ele, se apertarem enter ou application key, quem 
	// recebe a mensagem � o first responder, ou seja. a janela com o focod e entrada.
	//
    // Se clicarmos com o bot�o da direita, quem recebe a mensagem � 
    // a janela que o mouse est� passando por cima.	
	//
	
	
exit_irq:	
    // EOI.		
    outportb(0xa0, 0x20); 
    outportb(0x20, 0x20);
};


// Input.
// Input a value from the keyboard controller's data port, after checking
// to make sure that there's some data there for us.
static unsigned char inPort60(void)
{
    unsigned char data = 0;

    while (!(data & 0x01))
        kernelProcessorInPort8(0x64, data);

    kernelProcessorInPort8(0x60, data);
	
done:  
  return (data);
};


// Output.
// Output a value to the keyboard controller's data port, after checking
// to make sure it's ready for the data.
static void outPort60(unsigned char value)
{
    unsigned char data;
  
    // Wait for the controller to be ready
    data = 0x02;
    while (data & 0x02)
        kernelProcessorInPort8(0x64, data);
  
    data = value;
    kernelProcessorOutPort8(0x60, data);
  
done:  
    return;
};


// Output.
// Output a value to the keyboard controller's command port, after checking
// to make sure it's ready for the command
static void outPort64(unsigned char value)
{
    unsigned char data;
  
    // Wait for the controller to be ready
    data = 0x02;
    while (data & 0x02)
        kernelProcessorInPort8(0x64, data);

    data = value;
    kernelProcessorOutPort8(0x64, data);
	
done:	
    return;
};


/*
 * getMouseData:
 *     Essa fun��o � usada pela rotina kernelPS2MouseDriverReadData.
 * Input a value from the keyboard controller's data port, after checking
 * to make sure that there's some mouse data there for us.
 */
static unsigned char getMouseData(void)
{
    unsigned char data = 0;

    while ((data & 0x21) != 0x21)
        kernelProcessorInPort8(0x64, data);

    kernelProcessorInPort8(0x60, data);
	
done:
    return (data);
};


/*
 * kernelPS2MouseDriverReadData:  
 *     Pega os bytes e salva em um array.
 *     Exibe um caractere na tela, dado as cordenadas.
 *     This gets called whenever there is a mouse interrupt
 *     @todo: Rever as entradas no array.         
 */
void kernelPS2MouseDriverReadData(void)
{

/*
 * #suspensa !	
	
    // Bot�es do mouse.
	static volatile int button1, button2, button3;
    
	// Bytes da mensagem.
	unsigned char byte1=0, byte2=0, byte3=0;
    
	// Posicionamento.
	int xChange, yChange;
	
	
	// Zera o estado do mouse.
	mousemsg[9] = 0;  

	//
	// Get bytes.
	//

getBytes:
	
    // The first byte contains button information and sign information
    // for the next two bytes
    byte1 = mouse_status;
	
    // The change in X position
    byte2 = delta_x;
	
    // The change in Y position
    byte3 = delta_y;
	
	//
	// Procedimento com base nos bytes.
	//
	
takeSomeAction:
	
    if( (byte1 & 0x01) != button1 )
    {
        // kernelMouseButtonChange(1, button1 = (byte1 & 0x01));
        mousemsg[2] = 1;
        mousemsg[9] = MSG_MOUSEKEYDOWN; // clicado ou nao
        return;
    }else if( (byte1 & 0x04) != button2 )
          {
              //  kernelMouseButtonChange(2, button2 = (byte1 & 0x04));
              mousemsg[2]= 2;
              mousemsg[9] = MSG_MOUSEKEYDOWN; // clicado ou nao                                      
              return;
          }else if( (byte1 & 0x02) != button3 )
                {
                    //  kernelMouseButtonChange(3, button3 = (byte1 & 0x02));
                    mousemsg[2]= 3;
                    mousemsg[9] = MSG_MOUSEKEYDOWN; // clicado ou nao                                      
                    return;
                }else{
                    
					//++=======
					// Sign them
                    if(byte1 & 0x10){
	                    xChange = (int) ((256 - byte2) * -1);
                    }else{ xChange = (int) byte2; };
					//--=======

					//++=======
                    if (byte1 & 0x20){
                        yChange = (int) (256 - byte3);
                    }else{ yChange = (int) (byte3 * -1); };
					//--=======

                    //  char_blt(xChange * 8,yChange * 8,55,'A'); 
                    //  kernelMouseMove(xChange, yChange);
                    //  mousemove(xChange, yChange);
                    
					my_buffer_char_blt( xChange, yChange, COLOR_PINK, 'T');
					
					mousemsg[0]= xChange;
                    mousemsg[1]= yChange;
                };
				
	//Nothing.	
	
*/
	
done:							
    return;
};


/* 
 * ***************************************************************
 * P8042_install:
 *     Configurando o controlador PS/2, 
 *     e activar a segunda porta PS/2 (mouse).
 *     (Nelson Cole)
 */
void P8042_install()
{
	unsigned char status;

    // Desativar dispositivos PS/2 , isto evita que os dispositivos PS/2 
	// envie dados no momento da configura��o.

desablePorts:
	
	// Desativar a primeira porta PS/2.
  	kbdc_wait(1);
	outportb(0x64,0xAD);  
	
	// Desativar a segunda porta PS/2, 
	// hahaha por default ela j� vem desativada, s� para constar
	kbdc_wait(1);
	outportb(0x64,0xA7); 

goAhead:
	
	 // Defina a leitura do byte actual de configura��o do controlador PS/2.
	kbdc_wait(1);    
	outportb(0x64,0x20);    

	// Activar o segundo despositivo PS/2, modificando o status de 
	// configura��o do controlador PS/2. 
	// Lembrando que o bit 1 � o respons�vel por habilitar, desabilitar o 
	// segundo despositivo PS/2  ( o rato). 
	// S� para constar se vedes aqui fizemos duas coisas lemos ao mesmo tempo 
	// modificamos o byte de configura��o do controlador PS/2 
	
	kbdc_wait(0);
	status = inportb(0x60)|2;  
	
	// defina, a escrita  de byte de configura��o do controlador PS/2.
	kbdc_wait(1);
	outportb(0x64,0x60);  

	// devolvemos o byte de configura��o modificado.
	kbdc_wait(1);
	outportb(0x60,status);  

	// Obs:
	// Agora temos dois dispositivos seriais teclado e mouse (PS/2).

    //
    // Reabilitando portas.
    //
	
enablePorts:
	
	// Ativar a primeira porta PS/2.
	kbdc_wait(1);
	outportb(0x64,0xAE);   

	// Ativar a segunda porta PS/2.
	kbdc_wait(1);
	outportb(0x64,0xA8);  

    //
	// Done!
	//
	
done:	
	// espera.
	// ?? Pra que isso ??
	kbdc_wait(1);  
    return;
    // NOTA. 
	// Esta configura��o discarta do teste do controlador PS/2 e de seus dispositivos. 
	// Depois fa�amos a configura��o decente e minuciosa do P8042.
};



/*
 **********************************************************
 * load_mouse_bmp:
 *     Carregando o arquivo MOUSE.BMP que � o ponteiro de mouse.
 *     Usar isso na inicializa��o do mouse.
 */
int load_mouse_bmp()
{
	int Status = 1;
	int Index;
    struct page_frame_d *pf;
	unsigned long fileret;
	
#ifdef KERNEL_VERBOSE
	printf("load_mouse_bmp:\n");
#endif	
	
	// #Importante
	// Fizemos um teste e funcionou com 500 p�ginas.
	// Foi suficiente para testarmos um pano de fundo.
	// Ret = (void*) allocPageFrames(500);  
	
    // Alocando duas p�ginas para um BMP pequeno. 8KB.	
	//mouseBMPBuffer = (void*) allocPageFrames(2);
	mouseBMPBuffer = (void*) allocPageFrames(10);
	if( (void*) mouseBMPBuffer == NULL )
	{
	    printf("unblocked-ldisc-load_mouse_bmp: mouseBMPBuffer\n");
		goto fail;		
	};
	
	
	// #debug
	//printf("\n");
	//printf("BaseOfList={%x} Showing #32 \n",mouseBMPBuffer);
    //for(Index = 0; Index < 32; Index++)   	
	//{  
    //    pf = (void*) pageframeAllocList[Index]; 
	//	
	//	if( (void*) pf == NULL ){
	//	    printf("null\n");	 
	//	}
	//   if( (void*) pf != NULL ){
	//	    printf("id={%d} used={%d} magic={%d} free={%d} handle={%x} next={%x}\n",pf->id ,pf->used ,pf->magic ,pf->free ,pf ,pf->next ); 	
	//	}
	//}
	
	//
	// ## Loading ...  ##
	//
	
	//read_fntos( (char *) arg1);
	
	
	//===================================
	// MOUSE
	fileret = (unsigned long) fsLoadFile( "MOUSE   BMP", 
	                              (unsigned long) mouseBMPBuffer );
								  
	if( fileret != 0 )
	{
		printf("MOUSE.BMP FAIL\n");		
		// Escrevendo string na janela.
		//draw_text( gui->main, 10, 500, COLOR_WINDOWTEXT, "MOUSE.BMP FAIL");
		goto fail;	
	};
	
	// Render BMP file on backbuffer.
	bmpDisplayBMP( mouseBMPBuffer, 20, 20 );
	refresh_rectangle( 20, 20, 16, 16 );	
    //===================================							
	
	
	//===================================
	//DENNIS
	fileret = (unsigned long) fsLoadFile( "DENNIS  BMP", 
	                              (unsigned long) mouseBMPBuffer );
								  
	if( fileret != 0 )
	{
		printf("DENNIS  BMP FAIL\n");		
		// Escrevendo string na janela.
		//draw_text( gui->main, 10, 500, COLOR_WINDOWTEXT, "MOUSE.BMP FAIL");
		goto fail;	
	};
	
	// Render BMP file on backbuffer.
	bmpDisplayBMP( mouseBMPBuffer, 20, 40 );	
	refresh_rectangle( 20, 40, 400, 400 );
	//===================================
	
	
refresh_rectangle:

	//Isso funcionou ...
	//refresh_rectangle( 20, 20, 16, 16 );
	Status = (int) 0;
	goto done;
	
fail:
    printf("fail\n");
    Status = (int) 1;		
done:

#ifdef KERNEL_VERBOSE
    printf("done\n");
#endif	
	//refresh_screen();
    return (int) Status;	
};


/*
 * ps2:
 *     Essa rotina de inicializa��o do controladro poder� ter seu pr�prio m�dulo.
 *     Inicializa o controlador ps2.
 *     Inicializa a porta do teclado no controlador.
 *     Inicializa a porta do mouse no controlador.
 *     Obs: *importante: A ordem precisa ser respeitada.
 *     As vezes os dois n�o funcionam ao mesmo tempo se a 
 *     inicializa��o n�o for feita desse jeito. 
 */
void ps2()
{
    P8042_install();  //dever� ir para ps2.c @todo: criar arquivo.
	
	//@todo: isso deveria se chamar init_ps2_mouse ...
    init_keyboard();  //?? quem inicializar� a porta do teclado ?? o driver ??
	init_mouse();	  //?? quem inicializar� a porta do mouse ?? o driver ??
	
};




void set_current_keyboard_responder( int i ){
	current_keyboard_responder = i;
};

int get_current_keyboard_responder(){
	return (int) current_keyboard_responder;
};


void set_current_mouse_responder( int i ){
    current_mouse_responder = i;	
};

int get_current_mouse_responder(){
    return (int) current_mouse_responder;	
};


//inicializando a fila de mensagens do sistema
//com ponteiros para estruturas de mensagens ...
//es estruturas ser�o reutiliz�veis.
void initialize_system_message_queue()
{
	struct message_d *m;
	
	int i;
	for( i=0; i<SYSTEM_MESSAGE_QUEUE_MAX; ++i )
	{
		
		m = (void*) malloc( sizeof(struct message_d) );
		if( (void*) m == NULL )
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
	
	//
	// ==================================================
	// #importante:   (teste)
	// initializaes keyboard queue
	//
	
	xinit_queue(ld_keyboard_queue);
	
    return;	
};

//==========================================
// marlls1989 - marcos - queue 

void xinit_queue(ld_queue_t *queue) {
	//pthread_mutex_init(&(queue->lock), NULL);
	//pthread_cond_init(&(queue->wait), NULL);

	queue->data = malloc(QUEUE_INITIAL_SIZE*sizeof(void*));
	queue->size = QUEUE_INITIAL_SIZE;
	queue->reads = queue->writes = 0;
}

void xdestroy_queue(ld_queue_t *queue) {
	//pthread_mutex_destroy(&(queue->lock));
	//pthread_cond_destroy(&(queue->wait));

	free(queue->data);
}

//size should always be a power of 2
static void xresize_queue(ld_queue_t *queue, size_t new_size) {
	void **new_data;
	size_t i, k;

	new_data = malloc(new_size*sizeof(void*));

	for(i = 0, k = queue->writes - queue->reads ; i  < k ; i++)
		new_data[i] = queue->data[(queue->reads+i) & (queue->size-1)];

	free(queue->data);

	queue->data = new_data;
	queue->size = new_size;
	queue->writes = k;
	queue->reads = 0;
}


// xenqueue - por na fila.
void xenqueue(void *element, ld_queue_t *queue)
{
	size_t s;
	//pthread_mutex_lock(&(queue->lock));

	if(!(s = queue->writes - queue->reads))
		queue->reads = queue->writes = 0;

	if(s >= queue->size)
		xresize_queue(queue, queue->size << 1);

	queue->data[(queue->writes++) & (queue->size-1)] = element;

	//pthread_cond_broadcast(&(queue->wait));
	//pthread_mutex_unlock(&(queue->lock));
};


// xdequeue - Retirar da fila.
void *xdequeue(ld_queue_t *queue)
{
	void *ret;
	//pthread_mutex_lock(&(queue->lock));

	//while(queue->writes == queue->reads) {
	//	queue->reads = queue->writes = 0;
	//	pthread_cond_wait(&(queue->wait), &(queue->lock));
	//}
	
	//inclu�do por fred.
	//if(queue->writes == queue->reads){
	//	queue->reads = queue->writes = 0;
	//	return NULL;
	//}

	ret = queue->data[(queue->reads++) & (queue->size-1)];

	//pthread_mutex_unlock(&(queue->lock));

	return (void*) ret;
};



// Inicializa o status das teclas de modifica��o.
// s�o usadas em comjunto com outras teclas para criar atalhos.
// modificam temporariamente a fun��o de outra tecla.
void ldisc_init_modifier_keys()
{
	// Modifier keys.
	
	// Shift.
	shift_status = 0;
	
	// Control.
	ctrl_status = 0;
	
	// Win key.
	winkey_status = 0;
    
	// Alternate.
    alt_status = 0;

	// Alternate Graphic.
    //altgr_status = 0; //@todo
    
	// Function.
	//fn_status = 0;  //@todo
	
	//...
};


// modificam permanentemente a fun��o de outra tecla.
//ativa as teclas extendidas.
void ldisc_init_lock_keys()
{
    // Capital Lock.	
	capslock_status = 0;
	
	// Scrolling Lock.
	scrolllock_status = 0;
	
	// Number Lock.
	numlock_status = 0;	
};




//
// End.
//

