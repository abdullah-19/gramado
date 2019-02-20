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

