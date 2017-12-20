/*
 * File: executive\dd\unblocked\ldisc.c
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


//
// Imported functions.
//

//
// Defini��es para uso interno do m�dulo.
//
 
#define KEYBOARD_DRIVER_VERSION "1.0"

#define KEY_RELEASED 0x80

#define KEY_MASK 0x7F
#define CHEIO 1
#define VAZIO 0
#define OBF   0x01    //Output buffer flag.


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
unsigned long key_status;
unsigned long escape_status;
unsigned long tab_status;
unsigned long winkey_status;  // >> Winkey shotcuts. #super
unsigned long ctrl_status;
unsigned long alt_status;
unsigned long shift_status;
unsigned long capslock_status;
unsigned long numlock_status;
//...

//
// ** kernel Winkey shotcuts **
//

/*
 WINKEY+
 ...
 */


 //mouse support
unsigned char *mousemsg;
unsigned char buffer_mouse[3];
unsigned char mouse_status;
unsigned char delta_x;
unsigned char delta_y;
int count_mouse;

//@todo: fazer rotina de get status algumas dessas vari�veis.


//Se h� uma nova mensagem de teclado. 
int kbMsgStatus;



//
// keyboardMessage
//     estrutura interna para mensagens.
//
struct keyboardMessage 
{
	unsigned char scancode;
	
	//hwnd;  //@todo: na verdade todo driver usar� estrutura de janela descrita na API que o driver use.
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
			
		//...
	};

	//Nothing.
	
Done:
    return (unsigned long) State;		
}


/*
 * keyboardEnable:
 *     Enable keyboard.
 */
