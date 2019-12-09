/*
 * File: i8042/ps2mouse.c
 * 
 *      + ps2 mouse controler support.
 *      + mouse event support.
 * 
 * Environment:
 *     Ring 0. Kernel base persistent code.
 *
 * History:
 *     2018 - Created by Fred Nora.
 */

// #todo
// talvez fazer estruturas para controlar as configura��es de mouse.
 
#include <kernel.h>


// Screen width and height

extern unsigned long SavedX; 
extern unsigned long SavedY; 


//=============================
//mouse control

// habilitar e desabilitar o mouse.
// usada pelos aplicativos.
int ps2_mouse_status;


//salvaremos aqui o �ltimo total ticks pra
//pegarmos um delta, se o delta for menor que o limite
//ent�o temos um duploclick.
unsigned long ps2mouse_current_totalticks;
unsigned long ps2mouse_last_totalticks;
unsigned long ps2mouse_delta_totalticks;


int ps2_button_pressed;
int ps2_mouse_moving;
int ps2_mouse_drag_status;
//int ps2_mouse_drop_status;

//====================================================================
// update_mouse support

#define MOUSE_LEFT_BTN    0x01
#define MOUSE_RIGHT_BTN   0x02
#define MOUSE_MIDDLE_BTN  0x04
#define MOUSE_X_SIGN      0x10
#define MOUSE_Y_SIGN      0x20
#define MOUSE_X_OVERFLOW  0x40
#define MOUSE_Y_OVERFLOW  0x80

long mouse_x = 0;
long mouse_y = 0;

char mouse_packet_data = 0;
char mouse_packet_x = 0;
char mouse_packet_y = 0;
char mouse_packet_scroll = 0;
//=====================================================================




//====================================================================
//  mouseHandler support
#define MOUSE_DATA_BIT 1
#define MOUSE_SIG_BIT  2
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08 

// Contador
static int count_mouse = 0;

// Buffer
static char buffer_mouse[3];

int flagRefreshMouseOver;
//=====================================================================




// Prototype

int MOUSE_BAT_TEST (void);



/*
 ******************************************
 * MOUSE_SEND_MESSAGE:
 *     Send a mouse message. 
 */

// Pegamos os tres char do inout de mouse e transformamos em uma mensagem 
// que ser� enviada para uma thread.

int MOUSE_SEND_MESSAGE (void *buffer) {

    if ( (void *) buffer == NULL )
        return (int) -1;


    unsigned char *chars = (unsigned char *) buffer;

    //char char0 = (char) &chars[0];
    //char char1 = (char) &chars[1];
    //char char2 = (char) &chars[2];
    //char char3 = (char) &chars[3];
    //...

	//
	// #todo
	// Colocar esses dados em uma mesnagem e enviar para thread.
	//


    return (int) -1;
}



/*
 *********************************************
 * mouse_write:
 *     Envia um byte para a porta 0x60.
 */

void mouse_write (unsigned char write){

    kbdc_wait (1);
    outportb (0x64,0xD4);

    kbdc_wait (1);
    outportb (0x60,write);
}


/*
 **************************************
 * mouse_read:
 *     Pega um byte na porta 0x60.
 */

unsigned char mouse_read (void){

    kbdc_wait (0);

    return (unsigned char) inportb (0x60);
}


/*
 *********************************** 
 * MOUSE_BAT_TEST:
 * 
 */

// Esta rotina faz o Auto-teste. 
// 0xaa �xito, 0xfc erro.
// Created by Fed Nora.

int MOUSE_BAT_TEST (void){

    int val = -1;
    int i;

	// #todo:
	// Cuidado.
	// Diminuir isso se for possivel.
	// Nao funciona na maquina reala sem esse delay.

    for (i=0; i<99000; i++)
    {
        wait_ns (200);
    };


	//while (1)

    for ( i=0; i<999; i++ )
    {
        val = mouse_read ();

        if (val == 0xAA)
        {
            printf ("MOUSE_BAT_TEST OK\n");
            return 0;

        }else if (val == 0xFC){

            printf ("MOUSE_BAT_TEST fail\n");
            return (int) -1; 
        };
    
        	// Reenviar o comando. 
        	// OBS: este comando n�o � colocado em buffer

            mouse_write (0xFE);       
    };


    printf ("MOUSE_BAT_TEST %d times\n", i );
}


/*
 ******************************************************
 * mouse_install:
 * ...
 */

void mouse_install (void){

	// 0xFF
	// Espera o dados descer (ACK)
	// Basic Assurance Test (BAT)


    mouse_write (0xFF);
    while ( mouse_read() != 0xFA);


	//printf ("mouse_install: 2\n");
	//refresh_screen();


    if ( MOUSE_BAT_TEST() != 0) 
    {
        //printf("\n Mouse error!");
        printf ("mouse_install: [WARMING] MOUSE_BAT_TEST ignored\n");
    }


	// Use mouse default
	//Espera o dados descer (ACK)

    mouse_write (0xF6);
    while( mouse_read() != 0xFA);


	//printf ("mouse_install: 3\n");
	//refresh_screen();


	// habilita o mouse. (streaming)
	//Espera o dados descer (ACK)


    mouse_write (0xF4);
    while( mouse_read() != 0xFA);

	// Espera nossa controladora terminar.
	//printf ("mouse_install: 4\n");
	//refresh_screen();

    kbdc_wait (1);
}



