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
//address = endere�o base
//x       = posicionamento 
//y       = posicionamento
// deletar w h
void bmpDisplayBMP( void *address, unsigned long x, unsigned long y, int width, int height )
{
	int i, j, base, offset;
	
	unsigned long left, top, bottom;
	
	unsigned long color;
	unsigned long Width;
	unsigned long Height;
	
	
	
	//limits
	
	//@todo: Refazer isso
	if( x > 800 ){ return; }
	if( y > 600 ){ return; }
	//if( width > 800 ){ return; }
	//if( height > 600 ){ return; }
	
	
	//
	// struct.
	//
	
	//struct bmp_header_d *b;
	struct bmp_infoheader_d *bi = (struct bmp_infoheader_d *) address;
	
	//bi = (void*) malloc( sizeof(struct bmp_infoheader_d) );
	//.. check
	
	//base do bmp carregado na mem�ria
	unsigned char *bmp = (unsigned char *) address;
	unsigned char *c   = (unsigned char *) &color;	
	//unsigned char *w   = (unsigned char *) &Width;
	//unsigned char *h   = (unsigned char *) &Height;
	
	


    // extract image height and width from header
    Width  = *( unsigned long* )&bmp[18];
    Height = *( unsigned long* )&bmp[22];	
	
	
	bi->bmpWidth    = (unsigned long) Width;
	bi->bmpHeight   = (unsigned long) Height;
	//bi->bmpBitCount = (unsigned long)
	
	//unico suportado ainda.
	if(bi->bmpBitCount != 24 ){
		//fail
	}
	
	
	
	
	//
	// Draw !
	//
	
	left = x;    //
	top  = y; 
	//bottom = top + height;
	bottom = (top + bi->bmpHeight );

	
	base = 0x36;  //in�cio da �rea de dados do bmp
	
	for(i=0; i < bi->bmpHeight; i++)	
	{
		//#bugbug: 
		//Precisa ser o valor encontrado na estrutura +1.
		for(j=0; j < bi->bmpWidth; j++)	
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
	
	
	//Debug
	
	printf("w={%d} h={%d}\n",bi->bmpWidth ,bi->bmpHeight);
	
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