void keyboardEnable()
{
	//Wait for bit 1 of status reg to be zero.
    while( (inportb(0x64) & 2) != 0 ){
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
    while( (inportb(0x64) & 2) != 0 ){
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
 *        LINE DISCIPLINE
 * funciona como um filtro.
 */
void LINE_DISCIPLINE(unsigned char SC)
{
    /*
	 * Step 0 - Declara��es de vari�veis.
	 */

	//Vari�veis para tecla digitada.
    unsigned char scancode;
	unsigned char key;         //Tecla (uma parte do scancode).  
    unsigned long mensagem;    //arg2.	
    unsigned long ch;          //arg3 - (O caractere convertido para ascii).
    unsigned long status;      //arg4.  
	 
	struct window_d *wFocus;

	//Tela para debug em RING 0.
    //unsigned char *screen = (unsigned char *) 0x000B8000;   
    unsigned char *screen = (unsigned char *) SCREEN_START;    //Virtual.   
	//...
	
	
    /*
     * Step1 - Pegar o scancode.
     */

	//scancode = inportb(0x60);    //@todo: usar constante. (retorno).
    
	scancode = SC;
	

	//Obs: Observe que daqui pra frente todas as rotinas poderiam estar
	//     em user mode.


    //Show the scancode if the flag is enabled. 
	if(scStatus == 1){
	    printf("{%d,%x}\n",scancode,scancode);
	};
	
	
    /*
     *  Step 2 - Tratar as mensagens.
     */

    //Se a tecla for liberada.
	//D� '0' se o bit de paridade fo '0'.
    if( (scancode & KEY_RELEASED) == 0 )
	{
	    key = scancode;
		key &= KEY_MASK;    //Desativando o bit de paridade caso esteja ligado.

		//Configurando se � do sistema ou n�o.
		//@todo: Aqui podemos chamar uma rotina interna que fa�a essa checagem.
		switch(key)
		{
			//Os primeiros 'case' � quando libera tecla do sistema.
			//O case 'default' � pra quando libera tecla que n�o � do sistema.

			//@todo: Tab, capslock.
			
			//left Shift liberado.
			case VK_LSHIFT:
			//case KEY_SHIFT:
				shift_status = 0;
				mensagem = MSG_SYSKEYUP;
			    break;

			//Left Control liberado.			
			case VK_LCONTROL:
			//case KEY_CTRL:
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

			//@todo: alt gr.

			//right winkey liberada.
			case VK_RWIN:
			    winkey_status = 0;
                mensagem = MSG_SYSKEYUP;
				break;

			//@todo: control menu.

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

			//@todo: aqui deve acionar o shift?
            //Talvez um switch.
		};

		//Analiza: Se for tecla normal, pega o mapa de caracteres apropriado.
		if(mensagem == MSG_KEYUP)
		{
		    //Normal.
			ch = map_abnt2[key];

			//Shift.
		    if(shift_status == 1){
			    ch = shift_abnt2[key];
			};

			//Control.
		    if(ctrl_status == 1){
			    ch = ctl_abnt2[key];
			};
            //Nothing.
		};
        //Nothing.
		goto done;
	}
	//else    // * Tecla pressionada ...........	
	
	
	if( (scancode & KEY_RELEASED) != 0 )
	{ 
		key = scancode;
		key &= KEY_MASK; //Desativando o bit de paridade caso esteja ligado.

		//O �ltimo bit � zero para key press.
		//Checando se � a tecla pressionada � o sistema ou n�o.
		//@todo: Aqui podemos chamar uma rotina interna que fa�a essa checagem.
		switch(key)
		{
			//@todo: tab,

			case VK_CAPITAL:
			    //muda o status do capslock n�o importa o anterior.
				if(capslock_status == 0)
				{ 
				    capslock_status = 1;
					keyboard_set_leds(LED_CAPSLOCK);
					break; 
				};
				if(capslock_status == 1){ capslock_status = 0; break; };
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
				if(numlock_status == 0)
				{
				    numlock_status = 1;
					keyboard_set_leds(LED_NUMLOCK);  //@retorno.
					break;
				};
				if(numlock_status == 1){ numlock_status = 0; break; };
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
			if(abnt2 == 1){
		        ch = map_abnt2[key];
			};

            //@todo acionar status. 
		};

		if(mensagem == MSG_KEYDOWN)
		{
			if(abnt2 == 1){
		        ch = map_abnt2[key];
			};

		    if(shift_status == 1 || capslock_status == 1){
			    ch = shift_abnt2[key];
			};

		    if(ctrl_status == 1){
			    ch = ctl_abnt2[key];
			};
            //Nothing.
		};
		//Nothing.
		goto done;
	};//fim do else

    //Nothing.

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
	if( (ctrl_status == 1) && (alt_status == 1) && (ch == KEY_DELETE) )
	{
		//Uma op��o aqui, � enviar para o aplicativo uma mensagem de reboot.
		//como o aplicativo n�o trata esse tipo de mensagem ele apenas reecaminha 
		//para o procedimentod e janelas do sistema.
		services( SYS_REBOOT, 0, 0, 0);
	};

	
	//
	// ?? D�vida: E se o aplicativo em user mode n�o tem janela,
	// para onde vai as mensagens de digita��es que s�o recebidas 
	// pelas rotinas de input dos aplicativos ?? Onde as rotinas 
	// de input dos aplcativos de console pegam suas mensagens??
	// ?? Ou seja: ?? Qual � a janela que recebe mensagens quando 
	// o aplicativo que est� rodando � um aplicativo de console??
	// >>> A solu��o seria enviar a mensagem para a janela de terminal
	// criada pelo kernel para o aplicativo de console... Desse modo 
	// at� um aplicativo de console ter� uma janela de associada a ele.
	// podendo enviar mensagens para ela, e receber mensagens atrav�s da
	// fila de mensagens da janela.
	// Obs: Quando um aplicativo de console recebe tempo de processamento,
	// sua janela pode ou n�o estar com o foco de entrada, pois aplicativos 
	// podem rodar em segundo plano, at� mesmo minimizados.
	// ?? Pergunta: ?? como fazer ent�o para enviar mensagens para janelas de 
	// aplicativos que est�o rodando em segundo plano, sem o foco de entrada??
	// >> As mensagens que vem com o destinat�rio explicitado s�o mais f�ceis de 
    // serem enviadas. Para isso podemos utilizar canais de mensagens que 
	// indiquem quem s�o as janelas do emissor e do receptor. 
	// No caso de mensagens enviadas entre processos, os processos ser�o 
	// os emossores e receptores da mensagem, nesse caso ent�o a mensagem 
	// ser� enviada � janela principal do processo emissor. Para isso 
	// na estrutura de processo deve ter um ponteiro para a janela principal,
	// que ser� respons�vem por receber as emnsagens no caso de uma comunica��o entre 
	// processos... At� mesmo para os casos mais simples onde o kernel envia mensagens 
	// para  ajanela de um processo, poder�amos abrir um canal de comunica��o, 
	// onde o processo kernel seria o emissor da mensagem.
	//
	//
	
	//
    // Obs: N�O � ATRIBUI��O DO DRIVER DE TECLADO SABER INFORMA��ES
    // SOBRE AS JANELAS, ENT�O UM DRIVER DE TECLADO DEVE ENVIAR A
    // MENSAGEM PARA UM GERENCIADOR DE MENSAGENS QUE COLOCAR� A MENSAGEM
    // EM SEU DEVIDO LUGAR. (Fila de mensagens da janela com o foco de entrada.)
    // o GERENTE DE JANELAS (window.c) ENCONTRA A JANELA COM O FOCO DE ENTRADA.
	// >>>> Um driver de teclado deve enviar a mensagem para um servi�o do sistema
	// que decidir� o que fazer com ela. N�o � responsabilidade do driver de teclado
	// encontrar o destino certo da mensagem, ele s� precisa entregar ela pro
	//servi�o de sistema respons�vel.
	//

	// 
    //>>>>isso � o que um driver deve fazer.. apenas solicitar o servi�o.
	//
	
	//driver de teclado >> servi�o do kernel >> classe do servi�o.
	
	//Obs: um teclado virtual pode simular essas mensagens
	//apenas chamando essa rotina de servi�os.
	
	
	//test. TEM QUE MOSTRAR PARA TERMINAR O DRIVER DE TECLADO.
	//printf("%c", ch);
	
	//Escrevendo na tela do desenvolvedor.
	//Isso ser� portado para outro m�dulo. est� aqui para teste. 
	
	/*
	if( (void*) gui->DEVELOPERSCREEN == NULL)
	{
        printf("abnt2_keyboard_handler: gui->DEVELOPERSCREEN");	
        refresh_screen();
        while(1){}		
	}else{
		
		//Cursor.
		//g_cursor_x = gui->DEVELOPERSCREEN->cursor_x;
		//g_cursor_y = gui->DEVELOPERSCREEN->cursor_y;
		//printf("%c", ch);
		//@todo: N�o est� incrementando a escrita ...
	};
	*/
	
	//69 ??
	//Enviando mensagem pra janela com o foco de entrada.
	//#bugbug Error: O driver de teclado n�o deve saber qual � a janela com foco de 
	//entrada.. ele deve enviar essa mensagem para o kernel e o kernel 
	//decide quem � a janela com o foco de entrada e encameinha a mensagem para 
	//a fila da janela com o o foco de entrada.
	
	//services( 69, (unsigned long) mensagem, (unsigned long) ch, (unsigned long) ch);

	//
	// Obs: estamos enviando a messagem para 2 procedimentos,
	// mas o procedimento do sistema n�o trada digita��es de teclas comuns. 
	// Obs: O que estamos fazendo aqui � chamarmos o procedimento de janela do sistema
	//e em seguida colocarmos a mensagem na fila da janela com o foco de entrada.
	// Obs: Uma op��o seria n�o chamarmos o procedimento de janela do sistema agora e
	// e sim apenas colocarmos a mensagem na fila da janela com o foco de entrada.
	// Da� ent�o, quando o aplicativo em user mode receber a mensagem, ele trata a 
	// a mensagem ou reencaminha para o procedimento de janela do sistema atrav�s de uma 
	// system call.
	// *Importante; Uma chamada ao procedimento de janela do sistema nesse momento,
	// so atrasa o recebimento da mensagem por parte do aplicativo em user mode.
    // bom seria que o aplicativo rebece a mensagem antes de chamar o procedimento de 
    // de janela para tratar as mensagens de sistema. Isso privilegia as teclas de digita��o
	// em detrimento das teclas de acionamento de controles.
	//
	//
	
	//
	// ?? PORQUE O HANDLE DE JANELA � NULO AQUI ??
	//
	// DEVEMOS CHAMAR O PROCEDIMENTO DO SISTEMA PARA ATUAR EM ALGUMA JANELA.
	//
	
	
	//teste:
	// o procedimento de janela do sistema afetara a janela com o foco de entrada.
	
    //#bugbug : ?? o ldisc deveria ter essa informa��o toda sobre janelas ??
	struct window_d *wwf;
	wwf = (void *) windowList[window_with_focus];
	
	if( (void*) wwf != NULL ){
	    system_procedure( wwf, (int) mensagem, (unsigned long) ch, (unsigned long) ch );
    }					 
	
	
	//test
	//@todo: #bugbug Lembrando que um driver ainda n�o tem acesso abertos
	//a essa fun��o... uma chada � essa fun��o deve ser criada na API usada pelo driver.
	//windowSendMessage( (unsigned long) 0,
	//		           (unsigned long) mensagem, //msg
	//				   (unsigned long) ch,
	//				   (unsigned long) ch );
	

	//*Importante: se nenhuma janela tem o foco de entrada, n�o h� porque 
	//enviar uma mensagem para a janela com o foco de entrada.
	
	
	/*
	 * Importante: Nesse caso podemos abrir um canal de comunica��o,
	 * onde o kernel � o emissor da mensagem.
	 */
	
	struct window_d *w;
	
	w = (void*) windowList[window_with_focus];
	
	//Somente destituimos o foco se a janela for v�lida.
    //Isso envia uma mensagem para a janela com o foco de entrada.
	//se nenhuma janela tem o foco de entrada ent�o apenas retorna.			
	if( (void*) w != NULL )
	{	    
		if( w->used == 1 && w->magic == 1234 ){
	        windowSendMessage( 0, mensagem, ch, ch);
		};			    	
	};		
	
    //
    // @todo: ENVIANDO A MENSAGEM PARA O GERENCIADOR DE MENSAGENS.
    //	

	/*
	 * Quem � o cliente?
	 * ================
	 *     O cliente � o processo que � o dono da janela com o
	 * foco de entrada.
	 *     Identificando a janela com o foco de entrada, sabemos
	 * quem � o processo cliente.
	 *  Obs: A janela com o foco de entrada nem sempre � a janela ativa,
	 * pode ser tamb�m uma janela filha da janela ativa.
	 * Mas at� mesmo a janela filha de uma janela ativa est� vinculada
	 * ao processo cliente.
	 */

    /*
	 * Sobre a fila de mensagem da janela com o foco de entrada:
     * ========================================================
     *  *IMPORTANTE: 
	 *      O que importa � qual janela est� com o foco de entrada.
	 *      Quando sabemos qual janela est� com o foco de entrada, ent�o
	 * sabemos qual � a janela que � a �rea de cliente, sabemos e provavelmente
	 * a sua janela m�e � a janela ativa, se ela mesma n�o for a janela ativa.
	 *      Uma mensagem pode ser enviada para a janela com o foco de entrada,
	 * e a mensagem afetar� apenas a janela com o foco de entrada.
	 *      Se a janela que receber a mensagem for a janela ativa, ela n�o
	 * ter� janela m�e, nessa caso uma mensagem para fechar uma janela ativa
	 * implica em fechar o processo ao qual ela est� vinculada.
	 *      N�o importa qual processo foi interrompida pela interrup��o de 
	 * teclado. A mensagem deve ir para a fila de mensagens da janela com o foco
	 * de entrada.
	 */


	/*
	 * Sobre a escolha do procedimento de janela:
	 * =========================================
     * Procedimento de janela.
	 *     Se o pr�ximo procedimento for o procedimento do sistema
	 *     dispacha para ele.
	 *     Chama o pr�ximo procedimento, passando os argumentos pra ele.
	 *     *IMPORTANTE: � importante saber qual � a janela ativa
	 *     pra enviar a mensagem para o procedimento de janela certo
	 *     e enviar todos os 4 parametros.
     */




    //
	// @todo:
	// Obs: Aqui estamos enviando diretamente para um procedimento de janela,
 	//     mas o certo � colocar na fila da janela com o foco de entrada.    
	//

	//Se for o procedimento padr�o. 
	//if( g_next_proc == (unsigned long) &system_procedure )
	//{
	//	//@todo: Aqui janela tem que ser NULL.
	//	system_procedure( NULL, (int) mensagem, (unsigned long) ch, 0);
	//    //goto eoi;
	//};

	//Se N�O for o procedimento padr�o. 
	//if( g_next_proc != (unsigned long) &system_procedure )
	//{

		//
		// @todo: 
		// Na verdade, aqui tem que mandar a mensage para a janela com o foco 
		// de entrada, mesmo que seja uma janela filha.
		// 

	//	wFocus = (void *) WindowWithFocus;
	//	CurrentWindow = (void *) WindowWithFocus;

		//
		// A janela atual, seria a janela ativa ?
		//

		//
		// Estamos apenas enviando para a janela com foco de entrada.
		//

    //    if( (void*) wFocus == NULL ){			
    //        system_dispatch_to_procedure( NULL, (int) mensagem, (unsigned long) ch, 0);
	//	}else{

			//Valida a estrutura da janela com o foco de entrada.
	//		if( wFocus->used == 1 && wFocus->magic == 1234 )
	//		{		
				//Estamos enviando a mensagem para a estrutura da janela 
				//com o foco de entrada.
	//			wFocus->msg_window = (void*) wFocus;
	//			wFocus->msg = (int)	mensagem;
	//			wFocus->long1 = (unsigned long) ch;
	//			wFocus->long2 = (unsigned long)0;  //Ainda n�o implementada.

				//...

				//@todo: Precisamos identificar que � a janela m�e
				//e qual � processo ao qual essa janela est� vinvulada.

	//		} 	

	//		system_dispatch_to_procedure( wFocus, (int) mensagem, (unsigned long) ch, 0);
	//	}

		//Nothing.

		//goto eoi;
	//};


	/*
	 * IPC stuffs
	 * enviando para uma area de memoria compartilhada.
	 * ipc_dispatch_message( WindowProcedure->window,
			                 WindowProcedure->msg,
							 WindowProcedure->long1,
							 WindowProcedure->long2 );
	 */

eoi:
    outportb(0x20, 0x20);    //EOI.
    return;
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
 *     Inicializa o driver de teclado.
 *
 *  @todo: enviar para o driver de teclado o que for de l�.
 *         criar a vari�vel keyboard_type ;;; ABNT2 
 */
 
// void keyboardInit()
 
void init_keyboard()
{
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

//?? estranho
//?? usado pelo mouse
#define outanyb(p) __asm__ __volatile__( "outb %%al,%0" : : "dN"((p)) : "eax" )

/*
 * init_mouse:
 *     Inicializando o mouse no controlador 8042.
 */		
int init_mouse()
{
    unsigned char response = 0;
    unsigned char deviceId = 0;
    int i; 
	int bruto = 1;  //M�todo.
	
	//
	// Estamos espa�o para o buffer de mensagens de mouse.
	mousemsg = ( unsigned char *) malloc(32);
	//@todo:
	// Checar se n�o � NULL.
	
	//Inicializando ...
    mouse_status = 0;
    delta_x = 0;
    delta_y = 0;
    count_mouse = 0;
    //...
	
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
	if(bruto == 1){
	    mouse_write(0xFF);
	    mouse_write(0xF6); 
	    mouse_write(0xF4); 
		while(!0xFA)mouse_read();
	};
	
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
 * **************************************************
 * mouseHandler:
 *     Handler de mouse. 
 *
 * *Importante: 
 *     Se estamos aqui � porque os dados dispon�veis no controlador 8042 
 * pertencem ao mouse.
 * @todo: Essa rotina n�o pertence ao line discipline.
 * 
 */
void mouseHandler()
{	
	buffer_mouse[count_mouse++] = mouse_read();
	
	//
	// Contamos o n�mero de interrup��es. Quando chega a 3, ent�o j� temos 
	// a quantidade de informa��es necess�ria.
	//
	
	if(count_mouse >= 3)
	{
        mouse_status = buffer_mouse[0];
        
		delta_x = buffer_mouse[1];
	    delta_y = buffer_mouse[2];
		
        //printf("ocorreram 3 irq12 !\n");
        //kernelPS2MouseDriverReadData(); //bugbug
		printf("mouseStatus={%d} deltaX={%d} deltaY={%d} \n",mouse_status,delta_x,delta_y);
		refresh_screen();
		
	    //printf("#"); //teste
	    //MessageBox(gui->screen, 1, "mouseHandler:","Testing Mouse interrupt");
	    //kernelPS2MouseDriverReadData();
		
		//Zerando a contagem de interrup��es de mouse.
		count_mouse=0;
    };
	//return;
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
	
done:							
    return;
};


/* 
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
	
	// Activar a primeira porta PS/2.
	kbdc_wait(1);
	outportb(0x64,0xAE);   

	// Activar a segunda porta PS/2.
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
 * ps2:
 *     Inicializa o controlador ps2.
 *     Inicializa a porta do teclado no controlador.
 *     Inicializa a porta do mouse no controlador.
 *     Obs: *importante: A ordem precisa ser respeitada.
 *     As vezes os dois n�o funcionam ao mesmo tempo se a 
 *     inicializa��o n�o for feita desse jeito. 
 */
void ps2()
{
    P8042_install();
    init_keyboard();
	init_mouse();	
};

//
// End.
//

