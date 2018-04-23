/*
 * Arquivo: install.h 
 *
 * Descri��o:
 *     Header para rotinas de instala��o de partes do sistema, 
 * BootManager, mbr, metafile ...
 *
 *      @todo: Talvez essas rotinas n�o devam ficar dentro do Kernel Base.
 *
 * Vers�o 1.0, 2015.
 */


void install_os();
void install_boot_manager();
void install_bootmanager_stage1();
void install_bootmanager_stage2();
void install_bootmanager_metafile();

//
//fim.
//
