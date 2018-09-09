/*
 * File: unb\hdd.c 
 * 
 * Descri��o:
 *     Interface de acesso a discos do tipo HDD.
 *     Deve haver um driver para cada marca suportada.
 *     Driver IDE presente dentro do kernel.
 *
 *     Rotinas de hardware. N�o s�o opera��es no
 *     sistema de arquivos.
 *
 *     O sistema de arquivos chama essas rotinas.
 *     O gerenciamento de discos chama essas rotinas.
 *
 * Obs: Esse m�dulo � um gerenciador de controlador de hard disk
 *      Aqui n�o importa se � ATA ou Serial ATA. Rotinas espec�ficas
 * ser�o executadas pela classe apropriada de interface, aqui � ais geral.
 * Por exemplo: Sondagem por tipos de dispositivos de armazenamento.
 * H� muitos tipos de hard drive. Esse m�dulo � somente para hard drives.
 *
 *     Ambiente: (RING 0).
 *
 *  Vers�o: 1.0 - 2013, 2014, 2015, 2016.
 */

 
 
/*
 hd info:
 =======
 PIIX3 ATA: LUN#0: disk, PCHS=963/4/17, total number of sectors 65536. (Oracle Virtualbox)
 estat�stica de hoje:
 (apenas leitura, usando PIO mode)
00:01:59.902737 /Devices/IDE0/ATA0/Unit0/AtapiDMA            0 times
00:01:59.902742 /Devices/IDE0/ATA0/Unit0/AtapiPIO            0 times
00:01:59.902747 /Devices/IDE0/ATA0/Unit0/DMA                 0 times
00:01:59.902753 /Devices/IDE0/ATA0/Unit0/PIO              1699 times  ***
00:01:59.902760 /Devices/IDE0/ATA0/Unit0/ReadBytes      869376 bytes  ***
00:01:59.902766 /Devices/IDE0/ATA0/Unit0/WrittenBytes        0 bytes
 */ 
 
 
#include <kernel.h>


/*
 * Externs.
 */
 
extern void os_read_sector();
extern void os_write_sector();
extern void reset_ide0();	


//Usadas por read e write.
extern unsigned long hd_buffer;
extern unsigned long hd_lba;



//Vari�veis internas
int hddStatus;
int hddError;
//...


 

/*
 *****************************************
 * my_read_hd_sector:
 * eax - buffer
 * ebx - lba
 * ecx - null
 * edx - null
 * Op��o: void hddReadSector(....)
 */
void my_read_hd_sector( unsigned long ax, 
                        unsigned long bx, 
						unsigned long cx, 
						unsigned long dx )
{
    // Passando os argumentos.	
	hd_buffer = (unsigned long) ax;    //arg1 = buffer. 
	hd_lba    = (unsigned long) bx;    //arg2 = lba.
	
	// Read sector. (ASM)
	os_read_sector(); 	
    return;
};


/*
 *************************************
 * my_write_hd_sector:
 * eax - buffer
 * ebx - lba
 * ecx - null
 * edx - null
 * Op��o: void hddWriteSector(....)
 */
void my_write_hd_sector( unsigned long ax, 
                         unsigned long bx, 
						 unsigned long cx, 
						 unsigned long dx )
{
    // Passando os argumentos.	
	hd_buffer = (unsigned long) ax;    //arg1 = buffer. 
	hd_lba = (unsigned long) bx;       //arg2 = lba.
	
	// Write sector. (ASM)	
    // entry/x86/head/hwlib.inc
	
	os_write_sector(); 
	
	return;
};   


/*
 * init_hdd:
 *     Inicializa o driver de hd.
 *     @todo: Mudar para hddInit().
 */
int init_hdd()
{
    //
    // @todo: We need to do something here.
    //	

done:
    g_driver_hdd_initialized = (int) 1;
    return (int) 0;
};


/*
int hddInit();
int hddInit()
{
    hddStatus = 0;
    hddError = 0;
    //...

	init_hdd();
    return (int) 0;	
};
*/

//
// End.
//

