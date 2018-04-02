/*
 * Arquivo: install.c
 *
 * MB - M�dulos inclu�dos no Kernel Base.
 *
 * Descri��o:
 *     Rotinas de instala��o do sistema.
 *     Mbr, Vbr, Bootmanager, Bootloader, Metafiles, fonfigura��es.
 *     
 * @todo:
 *     Isso pode estar em um aplicativo de instala��o.
 *     Poderia ser um m�dulo do tipo M0.
 *
 *    Vers�o 1.0, 2015, 2016.
 */


#include <kernel.h>

//Vari�veis internas
//int installStatus;
//...


/*
 * install_os:
 *     Instala os arquivos do sistema operacional.
 *     BM.BIN, BL.BIN, (kernel N�o), IDLE.BIN, SHELL.BIN, TASKMAN.BIN.
 */
void install_os()
{
    //
	// BM.BIN
	//
	
    //
	// BL.BIN
	//
	
    //
	// IDLE.BIN
	//
	
    //
	// SHELL.BIN
	//
	
    //
	// TASKMAN.BIN
	//
	    
done: 
    return;
};


/*
 * install_boot_manager:
 *     Reinstala o bootmanager.
 *
 *     O Boot Manager � um arquivo de 32kb que fica no inicio do disco.
 *     O prop�sito dessa rotina � carregar na mem�ria uma vers�o atual 
 *     do arquivo BM.BIN e depois grav�la no in�cio do disco.
 */
void install_boot_manager()
{
   /* 
	//debug message.
	printf("* install_boot_manager: Reinstalando o Boot Manager ...\n");
	refresh_screen();
	
	//@todo: usar aloca��o din�mica.
	//ESSE BUFFER � MUITO GRANDE.
	unsigned long buf[1];//[32*1024]; //32kb
	
	printf("Loading ...\n");
	carregar_arquivo("BM      BIN",(unsigned long) buf);
	
	//@todo: Check file header on memory.
	
	//@todo: Gravar o arquivo ni disco.
	
	printf("done.");
	
	
done:
	refresh_screen();
	//while(1){}
	
	*/
done:
    return;
};


/*
 * install_bootmanager_stage1:
 *    Reinstala o stage 1 do bootmanager. (MBR)
 *
 */ 
void install_bootmanager_stage1()
{
  /*
	 * Teste: Funcionou, leu um arquivo.
	printf("*teste ...\n");
	refresh_screen();
	unsigned long buf[32];
	printf("Loading ...\n");
	carregar_arquivo("STAGE1  BIN",(unsigned long) buf);
	printf("Showing ...\n");
    printf("%s\n",buf);  			
	printf("*done.");
	refresh_screen();
	while(1){}
	*/
done:
	return;
};


/*
 * install_bootmanager_stage2:
 *     Reinstala o stage 2 do bootmanager.
 */ 
void install_bootmanager_stage2()
{
  /*
	 * Teste: Funcionou, leu um arquivo.
	printf("*teste ...\n");
	refresh_screen();
	unsigned long buf[32];
	printf("Loading ...\n");
	carregar_arquivo("STAGE1  BIN",(unsigned long) buf);
	printf("Showing ...\n");
    printf("%s\n",buf);  			
	printf("*done.");
	refresh_screen();
	while(1){}
	*/
done:
	return;
};

/*
 * install_bootmanager_metafile:
 *     Reinstala o metafile do bootmanager.
 *     O metafile poder� ser editado pelo usu�rio administrador.
 *
 */
void install_bootmanager_metafile()
{
  /*
	 * Teste: Funcionou, leu um arquivo.
	printf("*teste ...\n");
	refresh_screen();
	unsigned long buf[32];
	printf("Loading ...\n");
	carregar_arquivo("METAFILEBIN",(unsigned long) buf);
	printf("Showing ...\n");
    printf("%s\n",buf);  			
	printf("*done.");
	refresh_screen();
	while(1){}
	*/
done:
	return;
};

/*
int installInit()
{}
*/

//
//fim.
//

