/*
 *  File: hal\memmap.h 
 *
 * Descri��o: 
 *     Memory map support.
 *     Refer�ncia para localiza��o de VBR, FAT, ROOT, DATAAREA ...
 *
 * ATEN��O: 
 *     kernel base = 0x00100000
 *   
 * bug bug: 
 *     Ao usar 0x00020000, 0x00060000, 0x00080000 
 *     a mem�ria de video est� sendo afetada 
 *     quando usa esses endere�os.
 *
 * Vers�o 1.0. 2015, 2016.
 */
 

/*
    Memory map. (Kernel directory)
	
 " Cada processo tem seu diret�rio e uma configura��o de 4GB de 
   mem�ria virtual. Essa � mem�ria virtual do processo Kernel. "

 " Note que no in�cio da mem�ria virtual temos 4MB em kernel mode, 
   isso � usado pelo Kernel para acessar os endere�os mais baixos,
   inclusive a mem�ria de video VGA, mas a mem�ria de v�deo VGA 
   foi novamente mapeada em no endere�o virtual 0x800000. "  

  Mapeamento padr�o de mem�ria virtual para todos os processos:
    ============================================================	


		     +------------------------------------+
	FFFFFFFF |             FIM                    |
		     +------------------------------------+
		     +------------------------------------+
		     +------------------------------------+ 
		     +------------------------------------+
	         |         User Mode access           | @todo: Mudar de lugar.  
		     |                                    |        Seder espa�o para lfb.  
		     |                                    |
    C0800000 |           Back Buffer              |  			 
		     +------------------------------------+	
	         |        User Mode access            |	 Mem�ria da placa de v�deo.
		     |             (4MB)                  |  @todo Ampliar (PRECISA SER MAIOR)
			 |             ...                    |  obs: Tamanho do monitor.
	C0400000 |             LFB                    |
		     +------------------------------------+
			 +====================================+
             |           Kernel land              |
	         |                                    | 
             |  Stack = 0xC0300000 ~ 0xC03FFFF0   | Pilha do Kernel. 
	         |  Heap  = 0xC0100000 ~ 0xC02FFFF0   |	Obs: O Heap do kernel precisa ser maior.
             |                                    | 			 
			 |  Kernel Entry point = 0xC0001000   |      Entry point do kernel.
	         |  Kernel Base = 0xC0000000          |	     In�cio da im�gem do processo kernel. 
	C0000000 |         Kernel Mode access         |	 	   
	         +------------------------------------+
             |           User Land                |	 
	         |                                    |
             |  Stack = 0x40300000 ~ 0x403FFFF0   | @todo  In�cio da pilha em user mode do proesso.
	         |  Heap  = 0x40100000 ~ 0x402FFFF0   | @todo: In�cio do heap em user mode do processo.
             |                                    | ### Por enquando cada processo tem sua pr�pria
             |                                    |     pilha e heap no fim da imagem do processo.   			 
             | 40000000 = Process Base.           | ??
			 |                                    |
			 | 00041000 = Process entry point     | Entrypoint da imagem.
			 | 00040000 = Process image base      | Onde se carrega uma imagem de processo.
             |                                    |  			 
             | 00000000 = Dinamic Library Base    |
			 | 00000000 = Dinamic Library image   |
			 |                                    |
             |              ...                   | @todo: ampliar heap.   			 
	00000000 |       User Mode access             |	 
			 +====================================+			 
			    
  

    ***	 

			 
    ***
	
	Obs: O layout da mem�ria virtual est� em fase de desenvolvimento.	   
	
*/ 
 

//
// Refer�ncias para mem�ria de video.
//
 
#define KERNEL_TEXT_MODE_BASE     0x000B8000
#define KERNEL_GRAPHIC_MODE_BASE  0x000A0000

#define MEM_BASE    KERNEL_GRAPHIC_MODE_BASE

#define KERNEL_BASE  0x00100000    //1MB f�sico.
#define VM_BASE      0x000B8000    //F�sico.
#define USER_BASE    0x00400000    //4MB f�sico.


//Endere�o virtual padr�o para o Linear Frame Buffer. (LFB).
#define DEFAULT_LFB_VIRTUALADDRESS  0xC0400000 

//Endere�o virtual padr�o para o BackBuffer. (buffer1)
#define DEFAULT_BACKBUFFER_VIRTUALADDRESS 0xC0800000

//
// @todo: Com as informa��es de backbuffer e lfb pode-se
//        fazer rotinas de pintura r�pida de partes da tela, 
//        e n�o a tela inteira.
// 

/*
unsigned long memmapBaseMemoryStart;
unsigned long memmapBaseMemoryEnd;
unsigned long memmapOtherMemoryStart;
unsigned long memmapOtherMemoryEnd;
unsigned long memmapExtendedMemoryStart;
unsigned long memmapExtendedMemoryEnd;
*/

//
//fim.
//

