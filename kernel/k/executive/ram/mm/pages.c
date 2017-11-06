/*
 * File: pages.c
 *
 * Descri��o:
 *     Faz a configura��o da pagina��o de mem�ria e oferece rotinas de
 * suporte ao mapeamento de mem�ria f�sica.
 *     Faz parte do modulo /mm da classe ram.
 *
 * *Importante:
 *     Essa rotina pertence ao m�dulo de gerenciamento de mem�ria. N�o possui
 * informa��es sobre processos. Qualquer informa��o sobre processos deve ser 
 * conseguida atrav�s de invoca��o de m�todos pertencentes ao m�dulo 
 * /microkernel.
 *
 * @todo: 
 *    IMPORTANTE:
 *    +FAZER GERENCIAMENTO DA MEM�RIA F�SICA. 
 *    +DIVIDIR A MEM�RIA F�SICA EM PARTI��ES DE 4MB E 
 *    +DIVIDIR CADA PARTI��O EM FRAMES DE 4KB, O QUE D� 1024 FRAMES POR 
 * PARTI��O.
 *
 * Obs: 
 *     Chamaremos de 'framepool' o conjunto de 1024 frames.
 *     Poderemos mapear um framepool inteiro se preciso. 
 *
 * @todo:
 * *Importante:
 * Obs: 
 *     Os processos Idle, Shell e Taskman est�o usando o diret�rio de p�ginas 
 * do processo Kernel. � preciso criar um diret�rio para cada processo e 
 * criar uma rotina de automa��o na cria��o de diret�rios de p�ginas.
 *
 * @todo: 
 *     Criar rotinas para ver o conte�do da lista de diret�rios de p�ginas.
 *     ?? Cada diret�rio deve ter uma estrutura, cujo ponteiro vai pra dentro 
 * da lista.
 *     ?? A estrutura mostrar� informa��es sobre o diret�rio de p�ginas.
 *
 * @todo: 
 *     Criar rotinas que manipulem as estruturas de diret�rio de p�ginas e de 
 * pagetables. 
 *
 * Obs: Todas as pagetables podem ficar em sequ�ncia em uma mesma regi�o do endere�o
 * l�gico do processo kernel.
 *
 * In this file:
 * =============
 *     +CreatePageDirectory
 *     +CreatePageTable
 *     +SetCR3
 *     +SetUpPaging
 *     +
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2016 - Revision.
 *     ...
 */


//
// Algumas �reas de mem�ria:
// =========================
//
// + kernel area    = 1024 pageframes (4MB).
// + kernel image   = 1024 pageframes (4MB).
// + user mode area = 1024 pageframes (4MB).
// + vga            = 1024 pageframes (4MB).
//     (Obs: Isso transpassa o real tamanho da vga).
// + lfb (frontbuffer) = 1024 pageframes (4MB).
//     (Obs: Isso � muito pouco, pois uma placa de v�deo tem mais mem�ria
// que isso).
//      @todo: 
//      LFB needs to be bigger. (Ex: Four 8GB graphic cards). But the driver 
// needs to do all the work.
// + backbuffer     = 1024 pageframes (4MB). 
//     (Obs: Isso realmente � pouco, no backbuffer deve caber uma imagem 
// grande, que ser� dividida em v�rios monitores).
//


/*
 O Boot Loader criou alguns diret�rios nesses endere�os f�sicos, parece 
 conveniente usar esses endere�os.
 @todo: Rever os endere�os usados, tanto no kernel, quanto no Boot Loader.

	unsigned long *page_directory         = (unsigned long *) 0x01F00000;
	unsigned long *idle_page_directory    = (unsigned long *) 0x01E00000;
	unsigned long *shell_page_directory   = (unsigned long *) 0x01D00000;
	unsigned long *taskman_page_directory = (unsigned long *) 0x01C00000;
	unsigned long *x_page_directory       = (unsigned long *) 0x01B00000;
	unsigned long *xx_page_directory      = (unsigned long *) 0x01A00000;
	unsigned long *xxx_page_directory     = (unsigned long *) 0x01900000;

 */


#include <kernel.h>


//
// Vari�veis passadas pelo Boot Loader.
//

//
// Obs: Teremos que rever os argumentos passados pelo Boot Loader ao Kernel 
// Pois desejamos tornar o Kernel compat�vel com GRUB.
//

extern unsigned long SavedBootBlock;    //Par�mtros passados pelo Boot Loader.
extern unsigned long SavedLFB;          //LFB address.
extern unsigned long SavedX;            //Screen width.
extern unsigned long SavedY;            //Screen height.
extern unsigned long SavedBPP;          //Bits per pixel.
//...


//
// Fun��es importadas.
//

extern void set_page_dir();
//...


//
// Obs: Deixar aqui os endere�os das p�ginas.
//


//
// Pagetables:
// ==========
//     Endere�os f�sicos de algumas pagetales.
//

#define KM1_PAGETABLE     0x8F000  // Pagetable para o kernel mode stuff.
#define KM2_PAGETABLE     0x8E000  // Pagetable para 'O Kernel'. A 'imagem'.
#define UM_PAGETABLE      0x8D000  // Pagetable para o aplicativos em user mode.
#define VGA_PAGETABLE     0x8C000  // Pagetable para o VGA em user mode.
#define LFB_PAGETABLE     0x8B000  // LFB.
#define BUFFER_PAGETABLE  0x8A000  // BackBuffer.
//...