/*
 ***************************************************
 * getMouseData:
 *     Essa fun��o � usada pela rotina kernelPS2MouseDriverReadData.
 * Input a value from the keyboard controller's data port, after checking
 * to make sure that there's some mouse data there for us.
 */

static unsigned char getMouseData (void){

    unsigned char data = 0;

    while ((data & 0x21) != 0x21)
        kernelProcessorInPort8 (0x64, data);


    kernelProcessorInPort8 (0x60, data);

    return (data);
}


/*
 ******************************************************
 * kernelPS2MouseDriverReadData:  
 *     Pega os bytes e salva em um array.
 *     Exibe um caractere na tela, dado as cordenadas.
 *     This gets called whenever there is a mouse interrupt
 *     @todo: Rever as entradas no array.         
 */

void kernelPS2MouseDriverReadData (void)
{
    //#suspensa !
}


/*
 *******************************************************
 * load_mouse_bmp:  
 * 
 *     ## teste ##
 * 
 *     Carregando o arquivo MOUSE.BMP, que � o ponteiro de mouse.
 *     Usar isso na inicializa��o do mouse.
 * 
 *     #bugbug isso pode ir para windowLoadGramadoIcons
 * 
 *     2018 - Created by Fred Nora.
 */

int load_mouse_bmp (void){

    int Status = 1;
    int Index; 
    unsigned long fileret;


#ifdef KERNEL_VERBOSE
    //printf ("load_mouse_bmp:\n");
#endif


    // Alocando duas p�ginas para um BMP pequeno. 8KB.

	mouseBMPBuffer = (void *) allocPages (2);
	//mouseBMPBuffer = (void *) allocPages (10);


    if ( (void *) mouseBMPBuffer == NULL )
    {
        printf ("load_mouse_bmp: mouseBMPBuffer\n");
        goto fail;
    }


	//
	// ## Loading ...  ##
	//

	//read_fntos( (char *) arg1);


	//===================================
	// MOUSE

    fileret = (unsigned long) fsLoadFile ( VOLUME1_FAT_ADDRESS,
                                  VOLUME1_ROOTDIR_ADDRESS, 
                                  "MOUSE   BMP", 
                                  (unsigned long) mouseBMPBuffer );

    if ( fileret != 0 )
    {
        printf ("MOUSE.BMP FAIL\n");
        
		// Escrevendo string na janela.
		//draw_text( gui->main, 10, 500, COLOR_WINDOWTEXT, "MOUSE.BMP FAIL");

        goto fail;
    }


	// Render BMP file on backbuffer.
	//bmpDisplayBMP( mouseBMPBuffer, 20, 20 );
	//refresh_rectangle( 20, 20, 16, 16 );
	//===================================


//refresh_rectangle:

	//Isso funcionou ...
	//refresh_rectangle( 20, 20, 16, 16 );

    Status = (int) 0;
    goto done;


fail:
    printf ("fail\n");
    Status = (int) 1;

done:

#ifdef KERNEL_VERBOSE
    printf ("done\n");
#endif

	//refresh_screen();
    return (int) Status;
}


// events.h
void set_current_mouse_responder (int i)
{
    current_mouse_responder = i;
}


// events.h
int get_current_mouse_responder (void)
{
    return (int) current_mouse_responder;
}






/*
 * =====================================================
 * update_mouse:
 *     Updates the mouse position.
 */

void update_mouse (void){

//======== X ==========
// Testando o sinal de x.
// Do the x pos first.

//pega o delta x
//testa o sinal para x
do_x:

    if ( mouse_packet_data & MOUSE_X_SIGN ) 
    {
        goto x_neg;
    }


//Caso x seja positivo.
x_pos:

    mouse_x += mouse_packet_x;
    goto do_y;


//Caso x seja negativo.
x_neg:

    mouse_x -= ( ~mouse_packet_x + 1 );

    if (mouse_x > 0)
    {
        goto do_y;
    }
    mouse_x = 0;
 
 
//======== Y ==========
// Testando o sinal de x. 
// Do the same for y position.

//Pega o delta y.
//Testa o sinal para y.
do_y:

    if ( mouse_packet_data & MOUSE_Y_SIGN )
    {
        goto y_neg;
    }


//Caso y seja positivo.
y_pos:

    mouse_y -= mouse_packet_y;

    if ( mouse_y > 0 )
    {
        goto quit;
    }

    mouse_y = 0;
    goto quit;


//Caso y seja negativo. 
y_neg:

    mouse_y += ( ~mouse_packet_y + 1 );

// Quit
quit:
    return;
}





/*
 ********************************************************
 * mouseHandler:
 *     Handler de mouse. 
 *
 * *Importante: 
 *     Se estamos aqui � porque os dados dispon�veis no controlador 8042 
 * pertencem ao mouse.
 *
 * Obs: 
 * Temos externs no in�cio desse arquivo.
 */


