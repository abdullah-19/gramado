/*
 * File: bmp.c
 *
 * Descri��o:
 *     Rotinas para bmp.
 *     Faz parte do m�dulo Window Manager do tipo MB.
 *
 * Obs: 
 *     As rotinas de bmp precisam melhorar.
 *     Conferir os parametros do bmp em seu header.
 *     todo: Criar uma rotina para filtrar o bmp por tamanho e tipo.
 *            bugbug: bmp envolve o carregamento de arquivo.
 *            Temos a op��o de utilizarmos arquivos que foram carregados
 * na inicializa��o, como parte dos elementos da interface gr�fica.
 * 
 * History: 
 *     2015 - Created by Fred Nora. 
 */


#include <kernel.h>


//
// Fun��es importadas.
//


//@todo:
// ?? De onde vem essa fun��o ???
extern void my_buffer_load_bitmap_16x16();





//mostra na tela uma imagem bmp carregada na mem�ria.
void bmpDisplayBMP( void *address, unsigned long x, unsigned long y, int width, int height )
{
	int i, j, base, offset;
	
	unsigned long left, top, bottom;
	
	unsigned long color;
	
	base = 0x36;  //in�cio da �rea de dados do bmp
	
	//limits
	
	//@todo: Refazer isso
	if( x > 800 ){ return; }
	if( y > 600 ){ return; }
	if( width > 800 ){ return; }
	if( height > 600 ){ return; }
	
	left = x;    //
	top  = y; 
	bottom = top + height;
	
	//base do bmp carregado na mem�ria
	unsigned char *bmp = (unsigned char *) address;
	unsigned char *c   = (unsigned char *) &color;
	
	
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{	
			//construindo o char.
			
			offset = base;
			c[1] = bmp[offset];
			
			offset = base+1;
			c[2] = bmp[offset];
			
			offset = base+2;
			c[3] = bmp[offset];
			
			c[0] = 0;
			
			base = base + 3;
		
			my_buffer_put_pixel( (unsigned long) color, (unsigned long) left, (unsigned long) bottom, 0);
			
			left++; //pr�ximo pixel.
		}
		
		//vamos para a linha anterior.
		bottom = bottom-1;
		left = x;    //reiniciamos o x.
	};	
	
	return;
};







/* bugbug alloc
void bmpTest();
void bmpTest()
{
	printf("*teste ...\n");
	refresh_screen();
	//unsigned long buf[1024*2]; //3kb
	void *buf;
	
	buf = (void*) malloc( 3*1024);
	if(buf == NULL)
	{
	    printf("alloc.");
		refresh_screen();
		while(1){}
	}
	printf("Loading ...%x \n",(unsigned long)buf);
	refresh_screen();
	carregar_arquivo("FREDFILEBMP",(unsigned long) buf);
	load_bitmap_16x16( (unsigned long) buf, 100,100,0);
	printf("*done.");
	refresh_screen();
	while(1){}
};
*/


/*
 * carrega_bitmap_16x16:
 *    P�e bitmap de tamanho 16x16 no buffer.
 *
 * a - endre�o da imagem 
 * b - x
 * c - y 
 * d - null
 *
 * @todo: Deletar, tem outra igual logo abaixo.
 * Obs:   Buffer, n�o precisa esperar o retra�o vertical.
 */
void carrega_bitmap_16x16( unsigned long ax, 
                           unsigned long bx, 
                           unsigned long cx, 
                           unsigned long dx )
{
    load_bitmap_16x16( ax, bx, cx, dx );
    return;				  
};


/*
 * load_bitmap_16x16:
 *     P�e bitmap no backbuffer.
 *
 * Argumentos:
 *     eax = endere�o da imagem na mem�ria. 
 *     ebx = x no backbuffer.
 *     ecx = y no backbuffer.
 *     edx = null.
 *
 * Obs: Coloca no backbuffer, por isso n�o precisa de sincronizar o retra�o 
 * vertical.
 *      Talvez um bitmap deva ser colocado em seu pr�prio buffer dedicado.
 * Ou pintado no buffer dedicado de alguma janela.
 *
 * @todo: Mudar o nome da fun��o para bmpLoad16x16(...).
 */
void load_bitmap_16x16( unsigned long ax, 
                        unsigned long bx,
						unsigned long cx, 
						unsigned long dx )
{
    //todo: Checar par�metros no header do bmp.
	
	//Passa os argumentos via registradores.
    asm volatile(" \n " :: "a"(ax), "b"(bx), "c"(cx), "d"(dx) );
	
    my_buffer_load_bitmap_16x16();
    return;				  
};



/*
int bmpInit()
{}
*/

//
// End.
//

