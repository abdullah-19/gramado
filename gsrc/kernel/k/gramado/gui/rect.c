/*
 * File: rect.c
 *
 * Descri��o:
 *     Rotina de pintura de retangulos.
 *     Faz parte do m�dulo Window Manager do tipo MB.
 *
 * History:
 *    2015 - Created by Fred Nora.
 */



#include <kernel.h>


//
//===============================================================
// refresh rect - Fred. P.
//
// @todo:
// Nessa macro podemos usar vari�veis globais e inicializar
// essas vari�vel. E considerarmos valores como g_bpp, g_screen_width. 
//


//#define RGB_PIXEL_OFFSET(x,y) \
//( (3*800*(y)) + (3*(x)) )

//
// #define RGB_PIXEL_OFFSET(x,y) \
// ( (3*screenGetWidth()*(y)) + (3*(x)) )
//
// #define RGB_PIXEL_OFFSET(x,y) \
// ( ( screenGetBPP() * screenGetWidth()*(y)) + ( screenGetBPP() *(x)) )

// #define RGB_PIXEL_OFFSET(x,y) \
// ( ( g_bpp * g_screen_width *(y)) + ( g_bpp *(x)) )

//================================================



//Usada no refresh rectangle.
#define BUFFER_PIXEL_OFFSET(x,y) \
( (3*800*(y)) + (3*(x)) )




//
// @todo: Criar uma estrutura para o elemento gr�fico. entre os elementos da estrutura
// pode ter os buffer para o char. backbuffer, frontbuffer, dedicatedbuffer.
//
//


/*
 * rectDrawRectangleWindowBuffer:
 *     Pinta um ret�ngulo no buffer da janela.
 *     Serve para pintar janelas que ir�o direto do seu buffer para o LFB da 
 * mem�ria de v�deo, sem passar pelo back buffer. (OVERLAPPED)
 *
 */
/*
void rectDrawRectangleWindowBuffer(struct window_d *window, 
                                   unsigned long x, 
                                   unsigned long y, 
						           unsigned long width, 
						           unsigned long height, 
						           unsigned long color ); 
void rectDrawRectangleWindowBuffer(struct window_d *window, 
                                   unsigned long x, 
                                   unsigned long y, 
						           unsigned long width, 
						           unsigned long height, 
						           unsigned long color )
{
    return;	
}
*/


/* 
void rectDrawRectangle( struct window_d *window, struct rect_d *rect);
void rectDrawRectangle( struct window_d *window, struct rect_d *rect)
{
	struct window_d *hwndDesktop;
	
    hwndDesktop = guiGetMainWindow();
	
	// Criaremos o ret�ngulo na janela principal caso o ponteiro 
	//passado seja inv�lido.
	
    if( (void*) window == NULL ){
        window = hwndDesktop;
    };

    unsigned long x; 
    unsigned long y; 
	unsigned long width; 
	unsigned long height; 
	unsigned long color;

    x      = window->left + rect->x;	
	y      = window->top + rect->y;
	width  = rect->width;
	height = rect->height;
	color = rect->color_bg;
	
    drawDataRectangle( x, y, width, height, color );
	
}						
*/

 
/*
 * drawDataRectangle:
 *     Cria um ret�ngulo no Backbuffer. Em qualquer posi��o do Backbuffer
 * dado o limite do tamanho da tela.
 *     @todo: Criar rotina para pintar um ret�ngulo em um buffer dedicado.
 *            Toda janela tem um buffer dedicado. O BackBuffer � o buffer
 * dedicado da janela principal. 'gui->main'
 *
 *
 */
void drawDataRectangle( unsigned long x, 
                        unsigned long y, 
						unsigned long width, 
						unsigned long height, 
						unsigned long color )
{
	
	struct rect_d *rect;
	
	//A estrutura base � usada para efetuar essa rotina aqui,
	//e depois permanece suja.
    
	//bg
    rect->color_bg = color;
   
    //Dimens�es.
	rect->x = 0;        
    rect->y = 0;         
    rect->width = width;   
    rect->height = height;    
   
    //Margens.
    rect->left = x;    
    rect->top  = y;
    rect->right = rect->left + rect->width;
    rect->bottom = rect->top + rect->height; 

    //Limits.
	//@todo: Repensar os limites para uma janela.
	// Uma janela poder� ser maior que as dimens�es de um dispositivo.
	// mas n�o poder� ser maior que as dimens�es do backbuffer.
	// Ou seja: O dedicated buffer de uma janela deve ser menor que
	// o backbuffer.
    if( rect->right > 800 ){
        rect->right = 800;
	};	

	/* @todo:
    if( rect->bottom > 600 ){
        rect->bottom = 600;
	};
    */	
  	
    //Pinta as linhas no Backbuffer.  
	while( height-- )
	{
	    my_buffer_horizontal_line( rect->left, 
		                           y, 
								   rect->right, 
								   rect->color_bg );
		y++;
    };    
  
    //Nothing.
	
done: 
    return;
};
  
  
  
/*
 * getClientAreaRect:
 *     Obt�m o ponteiro da estrutura do ret�ngulo  
 *     da �rea de cliente da janela ativa.
 *     @todo: oferecer esse servi�o para a api.
 */  
void *getClientAreaRect(){
    return (void *) rectClientArea;	
};

/*
 * setClientAreaRect:
 *     Inicializa a estrutura do ret�ngulo
 *     da �rea de cliente da janela ativa.
 *
 */
void setClientAreaRect( unsigned long x, 
                        unsigned long y, 
						unsigned long cx, 
						unsigned long cy )
{
    struct rect_d *r;

	r->x = x;
	r->y = y;
	r->cx = cx;
	r->cy = cy;
	
	//Se a estrutura ainda n�o foi inicializada.
    if((void*) rectClientArea == NULL){
	    return;
	};
	 
    rectClientArea = (void *) r;
    return;	
};




/*
 ***********************************************************
 * refresh_rectangle:
 *     Copiar um ret�ngulo do backbuffer para o frontbuffer.   
 * 
 * Hist�rico:
 *     2017 - Criado por Frederico Lamberti Pissarra.
 *     2018 - Fred Nora.
 * 
 */					
void refresh_rectangle( unsigned long x, 
                        unsigned long y, 
						unsigned long width, 
						unsigned long height )
{    
	void *p = (void*) FRONTBUFFER_ADDRESS;		
	void *q = (void*) BACKBUFFER_ADDRESS;

	unsigned int line_size, lines;
	unsigned int offset;
	unsigned long Width  = (unsigned long) screenGetWidth();
	unsigned long Height = (unsigned long) screenGetHeight();	

	line_size = (unsigned int) width; 
	lines     = (unsigned int) height;
	offset    = (unsigned int) BUFFER_PIXEL_OFFSET( x, y );
	p = (void*) (p + offset);    
	q = (void*) (q + offset);    
	 
    vsync();	
	
	unsigned int i;
	for( i=0; i < lines; i++ )
	{
		memcpy( p, q, (line_size*3) );
		q += (Width*3);
		p += (Width*3);
	};	
};



/*
int rectInit()
{}  
*/



//
// End.
//
