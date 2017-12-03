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
 * drawDataRectangle:
 *     Cria um ret�ngulo no Backbuffer. Em qualquer posi��o do Backbuffer
 * dado o limite do tamanho da tela.
 *     @todo: Criar rotina para pintar um ret�ngulo em um buffer dedicado.
 *            Toda janela tem um buffer dedicado. O BackBuffer � o buffer
 * dedicado da janela principal. 'gui->main'
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
	while(height--){
	    my_buffer_horizontal_line( rect->left, y, rect->right, rect->color_bg);
		y++;
    };    
  
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
int rectInit()
{}  
*/

					
//copiar apenas um ret�ngulo do backbuffer para o frintbuffer.						
void refresh_rectangle( unsigned long x, 
                        unsigned long y, 
						unsigned long width, 
						unsigned long height )
{
    vsync();
    
    //calcular quantos bytes tem no ret�ngulo	
    int i;
	unsigned long color;
	
    for(i=0; i< (width*height*3); i++)
	{
		color = (unsigned long) get_pixel( x, y );
	    refresh_pixel(  x,  y, color );
	}	
};




//
// End.
//
