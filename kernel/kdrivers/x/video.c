/*
 * File: unb\video.c
 *
 * Descri��o: 
 *     Rotinas paga gerenciar os controladores de video.
 *     Crt, Vga ...
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2017 - Revision and small changes.
 */


#include <kernel.h>


//Defini��es internas.

//@todo: Criar um arquivo para gerenciar fontes.
#define VIDEO_BIOS_FONT8X8_ADDRESS 0x000FFA6E
//#define VIDEO_BIOS_FONT8X16_ADDRESS (0x000FFA6E+??)
//...

#define VIDEO_BIOS_FONT8X8_WIDTH   8
#define VIDEO_BIOS_FONT8X8_HEIGHT  8
//...

//
// Vari�veis importadas.
//

//??
extern unsigned long SavedBootMode;

//Vari�veis. (head.s)
extern unsigned long SavedBootBlock;    //Par�mtros passados pelo Boot Loader.

extern unsigned long SavedLFB;          //LFB address.  
extern unsigned long SavedX;            //Screen width. 
extern unsigned long SavedY;            //Screen height.
extern unsigned long SavedBPP;          //Bits per pixel.
//...

//
// Vari�veis internas.
//

int videoStatus;
int videoError;
//...


/*
 * videoSetupCGAStartAddress:
 *     Configura o endere�o inicial da mem�ria de video em modo texto   
 *     fis=b8000  vir=0x800000 
 */

void videoSetupCGAStartAddress (unsigned long address){
	
    g_current_vm = (unsigned long) address;
	//g_current_cga_address
}


/*
 fis=a0000
void videoSetupVGAStartAddress( unsigned long address);
void videoSetupVGAStartAddress( unsigned long address)
{
    g_current_vga_address = (unsigned long) address;
	return;
};
*/


/*
 * get_video_mode: 
 *     Obtem o modo de video atual.
 */

//#todo
//isso pode ser incluido em 'get system parameters' system call.

unsigned long videoGetMode (void){
	
	return (unsigned long) g_current_video_mode;
}


/*
 * videoSetMode:
 *     Configura o modo de video atual.
 */
void videoSetMode (unsigned long mode){
	
    unsigned long VideoMode;
    unsigned long Width;
	unsigned long Height;
	//continua...(outros parametros)

	
	VideoMode = (unsigned long) mode;
	
    //
	// todo: Check limits.
	//
	
	//if (){}
	

	//Se estiver nos limites.
	if ( VideoMode > 0 && VideoMode < 9000)
	{
		//g_current_video_mode = (unsigned long) VideoMode;
        g_video_mode = (unsigned long) VideoMode;
		VideoBlock.vesaMode = (unsigned long) VideoMode; 
		//...
	};
	
	
	//
	// @todo:
	// Organizar isso. Os valores atuais devem ser obtidos em real mode 
	// usando os recursos do VESA BIOS.
	//
	
    switch (VideoMode){
		
	    case 0x110: 
		    Width = 800;
			Height = 600;
		break;

	    case 0x111:
		    Width = 800;
			Height = 600;
		break;

	    case 0x112:
		    Width = 800;
			Height = 600;
		break;

	    case 0x113:
		    Width = 800;
			Height = 600;
		break;

	    case 0x114:
		    Width = 800;
			Height = 600;
		break;
		
	    case 0x115:
		    Width = 800;
			Height = 600;
		    break;
	
	    default:
		    Width = 800;
			Height = 600;
		    break;
	};    
	
	
	screenSetSize (Width,Height);
	
	// Continua... (outros parametros)

    g_current_video_mode = (unsigned long) VideoMode;
};


/*
 ***********************************************
 * videoInit:
 *     Inicia vari�veis de video de acordo com o modo gr�fico utilizado.
 */ 
 