void mouseHandler (void){

	// #importante:
	// Essa ser� a thread que receber� a mensagem.

	struct thread_d *t;


	// #importante:
	// Essa ser� a janela afetada por qualquer evento de mouse.
	// ID de janela.

    struct window_d *Window;
    int wID; 
    
    //#test
    int last_wID;



	// Coordenadas do mouse.
	// Obs: Isso pode ser global.
	// ?? O tratador em assembly tem as vari�veis globais do posicionamento.

    int posX = 0;
    int posY = 0;


	// Lendo um char no controlador.
    char *_byte;


    //#todo: Isso � um teste.
    // O mouse ps2 est� desabilitado por�m recebendo as interup��es
    // Os aplicativos podem reabilit�-lo a qualquer momento.
    // Atribuindo foco por exemplo.
    
    //#bugbug
    //Isso realmente paraliza o mouse. Mas reabilitar n�o est� funcionando.
    //if ( ps2_mouse_status == 0 )
    //{
    //      return;
    // }


	//Char para o cursor provis�rio.
	//#todo: Isso foi subtituido por uma bmp. Podemos deletar.
    //static char mouse_char[] = "T";


	//
	// Read !
	//

	// #obs:
	// Lendo um char no controlador.

    *_byte = (char) mouse_read ();


	// #importante:
	// Contagem de interru��es:
	// Obs: Precisamos esperar 3 interrup��es.


	//#obs: count_mouse � global. Provavelmente nesse arquivo mesmo.

    switch ( count_mouse )
    {

		// > Essa foi a primeira interrup��o.
        case 0:
            //Pegamos o primeiro char.
            buffer_mouse[0] = (char) *_byte;
            if (*_byte & MOUSE_V_BIT)
                count_mouse++;
            break;


		// >> Essa foi a segunda interrup��o.
        case 1:
            //Pegamos o segundo char.
            buffer_mouse[1] = (char) *_byte;
            count_mouse++;
            break;


		// >>> Essa foi a terceira interrup��o. � a �ltima.
        case 2:
            //Pegamos o terceiro char.
            buffer_mouse[2] = (char) *_byte;
            count_mouse = 0;

			// A partir de agora j� temos os tr�s chars.
			// Colocando os tr�s chars em vari�veis globais.
			// Isso ficar� assim caso n�o haja overflow.

            mouse_packet_data = buffer_mouse[0];    // Primeiro char
            mouse_packet_x    = buffer_mouse[1];    // Segundo char.
            mouse_packet_y    = buffer_mouse[2];    // Terceiro char.

    
			// Salvando o antigo antes de atualizar.
			// Para poder apagar daqui a pouco.
			// Atualizando.
            saved_mouse_x = mouse_x;
            saved_mouse_y = mouse_y;
            update_mouse (); 
 
            // Agora vamos manipular os valores obtidos atrav�s da 
            // fun��o de atualiza��o dos valores.
            // A fun��o de atualiza��o atualizou os valores de
            // mouse_x e mouse_y.
            mouse_x = (mouse_x & 0x000003FF );
            mouse_y = (mouse_y & 0x000003FF );

			// #importante:
			// Checando limites.
			// Isso � provis�rio.

            if ( mouse_x < 1 ){ mouse_x = 1; }
            if ( mouse_y < 1 ){ mouse_y = 1; }
            if ( mouse_x > (SavedX-16) ){ mouse_x = (SavedX-16); }
            if ( mouse_y > (SavedY-16) ){ mouse_y = (SavedY-16); }


            // Comparando o novo com o antigo, pra saber se o mouse se moveu.
            // #obs: Pra quem mandaremos a mensagem de moving ??
            if ( saved_mouse_x != mouse_x || saved_mouse_y != mouse_y )
            {
				// flag: o mouse est� se movendo.
				// usaremos isso no keydown.
				// >> na hora de enviarmos uma mensagem de mouse se movendo
				// se o bot�o estiver pressionado ent�o temos um drag (carregar.)
				// um release cancela o carregar.
				
				ps2_mouse_moving = 1;
				
				if ( ps2_button_pressed == 1 )
				{ 
				    ps2_mouse_drag_status = 1;
                }else{
				    ps2_mouse_drag_status = 0;
				}
                   
                //
                // draw
                //

                // Apaga o antigo.
                // + Copia no LFB um ret�ngulo do backbuffer 
                // para apagar o ponteiro antigo.
                refresh_rectangle ( saved_mouse_x, saved_mouse_y, 20, 20 );
                
                // Acende o novo.
                //+ Decodifica o mouse diretamente no LFB.
                // Copiar para o LFB o antigo ret�ngulo  
                // para apagar o ponteiro que est� no LFB.
                bmpDisplayMousePointerBMP ( mouseBMPBuffer, mouse_x, mouse_y );            
            }else{
				
				// N�o redesenhamos quando o evento for um click, sem movimento.
			    ps2_mouse_moving = 0;
			} 
            break;


        // Problemas na contagem de interrup��es.
        default:
            count_mouse = 0;
            break;
    };



	// #importante 
	// Por outro lado o mouse deve confrontar seu posicionamento com 
	// todas as janelas, para saber se as coordenadas atuais est�o passando 
	// por alguma das janelas. Se estiver, ent�o enviaremos mensagens para essa 
	// janela que o mouse passou por cima. Ela deve ser sinalizada como hover,
	// 
	// #importante:
	// Se houver um click, o elemento com mousehover ser� afetado, e 
	// enviaremos mesagens para ele, se apertarem enter ou application key, 
	// quem recebe a mensagem � o first responder, ou seja, a janela com o 
	// foco de entrada.
	// Se clicarmos com o bot�o da direita, quem recebe a mensagem � 
	// a janela que o mouse est� passando por cima.


	//
	//  ## Button ##
	//


	//Apenas obtendo o estado dos bot�es.
    mouse_buttom_1 = 0;
    mouse_buttom_2 = 0;
    mouse_buttom_3 = 0;



	// ## LEFT ##
    if ( ( mouse_packet_data & MOUSE_LEFT_BUTTON ) == 0 )
    {
		//liberada.
        mouse_buttom_1 = 0;

    }else if( ( mouse_packet_data & MOUSE_LEFT_BUTTON ) != 0 )
        {
		    //pressionada.
		    //N�o tem como pressionar mais de um bot�o por vez.

            mouse_buttom_1 = 1;
            mouse_buttom_2 = 0;
            mouse_buttom_3 = 0;
        };



	// ## RIGHT ##
    if ( ( mouse_packet_data & MOUSE_RIGHT_BUTTON ) == 0 )
    {
	    //liberada.
        mouse_buttom_2 = 0;

    }else if( ( mouse_packet_data & MOUSE_RIGHT_BUTTON ) != 0 )
        {
		    //pressionada.
		    //N�o tem como pressionar mais de um bot�o por vez.

            mouse_buttom_1 = 0;
            mouse_buttom_2 = 1;
            mouse_buttom_3 = 0;

        };



	// ## MIDDLE ##
    if ( ( mouse_packet_data & MOUSE_MIDDLE_BUTTON ) == 0 )
    {
	    //liberada.
        mouse_buttom_3 = 0;

    }else if( ( mouse_packet_data & MOUSE_MIDDLE_BUTTON ) != 0 )
        {
	        //pressionada.
	        //N�o tem como pressionar mais de um bot�o por vez.

	        mouse_buttom_1 = 0;
	        mouse_buttom_2 = 0;
	        mouse_buttom_3 = 1;

        };


	// ===
	// Confrontando o estado atual com o estado anterior para saber se ouve 
	// alguma altera��o ou n�o.
	// 1 = ouve altera��o.

    if ( mouse_buttom_1 != old_mouse_buttom_1 ||
         mouse_buttom_2 != old_mouse_buttom_2 ||
         mouse_buttom_3 != old_mouse_buttom_3 )
    {
        mouse_button_action = 1;
    }else{
        mouse_button_action = 0;
    };



	// #refletindo: 
	// ?? E no caso de apenas considerarmos que o mouse est� se movendo, 
	// mandaremos para janela over. ???
	
	// #refletindo:
	// Obs: A mensagem over pode ser enviada apenas uma vez. 
	// ser� usada para 'capturar' o mouse ... 
	// e depois tem a mensagem para 'descapturar'.
	
	
    // ## Sem escaneamento de janelas por enquanto, apenas mostre e mova o ponteiro ##
	
	//## ent�o n�o enviaremos mensagens para a thread ###
	
	//
	//  ## Scan ##
	//
	
	//===========
	// (capture) - On mouse over. 
	//

	// wID = ID da janela.
	// Escaneamos para achar qual janela bate com os valores indicados.
	// Ou seja. Sobre qual janela o mouse est� passando.
	
	// #BUGBUG
	// O problema nessa t�cnica s�o as sobreposi��o de janelas.
	// Quando uma janela est� dentro da outr, ent�o duas janelas
	// est�o �ptas a serem selecionadas.
	// Talvez devamos filtrar e s� aceitarmos sondar procurando 
	// por controles.

	// #IMPORTANTE
	// Se for v�lido e diferente da atual, significa que 
	// estamos dentro de uma janela.
	// -1 significa que ouve algum problema no escaneamento.
	
	//wID = (int) windowScan ( mouse_x, mouse_y );

    
	//#importante:
	//Ja que n�o passamos em cima de um bot�o ou editbox,
	//ent�o vamos ver se estamos em cima de uma janela overlapped.
    //#obs: isso ficou bom ... estamos testando
    
    // #bugbug
    // Com isso o sistema trava quando tentamos mover o mouse
    // no programa gdeshell, que � fullscreen.
    //Podemos tentar desabilitar o mouse quando entrarmos em fullscreen
    // no gdeshell(teste)?
    
    //#todo: 
    //N�o h� porque sondar janelas se tivermos em full screen.
    //pois somente teremos a �rea de cliente de uma das janelas.

    //============
    //if ( wID == -1 )
    //{
         //wID = (int) windowOverLappedScan ( mouse_x, mouse_y );
   // }


    //wID = (int) windowOverLappedScan ( mouse_x, mouse_y );
    
    //se pegamos uma overlapped.
    //vamos testar os botoes
    //if ( wID != -1 )
    //{
        //printf ("o ");
		//wID = (int) windowScan ( mouse_x, mouse_y );
    //}
    
    wID = (int) windowScan ( mouse_x, mouse_y );

    //================
	//Se houve problema no escaneamento de janela do tipo bot�o ou editbox.
    if ( wID == -1 )
    { 

		// Essa flag indica que podemos fazer o refresh da mouse ouver,
		// mas somente uma vez.
        if ( flagRefreshMouseOver == 1 )
        {
            Window = (struct window_d *) windowList[mouseover_window];

			//#bugbug:
			//precisamos checar a validade da estrutura antes de usa-la.

			//#importante:
			//isso apaga o que pintamos na janela, o valor foi salvo logo abaixo.

            if ( (void *) Window != NULL )
            {
                //refresh_rectangle ( Window->left, Window->top, 20, 20 );
                //refresh_rectangle ( savedmouseoverwindowX, savedmouseoverwindowY, 20, 20 );

            }

			// N�o podemos mais fazer refresh.
            flagRefreshMouseOver = 0;

			//#importante inicializa.
            mouseover_window = 0;
        }
     }


    if ( wID == -1 )
    {
		return;
    }

	// Se n�o houve problema no escaneamento de janela ou seja, se encontramos 
	// uma janela. Ent�o essa janela deve estar associada � uma thread para qual 
	// mandaremos a mensagem. Caso a thread for null ... apenas n�o enviamos.
	// A janela tem uma thread de controle, igual ao processo.

     //============================ 
     //Se estamos sobre uma janela v�lida.
     if ( wID > -1 )
     {

        Window = (struct window_d *) windowList[wID];

        if ( (void *) Window == NULL ) { return; }
        if ( Window->used != 1 || Window->magic != 1234){ return; }


		//#importante:
		//Nesse momento temos uma janela v�lida, ent�o devemos 
		//pegar a thread associada � essa janela, dessa forma 
		//enviaremos a mensagem para a thread do aplicativo ao qual 
		//a janela pertence.

        t = (void *) Window->control;

        if ( (void *) t == NULL ){ return; }
        if ( t->used != 1 || t->magic != 1234 ){ return; }

		//#bugbug 
		//#todo:
		// Deveriamos aqui checarmos a validade da estrutura ??

		//redraw_window(wScan);


        //
        // ==== Button events ====
        //
 
		// #importante
		// Se um bot�o foi pressionado ou liberado, ent�o enviaremos uma 
		// mensagem relativa ao estado do bot�o, caso contr�rio, enviaremos 
		// uma mensagem sobre a movimenta��o do mouse.

		//Qual bot�o mudou seu estado??
		//Checaremos um por um.


		//===============================================
		// ***Se houve mudan�a em rela��o ao estado anterior.
		// Nesse momento um drag pode terminar
        if ( mouse_button_action == 1 )
        {

			// >> BOT�O 1 ==================
			//Igual ao estado anterior
            if ( mouse_buttom_1 == old_mouse_buttom_1 )
            {
				//...
			
			//Diferente do estado anterior.
            }else{
				
				// down - O bot�o 1 foi pressionado.
                if ( mouse_buttom_1 == 1 )
                { 
					//clicou
                    if ( old_mouse_buttom_1 == 0 )
                    {
						// flag: um bot�o foi pressionado.
						ps2_button_pressed = 1;
						
						// Enviaremos a mensagem para a thread atual.
						// houve altera��o no estado do bot�o 1 e estamos em cima de uma janela.
                        if ( (void *) Window != NULL )
                        {
							//pegamos o total tick
							ps2mouse_current_totalticks = (unsigned long) get_systime_totalticks();
                            ps2mouse_delta_totalticks = (ps2mouse_current_totalticks - ps2mouse_last_totalticks); 
                            //printf ( "x=%d l=%d d=%d \n",
                              // ps2mouse_current_totalticks,ps2mouse_last_totalticks,ps2mouse_delta_totalticks ); 
                            //refresh_screen();
                            ps2mouse_last_totalticks = ps2mouse_current_totalticks;
                            t->window = Window;
                            t->msg = MSG_MOUSEKEYDOWN;
                            if (ps2mouse_delta_totalticks < 1000) //2000
                            {
								t->msg = MSG_MOUSE_DOUBLECLICKED; 
								ps2mouse_delta_totalticks=8000; // delta inv�lido.
							}
                            t->long1 = 1;
                            t->long2 = 0;
                            t->newmessageFlag = 1;
                        }
                        //else: // houve altera��o no estado do bot�o 1 mas n�o estamos em cima de uma janela.
                        
                        
						//Atualiza o estado anterior.
                        old_mouse_buttom_1 = 1;
                    }

				// up - O bot�o 1 foi liberado.
                }else{

				    // flag: um bot�o foi liberado.
				    ps2_button_pressed = 0;
						
					// #importante 
					// Enviaremos a mensagem para a thread atual.
                    if ( (void *) Window != NULL )
                    {
						// N�o estamos mais carregando um objeto.
						ps2_mouse_drag_status = 0;
                        
                        t->window = Window;
                        t->msg = MSG_MOUSEKEYUP;
                        t->long1 = 1;
                        t->long2 = 0;
                        t->newmessageFlag = 1;
                    }

                    old_mouse_buttom_1 = 0;
                }
            }; 


			// >> BOT�O 2 ==================
			// Igual ao estado anterior
            if ( mouse_buttom_2 == old_mouse_buttom_2 )
            {
				//...

			// Diferente do estado anterior.
            }else{

				// down - O bot�o 2 foi pressionado.
                if ( mouse_buttom_2 == 1 )
                {

					//clicou
                    if ( old_mouse_buttom_2 == 0 )
                    {
						// flag: um bot�o foi pressionado.
						ps2_button_pressed = 1;
						
						// houve altera��o no estado do bot�o 2 e estamos em cima de uma janela.
                        if ( (void *) Window != NULL )
                        {
                            t->window = Window;
                            t->msg = MSG_MOUSEKEYDOWN;
                            t->long1 = 2;
                            t->long2 = 0;
                            t->newmessageFlag = 1;
                        }
                        //else: // houve altera��o no estado do bot�o 2 mas n�o estamos em cima de uma janela.

						// atualiza o estado anterior.
                        old_mouse_buttom_2 = 1;
                    }

				// up - O bot�o 2 foi liberado.
                }else{

				    // flag: um bot�o foi liberado.
				    ps2_button_pressed = 0;
				    
                   if ( (void *) Window != NULL )
                   {
						// N�o estamos mais carregando um objeto.
						ps2_mouse_drag_status = 0;

                        t->window = Window;
                        t->msg = MSG_MOUSEKEYUP;
                        t->long1 = 2;
                        t->long2 = 0;
                        t->newmessageFlag = 1;
                    }

                    old_mouse_buttom_2 = 0;
                }
            }; 


			// >> BOT�O 3 ==================
			// Igual ao estado anterior
            if ( mouse_buttom_3 == old_mouse_buttom_3 )
            {
				//...

			// Diferente do estado anterior.
            }else{

				// down - O bot�o 3 foi pressionado.
                if ( mouse_buttom_3 == 1 )
                {
					//clicou
                    if ( old_mouse_buttom_3 == 0 )
                    {
						// flag: um bot�o foi pressionado.
						ps2_button_pressed = 1;
						
                        // houve altera��o no estado do bot�o 3 e estamos em cima de uma janela.
                        if ( (void *) Window != NULL )
                        {
                            t->window = Window;
                            t->msg = MSG_MOUSEKEYDOWN;
                            t->long1 = 3;
                            t->long2 = 0;
                            t->newmessageFlag = 1;
                        }
                        //else: // houve altera��o no estado do bot�o 1 mas n�o estamos em cima de uma janela.

						// Atualiza o estado anterior.
                        old_mouse_buttom_3 = 1;
                    }

				// up - O bot�o 3 foi liberado.
                }else{

				    // flag: um bot�o foi liberado.
				    ps2_button_pressed = 0;
				    
                    if ( (void *) Window != NULL )
                    {
						// N�o estamos mais carregando um objeto.
						ps2_mouse_drag_status = 0;

                        t->window = Window;
                        t->msg = MSG_MOUSEKEYUP;
                        t->long1 = 3;
                        t->long2 = 0;
                        t->newmessageFlag = 1;
                    }

                    old_mouse_buttom_3 = 0;
                }
            }; 

			// A��o conclu�da.
            mouse_button_action = 0;
        };


        
        //===============================================
        // *** Se N�O ouve altera��o no estado dos bot�es, ent�o apenas 
        // enviaremos a mensagem de movimento do mouse e sinalizamos 
        // qual � a janela que o mouse est� em cima.
        // N�o houve altera��o no estado dos bot�es, mas o mouse
        // pode estar se movendo com o bot�o pressionado.
        //a n�o ser que quando pressionamos o bot�o ele envie v�rias
        //interrup��es, igual no teclado.

        if ( mouse_button_action == 0 )
        {

			// #importante
			// Lembrando que estamos dentro de uma janela ...
			// por isso a mensagem � over e n�o move.

			//Obs: Se a janela for a mesma que capturou o mouse,
			//ent�o n�o precisamos reenviar a mensagem.
			//Mas se o mouse estiver em cima de uma janela diferente da 
			//que ele estava anteriormente, ent�o precisamos enviar uma 
			//mensagem pra essa nova janela.

			//#bugbug:
			//estamos acessando a estrutura, mas precisamos antes saber se ela � v�lida.

            //estamos em cima de uma janela e n�o houve altera��o no estado dos bot�es
            if ( (void *) Window != NULL )
            {
                // diferente de mouseover window.
                if ( Window->id != mouseover_window )
                {

                    if ( mouseover_window != 0 )
                    {

                       //if ( (void *) Window != NULL ){
                        t->window = (struct window_d *) windowList[mouseover_window];
                        t->msg = MSG_MOUSEEXITED;
                        t->long1 = 0;
                        t->long2 = 0;
                        t->newmessageFlag = 1;
                        //}
                    };

					// Agora enviamos uma mensagem pra a nova janela que 
					// o mouse est� passando por cima.
                    //#todo: reagir a isso l� nos apps.
                    
                    t->window = Window;
                    t->msg = MSG_MOUSEOVER;
                    t->long1 = 0;
                    t->long2 = 0;
                    t->newmessageFlag = 1;
                    
                    //estamos carregando a janela
                    if ( ps2_mouse_drag_status == 1 )
                    {
                        t->window = Window;
                        t->msg = MSG_MOUSE_DRAG;
                        t->long1 = 0;
                        t->long2 = 0;
                        t->newmessageFlag = 1;
                    }

					// J� que entramos em uma nova janela, vamos mostra isso.

					//bot�o.
					//#provis�rio ...
					//Isso � um sinalizador quando mouse passa por cima.
					//#test: Vamos tentar modificar as caracter�sticas do bot�o.

                    //#ok isso funciona, vamos deixar oapp fazer isso.
                    //faremos isso somente para os bot�es do sistema.  
                    //if ( Window->isButton == 1 )
                    //{
						//isso funciona.
                        //bmpDisplayCursorBMP ( fileIconBuffer, Window->left, Window->top );
						//#test
                        //update_button ( (struct button_d *) Window->button,
                            //(unsigned char *) Window->button->string,
                            //(int) Window->button->style,
                            //(int) BS_HOVER,
                            //(int) Window->button->type,
                            //(unsigned long) Window->button->x, 
                            //(unsigned long) Window->button->y, 
                            //(unsigned long) Window->button->width, 
                            //(unsigned long) Window->button->height, 
                            //(unsigned long) Window->button->color );
						//redraw_button ( (struct button_d *) Window->button );
						//show_window_rect (Window);
					//};
					

				    //n�o bot�o.
				    //if ( Window->isButton == 0 )
				    //{
				    //    bmpDisplayCursorBMP ( folderIconBuffer, Window->left, Window->top );
				    //};


				    //nova mouse over
                    mouseover_window = Window->id;

					//#importante:
					//flag que ativa o refresh do mouseover somente uma vez.
                    flagRefreshMouseOver = 1;

                //� mouse over window.
                // n�o estamos em cima de uma janela e n�o houve altera��o no estado dos bot�es
                }else{ 

				    //nothing ...
                };
            };

			// A��o conclu�da.
			// Para o caso de um valor incostante na flag.
            mouse_button_action = 0;
        };


    };
}