/*
 ?? Para qual tipo ??
enum PAGE_PTE_FLAGS {
 
	I86_PTE_PRESENT			=	1,		    // 0000000000000000000000000000001
	I86_PTE_WRITABLE		=	2,		    // 0000000000000000000000000000010
	I86_PTE_USER			=	4,		    // 0000000000000000000000000000100
	I86_PTE_WRITETHOUGH		=	8,		    // 0000000000000000000000000001000
	I86_PTE_NOT_CACHEABLE	=	0x10,		// 0000000000000000000000000010000
	I86_PTE_ACCESSED		=	0x20,		// 0000000000000000000000000100000
	I86_PTE_DIRTY			=	0x40,		// 0000000000000000000000001000000
	I86_PTE_PAT			   =	0x80,		// 0000000000000000000000010000000
	I86_PTE_CPU_GLOBAL		=	0x100,		// 0000000000000000000000100000000
	I86_PTE_LV4_GLOBAL		=	0x200,		// 0000000000000000000001000000000
   	I86_PTE_FRAME			=	0x7FFFF000 	// 1111111111111111111000000000000
};
*/


/*
void map_page(void * physaddr, void * virtualaddr, unsigned int flags)
{
    // Make sure that both addresses are page-aligned.
 
    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
    unsigned long * pd = (unsigned long *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.
 
    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?
 
    pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | 0x01; // Present
 
    // Now you need to flush the entry in the TLB
    // or you might not notice the change.
}
*/


/*
void * get_physaddr(void * virtualaddr);
void * get_physaddr(void * virtualaddr)
{
    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
    unsigned long * pd = (unsigned long *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
 
    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
 
    return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}
*/


/*
static inline void __native_flush_tlb_single(unsigned long addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}
*/


//Usar alguma rotina de hal_ pra isso;
//extern unsigned long _get_page_dir();


/*
 * CreatePageDirectory:
 *     Cria um page directory para um processo.
 *     Obs:
 *     + O endere�o precisa ser alocado antes.
 *     + Precisa ser um endere�o f�sico.
 *     +...
 *
 * @todo: Mudar para pagesCreatePageDirectory(.)
 */
//void *pagesCreatePageDirectory(unsigned long address) 
void *CreatePageDirectory(unsigned long directory_address)
{	
	int i;
	unsigned long *page_directory = (unsigned long *) directory_address;    //Diret�rio.


   /*
    * @todo: 
	*     Na verdade os diret�rios de p�gina e as page tables devem ser 
	* criados em sequ�ncia num local pre-definido ou deve-se alocar mem�ria 
	* para cri�-lo. 
	*/	
	
	//Limits.
	if(directory_address == 0){
		return NULL;
	};


	//
	//@todo:
	//    Save on a list of directories.
	//    Registra na lista de diret�rios.
	//
	//pagedirectoryList[PAGEDIRECTORY_COUNT_MAX]; 
	
	
	//...
	
done:	
	return (void*) directory_address;
};



/*
 * CreatePageTable:
 *     Cria uma page table em um diret�rio.
 *     Obs:
 *     + O endere�o precisa ser alocado antes.
 *     + Precisa ser um endere�o f�sico.
 *     + O diret�rio precisa ser um diret�rio v�lido.
 *
 * Argumentos:
 *     directory_address - 
 *     offset -
 *     page_address -
 *
 * @todo: Mudar para pagesCreatePageTable(...)
 */
void *CreatePageTable( unsigned long directory_address, 
                      int offset, 
					  unsigned long page_address )
{
	int i;
	unsigned long *page_directory = (unsigned long *) directory_address;    //Diret�rio.
	unsigned long *page_table = (unsigned long *) page_address;
	
	//Limits.
	if(directory_address == 0){
		return NULL;
	};

	//Limits.
	if(offset < 0){
		return;
	};


	page_directory[offset] = (unsigned long) page_address;


	//
	// @todo: 
	//     Registrar na lista de p�ginas.  
	//     Salvar estruturas.
	//
	//unsigned long pagetableList[PAGETABLE_COUNT_MAX]; 

done:
    return (void*) page_address;
};


/*
 * SetCR3:
 *     Configurando cr3.
 *     Obs: Precisa ser endere�o f�sico.
 *     Obs: Chamamos uma rotina em assembly.
 *     @todo: Mudar para pagesSetCR3(.)
 */
//void pagesSetCR3(unsigned long address) 
void SetCR3(unsigned long address)
{
	if(address == 0){
		return;
	};
	asm volatile("\n" :: "a"(address) );
	set_page_dir();
	return;
};


/*
 * SetUpPaging:
 *     Configura o diret�rio de p�ginas do processo Kernel e algumas tabelas 
 * de p�ginas.
 *
 * Obs: 
 *     Na hora em que um processo � criado deve-se criar seu diret�rio de 
 * p�ginas e as tabelas de p�ginas usadas por ele, de acordo com o tamanho 
 * do processo.
 *
 * Diret�rio:
 *     page_directory = 0x0009C000. (Endere�o f�sico).#kernel
 *
 * Obs:
 *     Esse diret�rio criado ser� usado pelo processo Kernel e tamb�m por 
 * outros processos tamb�m durante essa fase de constru��o do sistema. 
 * Depois cada processo ter� seu pr�prio diret�rio de p�ginas. Isso est� em 
 * fase de implementa��o. O ideal � um diret�rio por processo.
 *     Toda vez que o Kernel iniciar a execu��o de um processo ele deve 
 * carregar o endere�o do diret�rio do processo no registrador de controle CR3.
 * 
 * @todo: 
 *     Por enquanto s� um diret�rio foi criado.
 *     
 * @tod:
 *     o Mudar para pagesSetUpPaging.
 *
 * @TODO: AS COISAS EST�O MEIO BAGUN�ADAS AQUI. A INTEN��O � QUE 
 * A PARTE BAIXA DA MEM�RIA VIRTURAL DO PROCESSO PERTEN�A AO PROCESSO 
 * E A PARTE ALTA DA MEM�RIA VIRTUAL DO PROCESSO PERTEN�A AO KERNEL.
 * QUANTO A MEM�RIA F�SICA, DESEJAMOS QUE APENAS O KERNEL ACESSE A 
 * PARTE BAIXA DA MEM�RIA F�SICA, OS PROGRAMAS EM USER MODE MANIPULAR�O
 * APENAS A MEM�RIA QUE LHES FOR CONCEDIDA.
 *
 * Hist�rico:
 *     2015 - Essa fun��o foi criada por Fred Nora.
 *     2016 - Revis�o.
 *     ...
 */
