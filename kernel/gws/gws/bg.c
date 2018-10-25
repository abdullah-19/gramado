/*
 * File: gws\gws\bg.c 
 *
 * Descri��o:
 *     Rotinas de configura��o do background de um desktop.
 *     O background de um desktop � onde fica a imagem de pano de fundo.
 * Tamb�m � conhecido como Backbuffer.
 *
 * Window server.
 *
 * History:
 *     2013 - Created by Fred Nora.
 *     2016 - Revision.
 */


#include <kernel.h>

 
/*
 * backgroundDraw:
 *     Pinta um background em modo gr�fico.
 *     Limpa a tela em modo texto. 
 *   pinta a tela toda.
 *   #bugbug: As dimens�es da tela s�o inv�lidas e d� pagefault.
 */
void backgroundDraw (unsigned long color){
	
    unsigned long i;
	
	//unsigned long Left = (unsigned long) SCREEN_DEFAULT_LEFT;
	//unsigned long Top  = (unsigned long) SCREEN_DEFAULT_TOP;
	//unsigned long Width = (unsigned long) screenGetWidth();
	//unsigned long Height = (unsigned long) screenGetHeight();	
	
	
	//#debug
	//if( Width != 800 )
	//{
	//	printf("Width");
	//	refresh_screen();
	//	while(1){}
	//}

	//#debug
	//if( Height != 600 )
	//{
	//	printf("Height");
	//	refresh_screen();
	//	while(1){}
	//}
	
	//Modo gr�fico.
//drawGraphicsMode:
	
	if (VideoBlock.useGui == 1)
	{
		// @todo: Usar vari�vel global. 
		// Ou vari�vel presente na estrutura 'gui->'
		
		//backgroundSetColor( (unsigned long) color);
		
		for ( i=0; i<600; i++ ){
			
			my_buffer_horizontal_line ( 0, i, 800, color );
		};

		//for(i=0; i<Height; i++){
		//	my_buffer_horizontal_line( 0, i, Width, color);
		//};
		
        goto done; 		
	};
	
	//Modo texto.
//drawTextMode:
	
	if (VideoBlock.useGui == 0)
	{
		kclear(0);
		goto done; 
	};
	
	//Nothing
done:	
	
	g_cursor_x = 0;
	g_cursor_y = 0;

    //@todo:
    //usar uma vari�vel.
    //Cuidado para n�o dividir por zero.
    
	//if( gcharWidth <= 0 || gcharWidth >= Width ){
	//	gcharWidth = DEFAULT_CHAR_WIDTH;
	//} 

	//if( gcharHeight <= 0 || gcharHeight >= Height ){
	//	gcharHeight = DEFAULT_CHAR_HEIGHT;
	//} 
	
	//g_cursor_right  = (Width/gcharWidth);
	//g_cursor_bottom = (Height/gcharHeight);

	g_cursor_right  = (800/8);
	g_cursor_bottom = (600/8);
	
    return;
};


/*
 * backgroundSetColor:
 *     Set background color in graphic mode.
 */
void backgroundSetColor (unsigned long color){
	
    if ( gui->backgroundStatus == 0 ){
		
        return;
    };	

	if ( (void *) gui->background != NULL ){
		
	    gui->background->bg_color = (unsigned long) color;
	};
};


/*
 * backgroundRedraw:
 *     Redraw bg.
 */
void backgroundRedraw (unsigned long color){
	
    if ( gui->backgroundStatus == 0 ){
		
        return;
    };
	
	if ( (void *) gui->background != NULL ){
		
	   backgroundDraw (color);	
	};
};


//Constructor.
void backgroundBackground (){
	
	g_cursor_x = 0;
	g_cursor_y = 0; 
};


//Init.
int backgroundInit (){
	
	// #todo:
	// Ainda n�o implementada.
	
	backgroundBackground ();
	//...
	
	return (int) 0; 
};	


//
//End.
//

