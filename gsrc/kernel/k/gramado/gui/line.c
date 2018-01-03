/*
 * File: line.c
 *
 * Descri��o:
 *     Rotinas de pintura de linha.
 *     Faz parte do m�dulo Window Manager do tipo MB.
 *
 * History:
 *     2015 - Created by Fred Nora.
 */


#include <kernel.h>



/*
 * lineDrawHorizontalLineWindowBuffer:
 *     Pinta uma linha horizontal no buffer da janela.
 *     Serve para pintar janelas que ir�o direto do seu buffer para o LFB da 
 * mem�ria de v�deo, sem passar pelo back buffer. (OVERLAPPED)
 *
 */
/* 
void lineDrawHorizontalLineWindowBuffer(struct window_d *window,
                                        unsigned long x1,
										unsigned long y,
										unsigned long x2,
										unsigned long color );
										
void lineDrawHorizontalLineWindowBuffer(struct window_d *window,
                                        unsigned long x1,
										unsigned long y,
										unsigned long x2,
										unsigned long color )
{
    return;	
}
*/


//@todo: draw h line  x y width

/*
 * my_buffer_horizontal_line:
 *     Pinta uma linha horinzontal no backbuffer.
 */
void my_buffer_horizontal_line( unsigned long x1,
                                unsigned long y, 
								unsigned long x2,  
								unsigned long color )
{
	while(x1 < x2){
        my_buffer_put_pixel( color, x1, y, 0);
        x1++;  
    };
	return;
};




 

							  
void refresh_horizontal_line( unsigned long x1,
                              unsigned long y, 
			  				  unsigned long x2 )
{
    
	//unsigned char *backbuffer = (unsigned char *) BACKBUFFER_ADDRESS;	
	//unsigned char *frontbuffer = (unsigned char *) FRONTBUFFER_ADDRESS;		
    
	/*
	void *s = (void *) FRONTBUFFER_ADDRESS;
	void *d;
	
	s = (void*) (s) + (y*3*800) + (x1*3);
	d = (void*) (s) + (x2*3); 
	
	memcpy(d,s, x2-x1);
	*/
	
	/*
	unsigned long X = x1;
	unsigned long Y = y;
	
	int i;
	for( i=0; i< (x2-x1); i++ )
	{
	    refresh_pixel( X, Y );
		X++; 
	};
	*/
	
	//void *s = (void*) BACKBUFFER_ADDRESS;	
	//void *d = (void*) FRONTBUFFER_ADDRESS;
	
	void *s = (void*) (BACKBUFFER_ADDRESS)  + (y*3*800) + (x1*3);
    void *d = (void*) (FRONTBUFFER_ADDRESS) + (y*3*800) + (x2*3);
	
	//n�o pode ser isso. tem que ser uma string de tamanho definido.
	//strcpy(d,s);	
	
	unsigned long size = (unsigned long) ((x2-x1)*3); 
	
	memcpy(  (void *) d, (const void *) s, size);
	return;
};



//OP��ES
//bcopy(char *from, char *to, int len)
//strcpy(char *to, const char *from)
//memcpy(void *v_dst, const void *v_src, unsigned long c)
//strcpy(char *to, const char *from)
//fim.
//