//int pagesSetUpPaging() 
int SetUpPaging()
{
    unsigned int i;
	int Index;

    //
	// (phase 1) Endere�os da mem�ria f�sicas acess�veis em Kernel Mode.
    //	

	// Kernel process.
	unsigned long kernel_address = 0;        //para os 4 primeiros mega da memoria fisica     
	unsigned long kernel_base = KERNEL_BASE; //0x00100000 //para o kernnel que come�a no primeiro mega

    //
	// (phase 2) Endere�os da mem�ria f�sicas acess�veis em User Mode.
    //


	// User, VGA, VESA LFB, BUFFER ...
	unsigned long user_address = USER_BASE;                   //0x00400000; 	
	unsigned long vga_address  = VM_BASE;                     //0x000B8000; 
    unsigned long lfb_address  = (unsigned long) SavedLFB;    //g_lbf_pa, Foi passado pelo boot manager.
	unsigned long buff_address = (unsigned long) 0x01000000;  //16MB, #Provis�rio.
	// ...


	//
	// DIRECTORY:
	//     Diret�rio do processo Kernel. Esse diret�rio j� foi criado nesse 
	// endere�o f�sico pelo Boot Loader. Aqui o kernel apenas reconfigura, 
	// utilizando a mesma localiza�ao. KERNEL_PAGEDIRECTORY.
	//
	unsigned long *page_directory         = (unsigned long *) KERNEL_PAGEDIRECTORY;   //0x0009C000    
	//unsigned long *idle_page_directory    = (unsigned long *) IDLE_PAGEDIRECTORY;     //0x01E00000    
	//unsigned long *shell_page_directory   = (unsigned long *) SHELL_PAGEDIRECTORY;    //0x01D00000 
	//unsigned long *taskman_page_directory = (unsigned long *) TASKMAN_PAGEDIRECTORY;  //0x01C00000   	
    //...
	
	//
	// O que temos logo abaixo s�o pequenas parti��es de mem�ria f�sica.
	// cada parti��o tem 1024 unsigned longs. o que d� 4kb cada. 
	//
	//
	
	//
	// TABLES: 
	//     Tabelas de p�ginas para o diret�rio do processo Kernel. Essas 
	// tabelas j� foram criadas nesses endere�os f�sicos pelo Boot Loader. 
	// Aqui o Kernel apenas reconfigura utilizando as mesmas localiza��es.
	//
	
	//
	// Poder�amos alocar mem�ria para as page tables ??
	// Sim, mas precisa ser um mecanismo que devolva o endere�o f�sico 
	// de onde foi alocado mem�ria para a page table.
	// Na verdade deve haver uma �rea de mem�ria reservada para a aloca��o 
	// de page tables. Todas as que ser�o criadas ocupar�o muito espa�o.
	//

	//kernel mode. (Endere�os).
	unsigned long *km_page_table   = (unsigned long *) 0x0008F000;  //KM1_PAGETABLE.

	//kernel mode. (O kernel).
    unsigned long *km2_page_table  = (unsigned long *) 0x0008E000;  //KM2_PAGETABLE.

	//user mode.
	unsigned long *um_page_table   = (unsigned long *) 0x0008D000;  //UM_PAGETABLE.

	//user mode. (vga).
	unsigned long *vga_page_table  = (unsigned long *) 0x0008C000;  //VGA_PAGETABLE.

	//user mode. (LFB).
	unsigned long *lfb_page_table  = (unsigned long *) 0x0008B000;  //LFB_PAGETABLE.

	//user mode. (buffer).
    unsigned long *buff_page_table = (unsigned long *) 0x0008A000;  //BUFFER_PAGETABLE.

	//...


    //
	// Message. (verbose).
	printf("SetUpPaging: Initializing Pages..\n");


	//
	// Preenchendo todo o diret�rio de p�ginas do kernel com p�ginas n�o 
	// presentes. Usando um endere�o nulo de p�gina.
	//

	// Inicializando quatro diret�rios.
	for(i = 0; i < 1024; i++)
	{
		//kernel
		// Diret�rio de p�ginas do processo kernel.
		page_directory[i] = (unsigned long) 0 | 2;    //010 em bin�rio.
		
		//idle
		// @todo: Diret�rio de p�ginas do processo idle.
		//idle_page_directory[i] = (unsigned long) 0 | 2;    //010 em bin�rio.
		
		//shell
		// @todo: Diret�rio de p�ginas do processo shell.
		//shell_page_directory[i] = (unsigned long) 0 | 2;    //010 em bin�rio.

		//taskman
		// @todo: Diret�rio de p�ginas do processo taskman.
		//taskman_page_directory[i] = (unsigned long) 0 | 2;    //010 em bin�rio.
	};
	

	//===========================================================
	// kernel mode pages (0fis = 0virt)
	// kernel_address = 0.
	// Mapear os primeiros 4MB da mem�ria. (kernel mode). Preenchendo a tabela 
	// km_page_table. A entrada 0 do diret�rio refere-se aos primeiros 4 megas 
	// de endere�o virtual.
	//
    // Aqui estamos pegando uma parti��o de mem�ria f�sica de 4MB que come�a no 
	// in�cio da mem�ria RAM.
    // Obs: Essa page table d� acesso aos primeiros 4MB da mem�ria f�sica,
	// Isso inclu a �rea do kernel base que come�a no primeiro MB. Manipular
	// esse espa�o pode corromper o kernel base.
	//
	// A inten��o aqui � que o kernel base possa manipular as �reas baixas da 
	// mem�ria f�sica com facilidade. Por�m, para os outros processos, os endere�os 
	// l�gicos mais baixos n�o devem corresponder aos endere�os f�sicos mais baixos,
	// por seguran�a, apenas o kernel base deve ter acesso � essa �rea.
	// Para alguns processos especiais, algum tipo de permiss�o poder� ser concedida.
	//
    
	
	// Configurando uma pagetable.
	// a pagetable para os primeiros 4MB de mem�ria f�sica. 
	// kernel mode pages (0fis = 0virt)
	for(i = 0; i < 1024; i++)
    {
	    km_page_table[i] = (unsigned long) kernel_address | 3;     //011 bin�rio.
	    kernel_address   = (unsigned long) kernel_address + 4096;  //+ 4KB.
    };
    
	
	//kernel
	//Criando a entrada n�mero '0' do diret�rio de p�ginas do processo Kernel.
	//que apontar� para a pagetable que criamos.
    page_directory[0] = (unsigned long) &km_page_table[0];      //Salva no diret�rio o endere�o f�sico da tabela.
    page_directory[0] = (unsigned long) page_directory[0] | 3;  //Configurando os atributos.
    

	//idle
	//Criando a entrada n�mero '0' do diret�rio de p�ginas do processo idle.
	//que apontar� para a pagetable que criamos.
    //idle_page_directory[0] = (unsigned long) &km_page_table[0];      //Salva no diret�rio o endere�o f�sico da tabela.
    //idle_page_directory[0] = (unsigned long) idle_page_directory[0] | 3;  //Configurando os atributos.
	
	//shell
	//Criando a entrada n�mero '0' do diret�rio de p�ginas do processo shell.
	//que apontar� para a pagetable que criamos.
    //shell_page_directory[0] = (unsigned long) &km_page_table[0];      //Salva no diret�rio o endere�o f�sico da tabela.
    //shell_page_directory[0] = (unsigned long) shell_page_directory[0] | 3;  //Configurando os atributos.
	
	
	//taskman
	//Criando a entrada n�mero '0' do diret�rio de p�ginas do processo taskman.
	//que apontar� para a pagetable que criamos.
    //taskman_page_directory[0] = (unsigned long) &km_page_table[0];      //Salva no diret�rio o endere�o f�sico da tabela.
    //taskman_page_directory[0] = (unsigned long) taskman_page_directory[0] | 3;  //Configurando os atributos.	
	
	
	
	//===============================================
	// kernel mode pages (0x00100000fis = 0xC0000000virt)
	// kernel_base = 0x00100000 = KERNEL_BASE.
	// Mapear 4MB come�ando do primeiro mega. (kernel mode).
	// Preenchendo a tabela km2_page_table.
	//
    // Aqui estamos pegando uma parti��o de mem�ria f�sica de 4MB que come�a no
	// endere�o f�sico que carregamos a imagem do kernel.
    // s�o 4MB de mem�ria f�sica, come�ando do primeiro MB, onde o KERNEL.BIN 
    // foi carregado.	
    //	

	//Criando uma pagetable.
    //4MB de mem�ria f�sica, come�ando em 1MB.
    // kernel mode pages (0x00100000fis = 0xC0000000virt)	
    for(i = 0; i < 1024; i++)
    {
	    km2_page_table[i] = (unsigned long) kernel_base | 3;     //011 bin�rio.
	    kernel_base       = (unsigned long) kernel_base + 4096;  //+4KB.
    };
    
	
	//kernel
	// Criando a  entrada do diret�rio de p�ginas do processo kernel.
    page_directory[768] = (unsigned long) &km2_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    page_directory[768] = (unsigned long) page_directory[768] | 3;  //Configurando os atributos.

	
	//idle
	// Criando a  entrada do diret�ri de p�ginas do processo idle.
    //idle_page_directory[768] = (unsigned long) &km2_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    //idle_page_directory[768] = (unsigned long) idle_page_directory[768] | 3;  //Configurando os atributos.
	
	//shell
	// Criando a  entrada do diret�ri de p�ginas do processo shell.
    //shell_page_directory[768] = (unsigned long) &km2_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    //shell_page_directory[768] = (unsigned long) shell_page_directory[768] | 3;  //Configurando os atributos.
	
	
	//taskman
	// Criando a  entrada do diret�ri de p�ginas do processo taskman.
    //taskman_page_directory[768] = (unsigned long) &km2_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    //taskman_page_directory[768] = (unsigned long) taskman_page_directory[768] | 3;  //Configurando os atributos.
	
	

	//
    // Obs: Percebe-se que houve uma sobreposi��o. Os megas 0,1,2,3 para
	// kernel mode e os megas 1,2,3,4 para o kernel base.
	// Isso significa que o Kernel Base pode acessar o primeiro mega
	// da mem�ria f�sica, usando endere�o virtual igual ao endere�o f�sico.
	//
	
	//    ****    USER BASE    ****
	
	//===================================================================
	// user mode pages - (0x00400000fis = 0x00400000virt)
	// user_address = 0x00400000 = USER_BASE.
	// Mapear 4MB da mem�ria come�ando em 0x00400000fis. (user mode).
	//
	//
    // Aqui estamos pegando uma parti��o de mem�ria f�sica de 4MB que come�a no
	// endere�o f�sico 0x00400000, no quarto mega da mem�ria f�sica. 
    //
    // � nesse endere�o l�gico que ficar�o os processos em user mode.
	// Cada processo ter� um diret�rio de p�ginas, e nesse diret�rio de 
	// p�ginas ter� uma page table que atribuir� o endere�o l�gico de 0x400000
	// � algum endere�o f�sico alocado din�micamente para receber a imagem do processo.
	// Obs: Se o processo tiver mais que 4MB de tamanho, ent�o ser� preciso 
    // de mais de uma pagetable.
    //
    // Obs: 
	// HACK HACK. No momento, nessa pagetable do diret�rio do processo kernel, existem 
    // tr�s imagens carregadas nessa mesma �rea de mem�ria, compilados em endere�os 
    // absolutos diferentes. (Idle, Taskman e Shell). Mas queremos que todos os 
    // processos de usu�rio utilizem o mesmo endere�o l�gico. 0x400000.	
	//
	
	//Criando uma pagetable.
	//4MB de mem�ria f�sica, come�ando do querto mega.
	// user mode pages - (0x00400000fis = 0x00400000virt)
	//ser� usado pelo processo em user mode. Note as flags.(7).
	for(i = 0; i < 1024; i++)
    {
	    um_page_table[i] = (unsigned long) user_address | 7;     //7 decimal � igual a 111 bin�rio.
	    user_address     = (unsigned long) user_address + 4096;  //+4KB.
    };
	

    //kernel 	
    // Criando a entrada do diret�rio de p�ginas do processo kernel.
    page_directory[1] = (unsigned long) &um_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    page_directory[1] = (unsigned long) page_directory[1] | 7;  //Configurando os atributos.

    //idle 	
    // Criando a entrada do diret�rio de p�ginas do processo idle.
    //idle_page_directory[1] = (unsigned long) &um_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    //idle_page_directory[1] = (unsigned long) idle_page_directory[1] | 7;  //Configurando os atributos.


    //shell 	
    // Criando a entrada do diret�rio de p�ginas do processo shell.
    //shell_page_directory[1] = (unsigned long) &um_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    //shell_page_directory[1] = (unsigned long) shell_page_directory[1] | 7;  //Configurando os atributos.


    //taskman	
    // Criando a entrada do diret�rio de p�ginas do processo taskman.
    //taskman_page_directory[1] = (unsigned long) &um_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    //taskman_page_directory[1] = (unsigned long) taskman_page_directory[1] | 7;  //Configurando os atributos.
	
	
	//
    // Obs: Novamente aqui h� uma sobreposi��o. O primeiro mega
	// dessa �rea destinada � user mode, � o mesmo �ltimo mega da
    // �rea destinada ao Kernel Base. Isso significa uma �rea de mem�ria
    // compartilhada. O que est� no primeiro mega dessa �rea em user mode
	// tamb�m est� no �ltimo mega da �rea do kernel base.
    //	
	  
	  
    //==============================================================
	// user mode VGA pages - ( 0x000B8000fis = 0x00800000virt)
	// vga_address  = VM_BASE;   //0x000B8000;
	// Mapear 4MB da mem�ria come�ando em 0x000B8000fis. (user mode).
	// @todo: Aqui na verdade n�o precisa configurar 4 megas, 
	//        apenas o tamanho da mem�ria de v�deo presente em 0xb8000.
	//
    // Aqui estamos pegando uma parti��o de mem�ria f�sica de 4MB que come�a no
	// endere�o f�sico 0x000B8000.
    // @todo: bugbug: ESSA � CGA E N�O A VGA.
    //Mudar o nome para cga.	
    //	
    
    //Criando uma pagetable.
    //4MB de mem�ria f�sica, come�ando 0x000B8000fis.
    // user mode VGA pages - ( 0x000B8000fis = 0x00800000virt)
    // Podemos permitir que alguns processos em user mode acessem
    // essa �rea diretamente.	
    for(i = 0; i < 1024; i++)
    {
	    vga_page_table[i] = (unsigned long) vga_address | 7;     //7 decimal � igual a 111 bin�rio.
	    vga_address       = (unsigned long) vga_address + 4096;  //+4KB.
    };
	
    //kernel
	// Criando a entrada do diret�rio de p�ginas do processo kernel.
    page_directory[2] = (unsigned long) &vga_page_table[0];     //Salva no diret�rio o endere�o f�sico.
    page_directory[2] = (unsigned long) page_directory[2] | 7;  //Configurando os atributos.

	
    //idle
	// Criando a entrada do diret�rio de p�ginas do processo idle.
    //idle_page_directory[2] = (unsigned long) &vga_page_table[0];     //Salva no diret�rio o endere�o f�sico.
    //idle_page_directory[2] = (unsigned long) idle_page_directory[2] | 7;  //Configurando os atributos.

    //shell
	// Criando a entrada do diret�rio de p�ginas do processo shell.
    //shell_page_directory[2] = (unsigned long) &vga_page_table[0];     //Salva no diret�rio o endere�o f�sico.
    //shell_page_directory[2] = (unsigned long) shell_page_directory[2] | 7;  //Configurando os atributos.

    //taskman
	// Criando a entrada do diret�rio de p�ginas do processo taskman.
    //taskman_page_directory[2] = (unsigned long) &vga_page_table[0];     //Salva no diret�rio o endere�o f�sico.
    //taskman_page_directory[2] = (unsigned long) taskman_page_directory[2] | 7;  //Configurando os atributos.

	
	//
	// Obs: 4MB come�ando do endere�o f�sico 0x000B8000, s�o acess�veis
	// em user mode � partir do endere�o virtual 0x00800000virt.
	//
	
	
    //==================================================================
	// user mode LFB pages - (0x????????fis = 0xC0400000virt).
	// lfb_address  = SavedLFB = g_lbf_pa, Foi passado pelo boot manager.
	// Mapear 4MB da mem�ria f�sica come�ando no valor do endere�o f�sico 
	// do LFB que foi passado pelo Boot Manager.
	// O endere�o de mem�ria l�gica utilizada � 4MB � partir de 0xC0400000.
	//
    // Aqui estamos pegando uma parti��o de mem�ria f�sica de 4MB que come�a no
	// endere�o f�sico do LFB, de valor desconhecido. Foi configurado em modo real. 
    // pelo m�todo VESA.
    //	
	// @todo: LFB needs to be bigger. (Ex: Four 8GB graphic cards).
    //        But the driver needs to do all the work.
    //
	
	g_frontbuffer_buffer_va = (unsigned long) 0xC0400000;
	
	//Criando uma pagetable.
	//Os quatro primeiros MB da mem�ria de v�deo.
	//user mode LFB pages - (0x????????fis = 0xC0400000virt).
	//provavelmente o endere�o f�sico � 0xE0000000
    for(i = 0; i < 1024; i++)
    {
	    lfb_page_table[i] = (unsigned long) lfb_address | 7;     //7 decimal � igual a 111 bin�rio.
	    lfb_address       = (unsigned long) lfb_address + 4096;  //+4KB.
    };
    
	
	//kernel
	// Criando a entrada do diret�rio de p�ginas do processo kernel.
    page_directory[769] = (unsigned long) &lfb_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    page_directory[769] = (unsigned long) page_directory[769] | 7;  //Configurando os atributos.	


	//idle
	// Criando a entrada do diret�rio de p�ginas do processo idle.
    //idle_page_directory[769] = (unsigned long) &lfb_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    //idle_page_directory[769] = (unsigned long) idle_page_directory[769] | 7;  //Configurando os atributos.		
	

	//shell
	// Criando a entrada do diret�rio de p�ginas do processo shell.
    //shell_page_directory[769] = (unsigned long) &lfb_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    //shell_page_directory[769] = (unsigned long) shell_page_directory[769] | 7;  //Configurando os atributos.		

	//taskman
	// Criando a entrada do diret�rio de p�ginas do processo taskman.
    //taskman_page_directory[769] = (unsigned long) &lfb_page_table[0];       //Salva no diret�rio o endere�o f�sico.
    //taskman_page_directory[769] = (unsigned long) taskman_page_directory[769] | 7;  //Configurando os atributos.		
	
	
	
    //===============================================================
	// user mode BUFFER1 pages - (0x01000000fis = 0xC0800000virt).
	// ***BackBuffer: 
	//     � o buffer onde se pinta o que aparecer� na tela. O conte�do 
	// desse buffer � copiado no LFB da mem�ria de v�deo, (refresh_screen).
    // buff_address = 0x01000000 = 16MB, #Provis�rio.	
	// O endere�o de mem�ria l�gica utilizada � 4MB � partir de 0xC0800000.
	//
    // Aqui estamos pegando uma parti��o de mem�ria f�sica de 4MB que come�a no
	// endere�o f�sico 0x01000000, no decimo sexto mega da mem�ria f�sica. 
    //	

    // criando uma page table.
	//4MB de me�ria f�sica, come�ando em 16MB, que ser�o usados 
	//para backbuffer. Obs essa �rea deve ter o mesmo tamanho do frontbuffer.
	// user mode BUFFER1 pages - (0x01000000fis = 0xC0800000virt).
	for(i = 0; i < 1024; i++)
    {
	    buff_page_table[i] = (unsigned long) buff_address | 7;     //7 decimal � igual a 111 bin�rio.
	    buff_address       = (unsigned long) buff_address + 4096;  //+4KB.
    };
	
	//kernel
    // Criando a entrada do diret�rio de p�ginas do processo kernel.
    page_directory[770] = (unsigned long) &buff_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    page_directory[770] = (unsigned long) page_directory[770] | 7;  //Configurando os atributos.	


	//idle
    // Criando a entrada do diret�rio de p�ginas do processo idle.
    //idle_page_directory[770] = (unsigned long) &buff_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    //idle_page_directory[770] = (unsigned long) idle_page_directory[770] | 7;  //Configurando os atributos.	
	
	//shell
    // Criando a entrada do diret�rio de p�ginas do processo shell.
    //shell_page_directory[770] = (unsigned long) &buff_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    //shell_page_directory[770] = (unsigned long) shell_page_directory[770] | 7;  //Configurando os atributos.	
	
	//taskman
    // Criando a entrada do diret�rio de p�ginas do processo taskman.
    //taskman_page_directory[770] = (unsigned long) &buff_page_table[0];      //Salva no diret�rio o endere�o f�sico.
    //taskman_page_directory[770] = (unsigned long) taskman_page_directory[770] | 7;  //Configurando os atributos.	
	
	//
	// Obs: 4MB da mem�ria f�sica � partir do endere�o f�sico 0x01000000 (16MB)
    // s�o destinados ao back buffer. Obs: Isso � bem pouco, uma tela com alta 
	// resolu��o usa mais que isso.	
	//


	//
	// @todo:  
	// (sobre heaps para processos em user mode).
	// O que precisa ser feito no momento:
	// + Os processos em user mode precisam aloca��o din�mica de mem�ria, 
	// para isso ser� usado o heap do processo ou o heap do desktop ao qual o 
	// processo pertence.
	//
	//@todo:
	// *IMPORTANTE: 
	// (sobre heaps para gerenciamento de recursos gr�ficos).
	// + Os buffers de janela ser�o alocados no heap do processo em user mode 
	// que gerencia a cria��o de janelas, portanto esse processo tem que ter 
	// bastante heap dispon�vel. Talvez quem fa�a esse papel seja o pr�prio 
	// kernel base, a� quem precisa de bastante heap � o kernel base.
	// Talvez seja um m�dulo em kernel mode que gerencie as janelas.
	// Por enquanto � a camada superior do kernel base. Mas interfaces poder�o
	// chamar essa camada no kernel base e oferecerem servi�os de gerenciamento
	// de recursos gr�ficos, utilizando apenas as primitivas oferecidas pelo 
	// kernel base. Essas bibliotecas que oferecem recursos gr�ficos podem 
	// ser processos em kernel mode ou em user mode. Elas oferecer�o recursos 
	// bem elaborados e completos, chamando o kernel base apenas para 
	// as rotinas primitivas. Isso facilita a cria��o de recursos gr�ficos,
	// por�m prejudica o desempenho, por isso o kernel base tamb�m oferece 
	// seu conjunto de recursos gr�ficos mais elaborados, al�m das primitivas,
	// � claro.
	//


	//
	// @todo: 
	// Continuar: Mais p�ginas podem ser criadas manualmente agora.
	// Porem a inten��o � utilizar rotinas de automa��o da cria��o 
	// de paginas, pagetable e diret�rios.
	//


	//
	// @todo: 
	// At� agora tem uma sobreposi��o danada no mapeamento um mesmo 
	// endere�o f�sico de mem�ria � mapeado para v�rios endere�os virtuais. 
	// Isso n�o � proibido, � assim que se comaprtilha mem�ria. Na pr�tica
	// podemos acessar a mesma regi�o de mem�ria de v�rias maneira diferentes.
	// Mas devemos tomar cuidado, principalmente para n�o corrompermos o 
	// kernel base.
	// O acesso a mem�ria compartilhada ser� gerenciado pelos mecanismos
	// padr�o de comunica��o e compartilhamento. Sem�foros e mutexes ...
	//


	//
	// @todo: 
	// *IMPORTANTE.
	// O que queremos � utilizar uma lista de frames livres na hora
	// configurarmos o mapeamento. Queremos pegar um frame livre e
	// associarmos ele com uma PTE, (entrada na tabela de p�ginas).
	// O que est� faltando � o gerenciamento de mem�ria f�sica.
	// O gerenciamento de mem�ria f�sica � feito dividindo a mem�ria f�sica 
	// em parti��es, peda�os grandes de mem�ria. Tem um m�dulo que trata
	// de bancos, aspaces no kernel base.
	//
	
	
	//
	// @todo:
	// *SUPER IMPORTANTE.
	// para gerenciarmos a me�ria f�sica, precisamos saber o tamanho 
	// da mem�ria f�sica disponpivel. tem um m�dulo no kernel base 
	// que trata disso.
	// * Depois de alocarmos uma regi�o grande da mem�ria f�sica,
	// destinada para frames, ent�o criaremos a lista de frames livres.
	// que significar� uma quantidade de frames livres dentro da �rea 
	// destinadas � frames. N�o significa �rea toda a �rea livre
	// na mem�ria f�sica, mas apenas os frames livres dentro da regi�o 
	// destinada aos frames.
	//

	//
	// Debug:
	//     Mostrando os endere�os do diret�rio e das p�ginas.
	//     #verbose.
	//

	printf("Debug:\n");
	printf("Configurando CR3={%x} !\n", (unsigned long) &page_directory[0]);
	printf("Page={%x} !\n", (unsigned long) &km_page_table[0]);
	printf("Page={%x} !\n", (unsigned long) &km2_page_table[0]);
	printf("Page={%x} !\n", (unsigned long) &um_page_table[0]);
	printf("Page={%x} !\n", (unsigned long) &vga_page_table[0]);
	printf("Page={%x} !\n", (unsigned long) &lfb_page_table[0]);
	printf("Page={%x} !\n", (unsigned long) &buff_page_table[0]);
	//refresh_screen();
	//while(1){};
	
	//
	// Obs: Podemos reaproveitas pagetables em diferentes processos.
	//

	//
	// CR3:
	// Salvando o endere�o do diret�rio do processo Kernel no CR3. O diret�rio 
	// do processo Kernel est� agora dentro de uma �rea protegida. Est� em 
	// kernel mode.  
	// Obs: Ser� rotineiro configurar isso cada vez que um processo for 
	// executar ou for criado.
	// Obs: Quando se troca o valor do cr3, a TLB � atualizada pra todas 
	// as entradas. (?? autom�tico)
    //

	SetCR3( (unsigned long) &page_directory[0] );

	//Debug:
	//refresh_screen();
	//while(1){}


	//
    // LISTAS:
	//    Configurando a lista de diret�rios e 
	//    a lista de tabelas de p�ginas.
	//    Salvando na lista o endere�o f�sico dos
	// diret�rios e das tabelas de p�ginas.
	//


	//Inicializar a lista de diret�rios de p�ginas.
	for(Index = 0; Index < PAGEDIRECTORY_COUNT_MAX; Index++){
	    pagedirectoryList[Index] = (unsigned long) 0;
	};

	//O primeiro diret�rio da lista � o diret�rio do kernel.
	pagedirectoryList[0] = (unsigned long) &page_directory[0];          //kernel.
	//pagedirectoryList[1] = (unsigned long) &idle_page_directory[0];     //idle.
	//pagedirectoryList[2] = (unsigned long) &shell_page_directory[0];    //shell.
	//pagedirectoryList[3] = (unsigned long) &taskman_page_directory[0];  //taskman.	
    //pagedirectoryList[4] = (unsigned long) 0;
    //pagedirectoryList[5] = (unsigned long) 0;	
    //...


	//
	// Inicializando a lista de pagetables..
	//
	for(Index = 0; Index < PAGETABLE_COUNT_MAX; Index++){
	    pagetableList[Index] = (unsigned long) 0;
	};


    //Configurando manualmente as primeiras entradas da lista.
	pagetableList[0] = (unsigned long) &km_page_table[0];
	pagetableList[1] = (unsigned long) &km2_page_table[0];
	pagetableList[2] = (unsigned long) &um_page_table[0];
	pagetableList[3] = (unsigned long) &vga_page_table[0];
	pagetableList[4] = (unsigned long) &lfb_page_table[0];
	pagetableList[5] = (unsigned long) &buff_page_table[0];
	//pagetableList[6] = (unsigned long) 0;
    //...


	//
	// Inicializando a lista de pageframes.
	//
	for(Index = 0; Index < PAGEFRAME_COUNT_MAX; Index++){
	    pageframesList[Index] = (unsigned long) 0;
	};


	//Configurando manualmente a lista de pageframes.
	pageframesList[0] = (unsigned long) 0;
	pageframesList[1] = (unsigned long) 0;
	//...


	//
	// Inicializando a lista de framepools. (parti��es)
	//
	for(Index = 0; Index < FRAMEPOOL_COUNT_MAX; Index++){
	    framepoolList[Index] = (unsigned long) 0;
	};


	//Configurando manualmente a lista de pageframes.
	framepoolList[0] = (unsigned long) 0;
	framepoolList[1] = (unsigned long) 0;
	//...


	//
	// Creating kernel space framepool. 
	//

	struct frame_pool_d *kfp;

	//kernel framepool.
	kfp = (void*) malloc( sizeof(struct frame_pool_d) );
	if( (void*) kfp != NULL  )
	{
		kfp->id = 0;
		kfp->used = 1;
		kfp->magic = 1234;
		kfp->address = (unsigned long) (0 * MB);   //?? Come�a em 0 MB. ??
		
		//pertence ao processo kernel.
		kfp->process = (void*) KernelProcess;
		
		kfp->next =  NULL;
		//...

		//salva e ponteiro global.
		framepoolKernelSpace = (void*) kfp;

		//Salva na lista.
		framepoolList[0] = (unsigned long) kfp;
	};


    //
	// Creating user space framepool for small systems.
	//

	struct frame_pool_d *small_fp;

	//kernel framepool.
	small_fp = (void*) malloc( sizeof(struct frame_pool_d) );
	if( (void*) small_fp != NULL  )
	{
		small_fp->id = 1;
		small_fp->used = 1;
		small_fp->magic = 1234;
		small_fp->address = (unsigned long) (4 * MB);   //Come�a em 4 MB.

		//pertence ao processo kernel.
		small_fp->process = (void*) NULL; //??;

		small_fp->next =  NULL;
		//...

		//salva e ponteiro global.
		framepoolSmallSystemUserSpace = (void*) small_fp;

		//Salva na lista.
		framepoolList[1] = (unsigned long) small_fp;
	};


   //@todo: Outros indices, (2,3,4.)
   
	//Obs: Tem um buffer em  0x01000000 (16MB).
	//...

	//
	// Creating pageble space framepool.
	//

	struct frame_pool_d *pageable_fp;
	
	//kernel framepool.
	pageable_fp = (void*) malloc( sizeof(struct frame_pool_d) );
	if( (void*) pageable_fp != NULL  )
	{
		pageable_fp->id = 5;   //quinto �ndice.
		pageable_fp->used = 1;
		pageable_fp->magic = 1234;
		pageable_fp->address = (unsigned long) (20 * MB);   //Come�a em 20 MB.

		//pertence ao processo kernel.
		pageable_fp->process = (void*) NULL; //??
		
		pageable_fp->next =  NULL;
		//...

		//salva em ponteiro global.
		framepoolPageableSpace = (void*) pageable_fp;

		//Salva na lista.
		framepoolList[5] = (unsigned long) pageable_fp;
	};

	//???	
	//M�ximo de indices de quantidades de framepools na �rea de pagin�veis. 	
	//g_pageable_framepool_index_max = PAGEABLE_FRAMEPOOL_MAX;	

	//Usaremos o ultimo maga pra que??? o 32mb.

	//Obs: Tem LFB em 0xE0000000 //Excepcional.(� a mem�ria da placa de v�deo)

	//struct frame_pool_d *frontbuffer_fp; //lfb

	//kernel framepool.
	//frontbuffer_fp = (void*) malloc( sizeof(struct frame_pool_d) );
	//if( (void*) frontbuffer_fp != NULL  )
	//{
		//frontbuffer_fp->id = 5;   //quinto �ndice.
	//	frontbuffer_fp->used = 1;
	//	frontbuffer_fp->magic = 1234;

		//Obs: N�o sei se o endere�o do LFB � m�ltiplo de 4MB.
		//frontbuffer_fp->address = (unsigned long) SavedLFB; //Talvez 0xe0000000;   

		//pertence ao processo kernel.
	//	frontbuffer_fp->process = (void*) NULL; //??

	 //   frontbuffer_fp->next =  NULL;
		//...

		//salva em ponteiro global.
	//	framepoolFrontBuffer1 = (void*) frontbuffer_fp;

		//Salva na lista.
		//framepoolList[??] = (unsigned long) frontbuffer_fp;
	//};	


	// **** Estamos atuando em uma mem�ria de 32MB por enquanto.

	
	
	
	//
	// More?!
	//

// Done.
done:
	printf("Done!\n");
    return (int) 0;
};


//
// End.
//