/* 
 * *******************************************************************
 * ps2_mouse_initialize :
 *     Configurando o controlador PS/2, 
 *     e activar a segunda porta PS/2 (mouse).
 *     � uma rotina de inicializa��o do controlador i8042 para 
 *     habilitar o segundo dispositivo, o mouse.
 *     Essa rotina deve rodar somente uma vez, durante inicializa��o.
 *
 * Credits:
 *     2018 - Fred Nora.
 *     2018 - Nelson Cole.
 */
 
void ps2_mouse_initialize (void){

    int error_code = 0;
    unsigned char status;


	// Flush the output buffer
	//while ((inportb(0x64) & 1))
	//{
	//    inportb(0x60);
	//}


	//printf("ps2_mouse_initialize: enable second port\n");
	//refresh_screen();


	// Ativar a segunda porta PS/2.
	//kbdc_wait(1);
	//outportb(0x64,I8042_WRITE);    
	//kbdc_wait(1);
	//outportb(0x64,I8042_ENABLE_SECOND_PORT); //0xA8


	//printf("ps2_mouse_initialize: enable second device\n");
	//refresh_screen();

	// Activar o segundo despositivo PS/2, modificando o status de 
	// configura��o do controlador PS/2. 
	// Lembrando que o bit 1 � o respons�vel por habilitar, desabilitar o 
	// segundo despositivo PS/2  ( o rato). 
	// S� para constar se vedes aqui fizemos duas coisas lemos ao mesmo tempo 
	// modificamos o byte de configura��o do controlador PS/2 
	
	// Defina a leitura do byte actual de configura��o do controlador PS/2.
	//0x20 Read "byte 0" from internal RAM

	kbdc_wait(1);
	outportb(0x64,I8042_READ);
	kbdc_wait(0);
	status = inportb(0x60)|2;  
	//status = inportb(0x60)| 3;  


	// defina, a escrita  de byte de configura��o do controlador PS/2.
	kbdc_wait(1);
	outportb(0x64,I8042_WRITE);  
	// devolvemos o byte de configura��o modificado.
	kbdc_wait(1);
	outportb(0x60,status);


	//Agora temos dois dispositivos sereais teclado e mouse (PS/2).

	//Activar a primeira porta PS/2
	kbdc_wait(1);
	outportb(0x64,0xAE);  

	// activar a segunda porta PS/2
	kbdc_wait(1);
	outportb(0x64,0xA8);


	// espera 
	kbdc_wait(1);


	//## step 2 ##
	//checar se o mouse � de rolar ou n�o.
	
	//## step 3 ##
	//detectar a quantidade de bot�es.

	//## step 4 ##
	//configurar o sampling rate.
	//ativa o modo escrita,
	//envia o comando seguido de par�metros.
	//0xF3 ...

	//## step 5 ##
	//configura a resolu��o do mouse
	//0xE8 ...

	//## step 6 ##	


	//printf("ps2_mouse_initialize: restores defaults\n");
	//refresh_screen();

	//Restaura defaults do PS/2 mouse.
	//kbdc_wait(1);
	//outportb(0x64,I8042_WRITE);    
	//kbdc_wait(1);
	//mouse_write(MOUSE_SET_DEFAULTS);  //0xf6
	//while ( mouse_read() != I8042_ACKNOWLEDGE );


	//printf("ps2_mouse_initialize: enable transmission\n");
	//refresh_screen();	

   // TODO: Pode ser interessante diminuir a sensibilidade do mouse
   // aqui!!!
  //( 0xF4 = Habilita o mouse streaming, Enable Data Reporting,ENABLE MOUSE TRANSMISSION )
  // Habilita o mouse streaming
  // Interessante notar que, no modo streaming,
  // 1 byte recebido do PS/2 mouse gerar  uma IRQ...
  // Talvez valha a pena DESABILITAR o modo streaming
  // para colher os 3 dados de uma s� vez na IRQ!
	//kbdc_wait(1);
	//outportb(0x64,I8042_WRITE);      
    //kbdc_wait(1);
    //mouse_write(MOUSE_ENABLE_DATA_REPORTING);  //0xf4
    //while ( mouse_read() != I8042_ACKNOWLEDGE );        


	//uint8_t tmp = inportb(0x61);
	//outportb(0x61, tmp | 0x80);
	//outportb(0x61, tmp & 0x7F);
	//inportb(0x60);	//mouse port

	// Flush the output buffer
	//while ((inportb(0x64) & 1)) {
	//	inportb(0x60);
	//}


init_mouse_exit:

	// Se ouve um erro, ent�o analisamos e exibimos mensagem sobre ele.
    if ( error_code != 0 )
    {
        //
    }

	//printf("ps2_mouse_initialize: done\n");
	//refresh_screen ();

	//#imporante:
	//n�o habilitaremos e n�o resetaremos o dispositivo.
	//habilitar e resetar fica para a inicializa��o do ps2.
}