int videoInit (void){
	
	int Status = 0;
	

	// Se o modo de video nao esta habilitado
	if ( VideoBlock.useGui != 1 )
	{
	    panic ("videoInit");
	}
		
    g_useGUI = 1;
	VideoBlock.useGui = 1;
	    	
	//endere�o f�sico do frontbuffer.
	g_frontbuffer_pa = (unsigned long) SavedLFB;              

	//endere�o virtual do backbuffer.
	g_backbuffer_va = (unsigned long) BACKBUFFER_VA;
			
	//Device screen sizes. (herdadas do boot loader.)
	g_device_screen_width = (unsigned long) SavedX;
	g_device_screen_height = (unsigned long) SavedY;
	
	
	// #importante
	// Nesse momento podemos testar o LFB.
	// Enviando alguma coisa diretamente pra la' ...
	
	
	// #DEBUG
	// vamos tentar usar isso como breakpoint.
	// Ok isso funcionou na gigabyte/intel ... vamos usar isso por enquanto.
    //lfb_putpixel ( COLOR_YELLOW, 11, 11, 0 );
	//while(1){}
		
	//@todo: #bugbug Para configurar a estrutura HostDeviceInfo tem que 
	//alocar mem�ria antes, por isso s� faremos isso depois de 
	//inicializarmos o m�dulo runtime. /rt.
	
	//Isso pode n~ao funcionar pois nao temos
	//os endere�os corretos ainda.
	
	//Background.
	
	//#bugbug #hackhack	
	//vamos suprimir essa mensagem para testar
	//o kernel na maquina real gigabyte/intel
	//#importante: nuca mais usaremos essa rotina de bg aqui.
	//vamos passar ela para um ponto onde ela ja possa ser usada,
    
    
	// #DEBUG
	// breakpoint
    //vamos testar se 'e a rotina de bg que est'a falhando...
    //caso seja ela, ent~ao vamos suprimila se possivel.
	//ok isso funcionou, vamos avan�ar.
	
    //lfb_putpixel ( COLOR_YELLOW, 11, 11, 0 );
	//while(1){}

    
		
	//@todo: Isso deve fazer parte do construtor.
		
	//Poder�amos copiar a font da ROM para a RAM.
		
	//
	// ## Font support ##
	//
		
	//Font. (BIOS font).
	//#bugbug: 
	//Na verdade video.c n�o tem acesso a essa vari�vel,
	//� preciso chamar o servidor atrav�s de um m�todo para configur�-la.
        
	gwsSetCurrentFontAddress ( VIDEO_BIOS_FONT8X8_ADDRESS );		
 
	//#todo: usar a fun��o que configura essas vari�veis.
	gcharWidth = VIDEO_BIOS_FONT8X8_WIDTH;
	gcharHeight = VIDEO_BIOS_FONT8X8_HEIGHT;		
	gfontSize = FONT8X8;
			    
	//Cursor. 
	g_cursor_x = 0;
	g_cursor_y = 8;	
	g_cursor_width = 8;
	g_cursor_height = 8;
	g_cursor_color = COLOR_TERMINALTEXT;
        
	//@todo; Criar defines para default.
	g_cursor_left = 0;      // Margem esquerda dada em linhas.
    g_cursor_top = 0;       // Margem superior dada em linhas.
	g_cursor_right  = 256;  // Margem direita dada em linhas.
    g_cursor_bottom = 256;  // Margem inferior dada em linhas.
		
	set_up_cursor(0,0);    //Cursor.
	
	
	//Continua ...
		
	//
	// Outras configura��es de v�deo independentes do modo de v�deo.
	//
	
	//
    // @todo: Sizes, atribute, atribute color, row, column
	//

    g_driver_video_initialized = 1;
	
	
	
	
	// #DEBUG
	// breakpoint
    // ok isso funcionou na gigabyte/intel , vamos avan�ar ...
   // lfb_putpixel ( COLOR_YELLOW, 11, 11, 0 );
   //	while(1){}


	
	
#ifdef BREAKPOINT_TARGET_AFTER_VIDEO
    //#debug 
	//a primeira mensagem s� aparece ap�s a inicializa��o da runtime.
	//por isso n�o deu pra limpar a tela antes.
	printf(">>>debug hang: after video");
	refresh_screen(); 
	while (1){
		asm ("hlt");
	}
#endif		
	
    return (int) Status;    
}


/*
 * videoVideo:
 *     M�todo construtor.
 *     Fun�ao interna. 
 *     Confgura algumas vari�veis.
 *     @todo: isso poderia ter retorno void.
 */

int videoVideo (void){
	
    videoStatus = 0;
    videoError = 0;
    //...

	return 0;
}


//
// End.
//

