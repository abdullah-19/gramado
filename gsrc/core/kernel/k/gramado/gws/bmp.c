/*
 * File: gui\bmp.c
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
 *            24 bpp support.
 *     2018 - Display 4bpp, 8bpp, and 32bpp BMPs.
 */


#include <kernel.h>


// 4bpp support.
static int nibble_count_16colors = 0;



/*
 ********************************************************
 * bmpDisplayBMP:
 *
 *     Mostra na tela uma imagem bmp carregada na mem�ria.
 * 
 * IN:
 *     address = endere�o base
 *     x       = posicionamento 
 *     y       = posicionamento
 *
 *	// @todo: Criar defines para esses deslocamentos.
 */ 
int bmpDisplayBMP( char *address, 
                   unsigned long x, 
				   unsigned long y )
{
	int i, j, base, offset;
	
	unsigned long left, top, bottom;

	unsigned long Width, Height;
	unsigned long xLimit, yLimit;
	
	unsigned short sig;
	
	unsigned long color, color2;
	unsigned long pal_address;	
	
	struct bmp_header_d *bh;
	struct bmp_infoheader_d *bi;
	
	// Endere�o base do BMP que foi carregado na mem�ria
	unsigned char *bmp = (unsigned char *)  address;
	
	// Vari�vel para salvar rgba.
	unsigned char *c = (unsigned char *) &color;
    unsigned char *c2 = (unsigned char *) &color2;	
	
	unsigned long *palette    = (unsigned long *) (address + 0x36);		
	unsigned char *palette_index = (unsigned char *) &pal_address;	
	
	// Limits
	xLimit = 800;
	yLimit = 600;
	
	
	//@todo: Refazer isso
	if( x > xLimit || y > yLimit ){ 
        return (int) 1; 
	}
	

	
	//
	// @todo:
	// Testar validade do endere�o.
	//
	
	
	if( address == 0 )
	{
		//goto fail;
	};
	
	
	//
	// struct for Info header
	//
	
	//## BUGBUG 
	// Um malloc aqui pode esgotar o heap 
	//na hora de movimentar o mouse.
	//precisamos usar um buffer interno para
	//essa estrutura.
	
	char buffer[512];
	char buffer2[512];
	
	//bh = (struct bmp_header_d *) malloc( sizeof(struct bmp_header_d) );	
    bh = (struct bmp_header_d *) &buffer[0];
	if( (void*) bh == NULL )
	{
		//goto fail;
	}	
	
	// Signature.
	sig = *( unsigned short* ) &bmp[0];
	bh->bmpType = sig;
	
	// Size. ( 2 bytes )
	unsigned short Size = *( unsigned short* ) &bmp[2];
	bh->bmpSize = Size;
	
	
	//
	// struct for Info header
	//
	
	//Windows bmp.
	//bi = (struct bmp_infoheader_d *) malloc( sizeof(struct bmp_infoheader_d) );
    bi = (struct bmp_infoheader_d *)  &buffer2[0];
	if( (void*) bi == NULL )
	{
		//goto fail;
	}	
	
	//The size of this header.
	bi->bmpSize = *( unsigned long* ) &bmp[14];
	
	// Width and height.
    Width = *( unsigned long* ) &bmp[18];
    Height = *( unsigned long* ) &bmp[22];	
	
	//@todo: checar validade da altura e da largura encontrada.
	
	// Salvar.
	bi->bmpWidth = (unsigned long) Width;
	bi->bmpHeight = (unsigned long) Height;
	
	
	/* Number of bits per pixel */
	//1, 4, 8, 16, 24 and 32.
	bi->bmpBitCount = *( unsigned short* ) &bmp[28];
	
	// �nico suportado ainda.
	if(bi->bmpBitCount != 24 )
	{
		//fail
	}
	
	
	// 0 = Nenhuma compress�o.
	if(bi->bmpCompression != 0 )
	{
		//fail
	}
	
	
	//
	// Draw !
	//
	
//DrawBMP:	

	left = x;    
	top = y; 
	
	
	//bottom = top + height;
	bottom = (top + bi->bmpHeight );

		
	
	// In�cio da �rea de dados do BMP.
	
	//#importante:
	//a base � diferente para os tipos.
	 

	switch(bi->bmpBitCount)
    {
		//case 1:
		//    base = (0x36 + 0x40);
		//    break;
		    
		//case 2:
		//    base = (0x36 + 0x40);
		//    break;
			
		case 4:
		    //4bytes pra cada cor, 16 cores, 64bytes.
		    base = (0x36 + 0x40);
		    break; 
			
		case 8:
		    //4bytes pra cada cor, 256 cores, 1024bytes.
		    base = (0x36 + 0x400);
		    break; 
			
		default:
		    base = 0x36;
			break;
	};	
	

//1 - 1 bpp (Mono)
//4 - 4 bpp (Indexed)
//8 - 8 bpp (Indexed) bbgggrrr
//16565 - 16 bpp (5:6:5, RGB Hi color)
//16    - 16 bpp (5:5:5:1, RGB Hi color)
//160   - 16 bpp (5:5:5:1, RGBA Hi color)
//24 - 24 bpp (True color)
//32 - 32 bpp (True color, RGB)
//320 - 32 bpp (True color, RGBA)	

	
	for( i=0; i < bi->bmpHeight; i++ )	
	{		
		for( j=0; j < bi->bmpWidth; j++ )	
		{	
	        // 16 cores
            // Um pixel por nibble.
	        if(bi->bmpBitCount == 4 )
	        {				
				offset = base;
							    
				palette_index[0] = bmp[offset];
												
                //segundo nibble.
				if( nibble_count_16colors == 2222 )
				{
					palette_index[0] = ( palette_index[0] & 0x0F);  
					color = (unsigned long) palette[  palette_index[0]  ];
					
					nibble_count_16colors = 0;
					base = base + 1;
					
				//primeiro nibble.	
				}else{

			        palette_index[0] =  ( (  palette_index[0] >> 4 ) & 0x0F);
					color = (unsigned long) palette[  palette_index[0] ];
				    
					nibble_count_16colors = 2222;
					base = base;
				}
	        };	

			// 256 cores
			// Pr�ximo pixel para 8bpp
	        if( bi->bmpBitCount == 8 )
	        {   
				offset = base;
				color = (unsigned long) palette[  bmp[offset] ];
				
				base = base + 1;     
	        };			
			
			// 16bpp high color BMP
			// Pr�ximo pixel para 16bpp
			// apenas 565 por enquanto.  
	        if(bi->bmpBitCount == 16 )
	        {

			    //565
                //if(565 )
                //{
				    offset = base;					
					
				    //A
			        c[0] = 0;	

			        //b				
			        c[1] = bmp[offset];
			        c[1] = (c[1] & 0xF8);  // '1111 1000' 0000 0000  
				
				    //g
			        c2[0] = bmp[offset];
			        c2[0] = c2[0] &  0x07;    // '0000 0111' 0000 0000 
			        c2[1] = bmp[offset+1];
			        c2[1] = c2[1] &  0xE0;    //  0000 0000 '1110 0000' 
					c[2] = ( c2[0] | c2[1]  );
					
			        //r
			        c[3] = bmp[offset+1];
			        c[3] = c[3] & 0x1F;     // 0000 0000 '0001 1111' 										
		        
				    base = base + 2;    
				//};					
				
	        };			
			

			// Pr�ximo pixel para 24bpp
	        if(bi->bmpBitCount == 24 )
	        {
				offset = base;
			    
			    c[0] = 0; //A					
				
				c[1] = bmp[offset];
			    
			    offset = base+1;
			    c[2] = bmp[offset];
			
			    offset = base+2;
			    c[3] = bmp[offset];
										
		        base = base + 3;    
	        };
			
			
			// Pr�ximo pixel para 32bpp
	        if(bi->bmpBitCount == 32 )
	        {
			    
			    
				//A
				//offset = base+3;
			    c[0] = 0;				
				
				offset = base;
			    c[1] = bmp[offset];
			
			    offset = base+1;
			    c[2] = bmp[offset];
			
			    offset = base+2;
			    c[3] = bmp[offset];
				
		        base = base + 4;    
	        };
			
			my_buffer_put_pixel( (unsigned long) color, 
			                     (unsigned long) left, 
								 (unsigned long) bottom, 
								 0 );
			
			// Pr�ximo pixel.
			left++; 
		};
		
		
		// Vamos para a linha anterior.
		bottom = bottom-1;
		
		// Reiniciamos o x.
		left = x;    
	};	
	
	// ## test palette 
	//int p;
	
	//if(bi->bmpBitCount == 8 )
	//{
	//    printf("\n");
	//    for( p=0; p<16; ++p )
	//    {
	//	   printf("%x\n",palette[p]);
	//    }
	//    printf("\n");
	//};
	
fail:	
    //printf("fail");	
done:	
	//Debug
	//printf("w={%d} h={%d}\n", bi->bmpWidth, bi->bmpHeight );
	return (int) 0;
};


/*
int bmpInit();
int bmpInit()
{
	int Status = 0;
	//...
    return (int) Status;	
};
*/

//
// End.
//

