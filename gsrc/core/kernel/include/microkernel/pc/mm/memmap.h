/*
 *  File: mm\memmap.h 
 *
 * Descri��o: 
 *     Memory map support.
 *     Refer�ncia para localiza��o de VBR, FAT, ROOT, DATAAREA ...
 *
 * ATEN��O: 
 *     kernel base = 0x00100000pa
 *   
 * #bugbug: 
 *     Ao usar 0x00020000, 0x00060000, 0x00080000 a mem�ria de video 
 * � afetada.
 *     Tem mapeamento sobreposto nessa regi�o.
 *
 * History:
 *     2015 - Created by Fred Nora.
 */
 
 
 /*
  #importante:
  Basicamente existem 2 mapeamentos de mem�ria:
  
  + Um usado pelo kernel e seus m�dulos, mesmo que esses 
    m�dulos estejam em user mode ou em outro processo e 
	
  + Outro mapeamento padr�o para todos os processos.
  
  + ?? N�o sei quanto ao mapeamento para servidores em user mode 
    ou para bibliotecas em user mode
  ...
 
  */

/*    
  Memory map. 
	
 " Cada processo tem seu diret�rio de p�ginas e uma configura��o 
   de 4GB de mem�ria virtual. "

   Mapeamento de mem�ria para o processo kernel.
  
 " Note que no in�cio da mem�ria virtual temos 4MB em kernel mode. 
   Isso � usado pelo Kernel para acessar os endere�os mais baixos,
   inclusive a mem�ria de video VGA, mas a mem�ria de v�deo VGA 
   foi novamente mapeada no endere�o virtual 0x800000. "
   
			 +====================================+
             |           Kernel land              |
	         |                                    | 
             |  Stack = 0xC02F7FF0 ~ 0xC02FFFF0   | Total 32KB. 
	         |  Heap  = 0xC0100000 ~ 0xC02F7FF0   |	Total 2015 KB.
             |                                    | 			 
			 |  Kernel Entry point = 0xC0001000   | Entry point do kernel.
	         |  Kernel Base = 0xC0000000          |	In�cio da imagem do 
             |                                    |	processo kernel. 		 
	C0000000 |        ( Kernel Mode access )      |	 	   
	         +------------------------------------+
			 
*/

