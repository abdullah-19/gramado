/*
 * File: gui\button.c
 *
 * Descri��o:
 *     Rotinas de cria��o e gerenciamento de bot�es. 
 *     Faz parte do m�dulo Window Manager do tipo MB.
 *
 * Vers�o 1.0, 2015.
 */


#include <kernel.h>


/*
 * draw_button:
 *     Desenha um bot�o dado um tipo.
 *     
 * @todo: Medir o tamanho da string da tag.
 * @todo: O bot�o precisa de uma etiqueta, que pode ser uma string ou uma imagem.
 * Talvez uma estrutura de bot�o seja necess�rio, para armazenas coisas como 
 * deslocamento dentro da janela e endere�o da imagem carregada como etiqueta.
 *
 * Retorna o ponteiro pra estrutura do bot�o.
 *
 *
 */
void *draw_button( struct window_d *window,
                  unsigned char *string,
                  unsigned long type, 
                  unsigned long x, 
                  unsigned long y, 
                  unsigned long width, 
                  unsigned long height, 
                  unsigned long color)
{
    int i; //??
	int Selected;
	unsigned long border1;
    unsigned long border2;
	struct button_d *b;
	
	//validade da janela onde o bot�o est�.
	if((void*) window == NULL){
	    return;
	};
	

	//Alocando mem�ria para a estrutura do bot�o.
	b = (void*) malloc( sizeof(struct button_d) );
    if( (void*) b == NULL ){
		return NULL;
	}else{
		b->objectType = ObjectTypeButton;
		b->objectClass = ObjectClassGuiObjects;
		
	    b->used = 1;	
	    b->magic = 1234;
		
		b->string = string;  //Label.
        b->type = type;
        b->x = x;
        b->y = y;
        b->width = width;
        b->height = height;
		
        b->window = (void*) window;
        b->color = color; 	
        b->Next = NULL;		 
		//...
	};
	
	//Devemos colocar o ponteiro na lista encadeada de bot�es 
	//dentro da estrutura da janela.
	//if( (void*) window->buttonList == NULL ){
	//	window->buttonList = (void*)b;
	//}
	
	//Todo: Usar esquema padr�o de cores.
	
    //Quem chamou precisa saber ao menos o tipo de bot�o que quer.	
    if(type == 0){
		//printf("draw_button: The button needs a type.\n");
		//refresh_screen();
        return NULL;
    };    
	
    switch(type)
    {
        //N�o pressionado.
        case BN_DOWN:
		    Selected = 0;
			b->selected = 0;
		    border1 = COLOR_BUTTONHIGHLIGHT;
			border2 = COLOR_BUTTONSHADOW;
			b->border1 = COLOR_BUTTONHIGHLIGHT;
			b->border2 = COLOR_BUTTONSHADOW;
            goto do_draw_button;			
			break;
                                    
        //Precionado.
        case BN_UP:
		    Selected = 0;
		    b->selected = 0;
			border1 = COLOR_BUTTONSHADOW;
			border2 = COLOR_BUTTONHIGHLIGHT;
            b->border1 = COLOR_BUTTONSHADOW;
			b->border2 = COLOR_BUTTONHIGHLIGHT;
			goto do_draw_button;			
            break;
                       
         //Selecionado. (highlight)
        case BN_SELECTED:
		    Selected = 1;
			b->selected = 1;
		    border1 = COLOR_BUTTONHIGHLIGHT;
			border2 = COLOR_BUTTONSHADOW;
            b->border1 = COLOR_BUTTONHIGHLIGHT;
			b->border2 = COLOR_BUTTONSHADOW;
			goto do_draw_button;			
            break;
        
		default:  
		    return NULL; 
            break;    
    };  

//
// Do draw the button.
//
		
	
do_draw_button:

    //
	// Usaremos ret�ngulos para desenharmos o bot�o.
	//
   
	
	//bg
	drawDataRectangle( window->left +x, window->top +y, width, height, color);
    
	//board1, borda de cima e esquerda.
	drawDataRectangle( window->left +x, window->top +y, width, 1, border1);
	drawDataRectangle( window->left +x, window->top +y, 1, height, border1);

	//board2, borda direita e baixo.
	drawDataRectangle( window->left +x +width -1, 
	                   window->top +y, 
					   1, 
					   height, 
					   border2);
					   
	drawDataRectangle( window->left +x, 
	                   window->top  +y +height -1, 
					   width, 
					   1,
					   border2);

//
// Do draw label.
//
					   
do_draw_label:

	
	// Se vamos usar uma imagem ao inv�s de uma string.
	//if(useImage == 1{ ... goto done;}
	
	//button label								   
    if(Selected == 1)
	{
	    draw_string( window->left +x +8, 
		             window->top  +y +8, 
					 COLOR_WHITE, 
					 string);	
    }else{
	    draw_string( window->left +x +8, 
		             window->top  +y +8, 
					 COLOR_WINDOWTEXT, 
					 string);	
	};
	
	
	//
	// @todo: Algo mais?
	//
	
// Done! 
done:
    //Retornando o ponteiro para a estrutura do bot�o.
    return (void*) b;          
};


/*
int buttonInit()
{}
*/

//
// Fim.
//