/*
 ***********************************************
 * ps2_mouse_globals_initialize:
 *     Inicializando o mouse no controlador 8042.
 *     Carregando o bmp para o curso do mouse.
 * 
 * History:
 *     2018 - Created by Fred Nora.  
 */

int ps2_mouse_globals_initialize (void){

    unsigned char response = 0;
    unsigned char deviceId = 0;
    int i; 
    int bruto = 1;  //M�todo.
    int mouse_ret;


	//printf("ps2_mouse_globals_initialize: inicializando estrutura\n");
	//refresh_screen ();


	//user.h
    ioControl_mouse = (struct ioControl_d *) malloc ( sizeof(struct ioControl_d) );

    if ( (void *) ioControl_mouse == NULL )
    {

        panic ("ps2_mouse_globals_initialize: ioControl_mouse fail\n");

    }else{

        ioControl_mouse->id = 0;
        ioControl_mouse->used = 1;
        ioControl_mouse->magic = 1234;

		//Qual thread est� usando o dispositivo.
        ioControl_mouse->tid = 0;  
        //ioControl_mouse->
    };


	//printf("ps2_mouse_globals_initialize: inicializando variaveis\n");
	//refresh_screen ();

    // #importante
    // habilitando o mouse ps2.
    ps2_mouse_status = 1;


	// Estamos espa�o para o buffer de mensagens de mouse.
	// mousemsg = ( unsigned char *) malloc(32);


	//Inicializando as vari�veis usadas na rotina em Assemly
	//em hardwarelib.inc

    //#todo:
    //Podemos inicialziar com o mouse no centro da tela.

	//Coordenadas do cursor.
    g_mousepointer_x = (unsigned long) 0;
    g_mousepointer_y = (unsigned long) 0;
    mouse_x = 0;
    mouse_y = 0;


	// #bugbug: 
	// Essa inicializa��o est� travando o mouse.
	// Fazer com cuidado.


	//#bugbug. Cuidado com essa inicializa�ao.
    g_mousepointer_width = 16;
    g_mousepointer_height = 16;


    //Bytes do controlador.
    //mouse_packet_data = 0;
    //mouse_packet_x = 0;
    //mouse_packet_y = 0;
    //mouse_packet_scroll = 0;


	//
	// ## BMP ##
	//


	//printf ("ps2_mouse_globals_initialize: carregando bmp\n");
	//refresh_screen();

	// Carregando o bmp do disco para a mem�ria
	// e apresentando pela primeira vez.

    mouse_ret = (int) load_mouse_bmp ();

    if (mouse_ret != 0)
    {
        panic ("ps2_mouse_globals_initialize: load_mouse_bmp\n");
    }



	//printf("ps2_mouse_globals_initialize: done\n");
	//refresh_screen ();

    //initialized = 1;
    //return (kernelDriverRegister(mouseDriver, &defaultMouseDriver));


    return 0;
}


void set_ps2_mouse_status(int status)
{
	ps2_mouse_status = status;
}

int get_ps2_mouse_status(void)
{
	return (int) ps2_mouse_status;
}


unsigned long 
ps2_mouse_dialog ( int msg,
                   unsigned long long1,
                   unsigned long long2 )
{
    switch (msg)
    {
		//habilitar
        case 4000:
            printf ("ps2_mouse_dialog: 4000\n");
            refresh_screen();
            ps2_mouse_status = 1;
            break;

        //desabilitar.
        case 4001:
            printf ("ps2_mouse_dialog: 4001\n");
            refresh_screen();
            ps2_mouse_status = 0;
            break;

        //#test
        // reinicializar ??
        case 4002:
            break;
            
        default:
            break;
    };


    return 0;
}


//
// End.
//