/*   

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
             |  Stack = 0xC02F7FF0 ~ 0xC02FFFF0   | Total 32KB. 
	         |  Heap  = 0xC0100000 ~ 0xC02F7FF0   |	Total 2015 KB.
             |                                    | 			 
			 |  Kernel Entry point = 0xC0001000   | Entry point do kernel.
	         |  Kernel Base = 0xC0000000          |	In�cio da imagem do 
             |                                    |	processo kernel. 		 
	C0000000 |        ( Kernel Mode access )      |	 	   
	         +------------------------------------+
             |           User Land                |	 
	         |                                    |
             |                                    | @todo  In�cio da pilha em user mode do proesso.
	         |                                    | @todo: In�cio do heap em user mode do processo.
             |                                    | ### Por enquando cada processo tem sua pr�pria
             |                                    |     pilha e heap no fim da imagem do processo.   			 
             |                                    | ??
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

#define MEM_BASE  KERNEL_GRAPHIC_MODE_BASE

#define VM_BASE      0x000B8000    //F�sico.

#define KERNEL_BASE  0x00100000    //1MB f�sico.
#define USER_BASE    0x00400000    //4MB f�sico.

//Endere�o virtual padr�o para o Linear Frame Buffer. (LFB).
#define DEFAULT_LFB_VIRTUALADDRESS  0xC0400000 

//Endere�o virtual padr�o para o BackBuffer. (buffer1)
#define DEFAULT_BACKBUFFER_VIRTUALADDRESS 0xC0800000


//MEDIDAS.
//0x01000000 (16Mb)	
//0x02000000 (32Mb)
//0x04000000 (64Mb)	
//0x08000000 (128Mb)
//...	

//Onde come�a a �rea onde alocaremos frames para os processos.
//f�sico ??
//Esse in�cio ser� o mesmo tanto para sistemas com 32mb quanto para sistemas maiores.

//definindo o in�cio do paged pool para sustemas pequenos. Com 32MB.

//
// SMALL SYSTEM (M�NIMO 32MB)
#define SMALLSYSTEM_KERNELADDRESS    0
#define SMALLSYSTEM_KERNELBASE       KERNEL_BASE
#define SMALLSYSTEM_USERBASE         USER_BASE
#define SMALLSYSTEM_VGA              VM_BASE
//#define SMALLSYSTEM_FRONTBUFFER      ?? 
#define SMALLSYSTEM_BACKBUFFER       (0x01000000 - 0x400000)   
#define SMALLSYSTEM_PAGEDPOLL_START  0x01000000   //Come�a em 16MB.
//...

//
// MEDIUM SYSTEM  (M�NIMO 64MB) 
#define MEDIUMSYSTEM_KERNELADDRESS    0
#define MEDIUMSYSTEM_KERNELBASE       KERNEL_BASE
#define MEDIUMSYSTEM_USERBASE         USER_BASE
#define MEDIUMSYSTEM_VGA              VM_BASE
//#define MEDIUMSYSTEM_FRONTBUFFER      ?? 
#define MEDIUMSYSTEM_BACKBUFFER       0x01000000   
#define MEDIUMSYSTEM_PAGEDPOLL_START  0x02000000   //Come�a em 32MB.
//...

//
// LARGE SYSTEM (M�NIMO 128MB)
#define LARGESYSTEM_KERNELADDRESS    0
#define LARGESYSTEM_KERNELBASE       KERNEL_BASE
#define LARGESYSTEM_USERBASE         USER_BASE
#define LARGESYSTEM_VGA              VM_BASE
//#define LARGESYSTEM_FRONTBUFFER      ?? 
#define LARGESYSTEM_BACKBUFFER       0x01000000   
#define LARGESYSTEM_PAGEDPOLL_START  0x04000000   //Come�a em 64MB.
//...


//
// Endere�os f�sicos de algumas pagetables.
//

//
// 0x0008F000 Tabela para mapear a parte mais baixa da mem�ria f�sica. Come�a em 0.
// 0x0008E000 Tabela para mapear a mem�ria usada pela imagem do kernel. Come�a em 0x100000.
// 0x0008D000 Tabela para mapear uma �rea em user mode onde rodam c�digos. Come�a em 0x400000.
// 0x0008C000 Tabela para mapear a vga. Come�a em 0xb8000.
// 0x0008B000 Tabela para mapear o frontbuffer, O come�o � passado pelo Boot.
// 0x0008A000 Tabela para mapear o backbuffer, o come�o � em (0x01000000 - 0x400000) no small system.
// // 0x00089000 Tabela de p�ginas para o pagedpool.
// //...
//?? Obs: N�o sei o quanto podemos ir escolhendo endere�os nessa regi�o sem sobrepor o boot loader.
//

// System page tables:
// Essas s�o as pagetables mais importantes,
// por isso ficar�o aqui isoladas.
// J� as outras pagetables ficar�o concatenadas 
// numa lista grande em um outro lugar de f�cil acesso. 
#define PAGETABLE_KERNELAREA   0x0008F000
#define PAGETABLE_KERNELBASE   0x0008E000
#define PAGETABLE_USERBASE     0x0008D000
#define PAGETABLE_VGA          0x0008C000
#define PAGETABLE_FRONTBUFFER  0x0008B000
#define PAGETABLE_BACKBUFFER   0x0008A000
#define PAGETABLE_PAGEDPOOL    0x00089000  //?? isso � um teste ... Cuidado. #bugbug
//?? Obs: N�o sei o quanto podemos ir escolhendo endere�os nessa regi�o sem sobrepor o boot loader.

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

