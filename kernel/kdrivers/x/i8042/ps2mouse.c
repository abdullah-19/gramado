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


// o driver de mouse entrega informa��es sobre o evento atual
// para quem pedir. Como o servidor de janelas atual.
unsigned long ps2_mouse_get_info ( int i )
{
    switch (i)
    {
        case 1:
            return (unsigned long) saved_mouse_x;
            break;

        case 2:
            return (unsigned long) saved_mouse_y;
            break;
            
        case 3:
            return (unsigned long) mouse_x;
            break;
            
        case 4:
            return (unsigned long) mouse_y;
            break;
            
        case 5:
            return (unsigned long) ps2_mouse_moving;
            break;

        case 6:
            return (unsigned long) ps2_mouse_drag_status;
            break;
            
        case 7:
            return (unsigned long) mouse_button_action;
            break;            


        case 8:
            return (unsigned long) mouse_buttom_1;
            break;
            
        case 9:
            return (unsigned long) mouse_buttom_2;
            break;
            
        case 10:
            return (unsigned long) mouse_buttom_3;
            break;



        case 11:
            return (unsigned long) old_mouse_buttom_1;
            break;
            
        case 12:
            return (unsigned long) old_mouse_buttom_2;
            break;
            
        case 13:
            return (unsigned long) old_mouse_buttom_3;
            break;
            
            
        case 14:
            return (unsigned long) ps2_button_pressed;
            break;

          //...
    }	
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

int __do_scan;

void mouseHandler (void){

	// #importante:
	// Essa ser� a thread que receber� a mensagem.
	//struct thread_d *t;


	// #importante:
	// Essa ser� a janela afetada por qualquer evento de mouse.
	// ID de janela.

    //struct window_d *Window;
    //int wID; 
    
    //#test
    //int last_wID;



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
            
            __do_scan = 0; //n�o escaneie ainda.
            break;


		// >> Essa foi a segunda interrup��o.
        case 1:
            //Pegamos o segundo char.
            buffer_mouse[1] = (char) *_byte;
            count_mouse++;
            
            __do_scan = 0; //n�o escaneie ainda.
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
				
				
				//#todo: drag support
				//if ( ps2_button_pressed == 1 )
				//{ 
				//    ps2_mouse_drag_status = 1;
                //}else{
				//    ps2_mouse_drag_status = 0;
				//}

                   
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
			__do_scan = 1; //agora pode escanear
            break;


        // Problemas na contagem de interrup��es.
        default:
            count_mouse = 0;
            __do_scan = 0; //n�o escaneie ainda.
            return;
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


    // >> Daqui pra frente � opera��o envolvendo janelas.
    // Passremos o comando para o servidor de janelas,
    // pois o driver de mouse ps2 n�o entende de janelas.  
    // >> Mas antes ja passamos informa��es sobre o evento.

    //
    // Scan 
    //
    
    // #bugbug
    // Estamos chamando o scan.
    // Mas isso deveria ser o trabalho do servidor de janelas
    // que faria isso num loop pegando os eventod de uma fila de eventos.
    
    if (__do_scan == 1)
    {
		kgws_mouse_scan_windows ();
	    __do_scan = 0;
	}
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

