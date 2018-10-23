/*
 * File: sm\disk\disk.c 
 * 
 * Descri��o:
 *     Gerenciador de discos. 
 *     Arquivo principal do Disk Manager.
 *     MB - M�dulos inclu�dos no Kernel Base.
 *
 *     O Gerenciador de Discos � um m�dulo muito importante do Kernel.
 *
 * Credits:
 *     Some IDE/SATA support - Nelson Cole.
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2018 - IDE COntroller by Nelson Cole.
 *            Revision.
 *
 */

 
//
// Obs: o foco est� na lista de discos. diskList
//


#include <kernel.h>


//
// ==============================================================================================
// ATA  - TESTANDO O SUPORTE A ATA DO NELSON...
// ==============================================================================================
//


/* *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.3
 */


extern st_dev_t *current_dev;
//static _u32 ata_irq_invoked = 1; 
static _u32 ata_irq_invoked = 0;



int disk_get_ata_irq_invoked (){
	
	return (int) ata_irq_invoked;
};


void disk_reset_ata_irq_invoked (){
	
	ata_irq_invoked = 0;
};


void ata_wait (_i32 val){
	
   val /= 100;
   
    
	//while(val--)delay();
	while (val--)
    {
	   io_delay();
	};
};


// TODO: Nelson, ao configurar os bits BUSY e DRQ 
// devemos verificar retornos de erros.

_u8 ata_wait_not_busy (){
	
    while ( ata_status_read() & ATA_SR_BSY )
        if ( ata_status_read() &ATA_SR_ERR )
            return 1;

    return 0;
};


_u8 ata_wait_busy (){
	
    while(!(ata_status_read() &ATA_SR_BSY))
    if(ata_status_read() &ATA_SR_ERR)
    return 1;

    return 0;

}


_u8 ata_wait_no_drq()
{
    while(ata_status_read() &ATA_SR_DRQ)
    if(ata_status_read() &ATA_SR_ERR)
    return 1;

    return 0;
}

_u8 ata_wait_drq()
{
    while(!(ata_status_read() &ATA_SR_DRQ))
    if(ata_status_read() &ATA_SR_ERR)
    return 1;

    return 0;
}



_u8 ata_wait_irq (){
	
   _u32 tmp = 0x10000;
   _u8 data;
   
    while (!ata_irq_invoked)
    {
        data = ata_status_read();
        
		if( (data &ATA_SR_ERR) )
		{
            ata_irq_invoked = 0;
            return -1;
        }
                         //ns
        if(--tmp)ata_wait(100);
        else{
        ata_irq_invoked = 0;
        return 0x80;
        }
    }
 
    ata_irq_invoked = 0;
    return 0;
}

_void ata_soft_reset()
{
    _u8 data =  inb(ata.ctrl_block_base_address + 2);
    outb(ata.ctrl_block_base_address, data | 0x4);
    outb(ata.ctrl_block_base_address, data &0xfb);    
}


//#bugbug
//L� o status de um disco determinado, se os valores  
//na estrutura estiverem certos.
_u8 ata_status_read (){
	
   	return inb( ata.cmd_block_base_address + ATA_REG_STATUS );
};


void ata_cmd_write(_i32 cmd_val)
{
           
    	// no_busy      	
	ata_wait_not_busy();
	outb(ata.cmd_block_base_address + ATA_REG_CMD,cmd_val);
	ata_wait(400);  // Esperamos 400ns
}



_u8 ata_assert_dever(_i8 nport)
{

    switch (nport)
	{
        case 0:
            ata.channel = 0;
            ata.dev_num = 0;
        break;
		
        case 1:   
            ata.channel = 0;
            ata.dev_num = 1;
            break;
        
		case 2:
            ata.channel = 1;
            ata.dev_num = 0;
        break;
       
	    case 3:
            ata.channel = 1;
            ata.dev_num = 1;
        break;
		
        default:
            kprintf ("ata_assert_dever: Port %d, volue not used\n", nport );
            return -1;
            break;
    };

    set_ata_addr (ata.channel);
	
    return 0;
};


/*
 *********************************************
 * ide_identify_device:
 * 
 * IN:
 *
 * OUT:
 *     -1   =  Error. 
 *     0x80 = PATAPI ou SATAPI.
 *     0    = PATA ou SATA. 
 *
 */
int ide_identify_device ( uint8_t nport ){
	
    _u8 status;
    _u8 lba1, lba2;

    ata_assert_dever (nport);

	//
    // ## Ponto flutuante ##
	//
	
	

    // ??
	// Pelo jeito isso significa que se o registrador de status retornar 
	// -1, significa que nenhum disco est� conectado ao controlador.
	
	// Sem unidade conectada ao barramento.	
	
	if ( ata_status_read() == 0xff )
	{
		//??message
		//se n�o h� dispositivo, temos que zerar os valores dentro das
		//estruturas de controle do ide.
		
        return (int) -1;
	};
	
	//
	// ??
	// O que isso faz?
	// Parece inicializa��o com '0'.
	//

    outb ( ata.cmd_block_base_address + ATA_REG_SECCOUNT, 0 );  // Sector Count 7:0
	outb ( ata.cmd_block_base_address + ATA_REG_LBA0, 0 );      // LBA 7-0   
	outb ( ata.cmd_block_base_address + ATA_REG_LBA1, 0 );      // LBA 15-8
	outb ( ata.cmd_block_base_address + ATA_REG_LBA2, 0 );      // LBA 23-16

    //
    // Select device,
    // ??
	//
	
	outb ( ata.cmd_block_base_address + ATA_REG_DEVSEL, 0xE0 | ata.dev_num << 4 );    
	ata_wait (400);

	//
    // cmd
    // O Comando � para identificar o dispositivo.
	//
	
	ata_cmd_write (ATA_CMD_IDENTIFY_DEVICE); 
    ata_wait (400);
    
	// ata_wait_irq();
    // Nunca espere por um IRQ aqui
    // Devido unidades ATAPI, ao menos que pesquisamos pelo Bit ERROR
    // Melhor seria fazermos polling

	//Sem unidade no canal
    if ( ata_status_read() == 0 )
	{
        //??message		
        return (int) -1;
	};

    //
	// ?? LBA ??
	//
	
	lba1 = inb ( ata.cmd_block_base_address + ATA_REG_LBA1 );
    lba2 = inb ( ata.cmd_block_base_address + ATA_REG_LBA2 );

	// ??
    if ( lba1 == 0x14 && lba2 == 0xEB )
    {
        //kputs("Unidade PATAPI\n");   
        ata_cmd_write(ATA_CMD_IDENTIFY_PACKET_DEVICE);
        ata_wait(400);
        ata_wait_drq(); 
        ata_pio_read(ata_identify_dev_buf,512);
        ata_wait_not_busy();
        ata_wait_no_drq();
        
        //salvando o tipo em estrutura de porta.
		ide_ports[nport].id = (int) nport;
		ide_ports[nport].used = (int) 1;
		ide_ports[nport].magic = (int) 1234;
		ide_ports[nport].name = "PATAPI";			
		ide_ports[nport].type = (int) idedevicetypesPATAPI;
		
		
		return 0x80;
   
    // ??
    }
    else if ( lba1 == 0x69  && lba2 == 0x96 ){

        //kputs("Unidade SATAPI\n");   
        ata_cmd_write(ATA_CMD_IDENTIFY_PACKET_DEVICE);
        ata_wait(400);
        ata_wait_drq(); 
        ata_pio_read(ata_identify_dev_buf,512);
        ata_wait_not_busy();
        ata_wait_no_drq();
		
        //salvando o tipo em estrutura de porta.
		ide_ports[nport].id = (int) nport;
		ide_ports[nport].used = (int) 1;
		ide_ports[nport].magic = (int) 1234;
		ide_ports[nport].name = "SATAPI";	
		ide_ports[nport].type = (int) idedevicetypesSATAPI;
		
        return 0x80;

    // ??
	}
    else if ( lba1 == 0x3C && lba2 == 0xC3 ){
		
        //kputs("Unidade SATA\n");   
        // O dispositivo responde imediatamente um erro ao cmd Identify device
        // entao devemos esperar pelo DRQ ao invez de um BUSY
        // em seguida enviar 256 word de dados PIO.
        
		ata_wait_drq(); 
        ata_pio_read(ata_identify_dev_buf,512);
        ata_wait_not_busy();
        ata_wait_no_drq();
		
        //salvando o tipo em estrutura de porta.
		ide_ports[nport].id = (int) nport;
		ide_ports[nport].used = (int) 1;
		ide_ports[nport].magic = (int) 1234;
		ide_ports[nport].name = "SATA";	
		ide_ports[nport].type = (int) idedevicetypesSATA;
		
        return 0;
    
	// ??
	}
    else if ( lba1 == 0 && lba2 == 0 ){
		
        // kputs("Unidade PATA\n");
        // aqui esperamos pelo DRQ
        // e eviamoos 256 word de dados PIO
        
		ata_wait_drq();
        
		// ## read ##
		ata_pio_read ( ata_identify_dev_buf, 512 );

        ata_wait_not_busy();
        ata_wait_no_drq();
		
		
        //salvando o tipo em estrutura de porta.
		ide_ports[nport].id = (int) nport;
		ide_ports[nport].used = (int) 1;
		ide_ports[nport].magic = (int) 1234;
		ide_ports[nport].name = "PATA";	
		ide_ports[nport].type = (int) idedevicetypesPATA;
		
        return 0;
    };


done:

	return (int) 0;   
};






/* *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.3
 *
 *
 */
//#include "ata.h"

#define DISK1 1
#define DISK2 2
#define DISK3 3
#define DISK4 4

static const char *ata_sub_class_code_register_strings[] ={
    "Unknown",
    "IDE Controller",
    "Unknown",
    "Unknown",
    "RAID Controller",
    "Unknown",
    "AHCI Controller"
};


extern st_dev_t *current_dev;



// base address 
static _u32 ATA_BAR0;    // Primary Command Block Base Address
static _u32 ATA_BAR1;    // Primary Control Block Base Address
static _u32 ATA_BAR2;    // Secondary Command Block Base Address
static _u32 ATA_BAR3;    // Secondary Control Block Base Address
static _u32 ATA_BAR4;    // Legacy Bus Master Base Address
static _u32 ATA_BAR5;    // AHCI Base Address / SATA Index Data Pair Base Address



 

void set_ata_addr (int channel){
	
	//
	// @todo:
	// Checar a validade da estrutura.
	//

    switch (channel)
	{
        case ATA_PRIMARY:
            ata.cmd_block_base_address  = ATA_BAR0;
            ata.ctrl_block_base_address = ATA_BAR1;
            ata.bus_master_base_address = ATA_BAR4;
            break;
			
        case ATA_SECONDARY:
            ata.cmd_block_base_address  = ATA_BAR2;
            ata.ctrl_block_base_address = ATA_BAR3;
            ata.bus_master_base_address = ATA_BAR4 + 8;
            break;
			
        default:
            //PANIC
            break;
    };

done:
    return;
};




/* *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.3
 *
 * Obs: O que segue s�o rotinas de suporte ao controlador IDE.
 *
 */


const char *dev_type[] = {
    "ATA",
    "ATAPI"
};

st_dev_t *current_dev;       // A unidade atualmente selecionada.
st_dev_t *ready_queue_dev;   // O in�cio da lista.
uint32_t  dev_next_pid = 0;  // O pr�ximo ID de unidade dispon�vel. 


/*
 ********************************************************
 * ide_mass_storage_initialize:
 *     Rotina de inicializa��o de dispositivo de 
 * armazenamento de dados.
 *
 */
void ide_mass_storage_initialize (){
	
	int port;
	

    //
    // Vamos trabalhar na lista de dispositivos.
    //	
	
	// Iniciando a lista.
	ready_queue_dev = ( struct st_dev * ) kmalloc ( sizeof( struct st_dev) );
	
	current_dev = ( struct st_dev * ) ready_queue_dev;
	
    current_dev->dev_id      = dev_next_pid++;
    current_dev->dev_type    = -1;
    current_dev->dev_num     = -1;
    current_dev->dev_channel = -1;
    current_dev->dev_nport   = -1;
    current_dev->next        = NULL;

    // ??
	ata_identify_dev_buf = ( _u16 * ) kmalloc (4096);


	//
	// Sondando dispositivos e imprimindo na tela.
	//
	
    // As primeiras quatro portas do controlador IDE.    
	for ( port=0; port < 4; port++ )
	{
		// ??
        ide_dev_init (port);
	};
    
done:
    return;
};


/*
 *******************************************************************
 * ide_dev_init:
 *     ?? Alguma rotina de configura��o de dispositivos.
 *
 */
int ide_dev_init(char port)
{
    int data;

    st_dev_t *new_dev;	
	
    new_dev = ( struct st_dev * ) kmalloc( sizeof( struct st_dev) );
    
	//@todo: Checar a validade da estrutura.
	if( (void*) new_dev ==  NULL ){
		
		printf("ide_dev_init: struct");
		refresh_screen();
		while(1){}
	} 
	
	
	data = (int) ide_identify_device(port);
	
    if( data == -1 )
	{
		//@todo: Message.
        return (int) 1;
	};
	
	if( data == 0 )
	{
        // Unidades ATA.

        new_dev->dev_type   = (ata_identify_dev_buf[0] &0x8000)? 0xffff: ATA_DEVICE_TYPE;
        new_dev->dev_access = (ata_identify_dev_buf[83]&0x0400)? ATA_LBA48: ATA_LBA28;
        
		if(ATAFlag == FORCEPIO)
		{
			//com esse s� funciona em pio
		    new_dev->dev_modo_transfere = 0;	
		}else{
		    //com esse pode funcionar em dma
		    new_dev->dev_modo_transfere = (ata_identify_dev_buf[49]&0x0100)? ATA_DMA_MODO: ATA_PIO_MODO;
        };
		
		new_dev->dev_total_num_sector   = ata_identify_dev_buf[60];
        new_dev->dev_total_num_sector  += ata_identify_dev_buf[61];
		
        new_dev->dev_byte_per_sector = 512;
		
        new_dev->dev_total_num_sector_lba48  = ata_identify_dev_buf[100];
        new_dev->dev_total_num_sector_lba48 += ata_identify_dev_buf[101];
        new_dev->dev_total_num_sector_lba48 += ata_identify_dev_buf[102];
        new_dev->dev_total_num_sector_lba48 += ata_identify_dev_buf[103];
		
        new_dev->dev_size = (new_dev->dev_total_num_sector_lba48 * 512);

        
    }else if( data == 0x80 )
	      {

              // Unidades ATAPI.

              new_dev->dev_type = (ata_identify_dev_buf[0]&0x8000)? ATAPI_DEVICE_TYPE: 0xffff;
              
			  new_dev->dev_access = ATA_LBA28;
              
			  if(ATAFlag == FORCEPIO)
			  {
                  //com esse s� funciona em pio 				  
			      new_dev->dev_modo_transfere = 0; 
			  }else{
			      //com esse pode funcionar em dma
			      new_dev->dev_modo_transfere = (ata_identify_dev_buf[49]&0x0100)? ATA_DMA_MODO: ATA_PIO_MODO;
              };
              			  
			    
			  
			  new_dev->dev_total_num_sector  = 0;
              new_dev->dev_total_num_sector += 0;
              
			  new_dev->dev_byte_per_sector = 2048; 
              
			  new_dev->dev_total_num_sector_lba48  = 0;
              new_dev->dev_total_num_sector_lba48 += 0;
              new_dev->dev_total_num_sector_lba48 += 0;
              new_dev->dev_total_num_sector_lba48 += 0;
              
			  new_dev->dev_size = (new_dev->dev_total_num_sector_lba48 * 2048);

    
          }else{
               
			   // @todo: Message.
			   // Identificar o erro
               return (int) 1;
		  };

    //Dados em comum.

    new_dev->dev_id = dev_next_pid++;
	
    new_dev->dev_num = ata.dev_num;
    
	new_dev->dev_channel= ata.channel;

    new_dev->dev_nport = port;
    
	//
	// port
	//
	
	switch( port )
	{
        case 0:
            dev_nport.dev0 = 0x81;
            break;
            
		case 1:
            dev_nport.dev1 = 0x82;
            break;
        
		case 2:
            dev_nport.dev2 = 0x83;
            break;
            
		case 3:
            dev_nport.dev3 = 0x84;
            break;

        //?? default ?? 			
    };


#ifdef KERNEL_VERBOSE
    kprintf("[ Detected Disk type: %s ]\n", dev_type[new_dev->dev_type] );
	refresh_screen();
#endif

    new_dev->next = NULL;

    //
    // Add no fim da lista (ready_queue_dev).
	//
	
    st_dev_t *tmp_dev; 
	
	tmp_dev = ( struct st_dev * ) ready_queue_dev;

    while( tmp_dev->next )
	{
        tmp_dev = tmp_dev->next;
    };
    
    tmp_dev->next = new_dev;
    
done:	
	return (int) 0;
};


/*
 * dev_switch:
 *     ?? Porque esse tipo ??
 */
static inline void dev_switch(void)
{
	// ??
    // Pula, se ainda n�o tiver nenhuma unidade.
    if( !current_dev ){
        return;
	}
	
    // Obter a pr�xima tarefa a ser executada.
    current_dev = current_dev->next;
    
    // Se ca�mos no final da lista vinculada, 
    // comece novamente do in�cio.
    if( !current_dev ){
        current_dev = ready_queue_dev;
    }
};


/*
 * getpid_dev:
 *     ?? Deve ser algum suporte a Processos.
 */
static inline int getpid_dev()
{
    return current_dev->dev_id;
};


/*
 * getnport_dev:
 *
 */
static inline int getnport_dev()
{
    return current_dev->dev_nport;
};


/*
 * nport_ajuste:
 *
 *
 */
int nport_ajuste( char nport )
{
    _i8 i = 0;
	
    while( nport != getnport_dev() )
	{
        if( i == 4 ){ 
		    return (int) 1; 
		}
        
		dev_switch();
        i++;
    };
	
    if( getnport_dev() == -1 ){ 
	    return (int) 1; 
	}
	
	
done:
    return (int) 0;
};



/**
 * Credits:
 *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.3
 *  
 * Obs: O que segue s�o rotinas de suporte a ATA.
 *
 */

 
// ?? 
// ## PIO READ ##  
void ata_pio_read ( void *buffer, _i32 bytes ){
	
    __asm__ __volatile__ (\
                "cld;\
                rep; insw": :"D" (buffer),\
                "d" ( ata.cmd_block_base_address + ATA_REG_DATA ),\
                "c" ( bytes/2 ) );
};

// ??
// ## PIO WRITE ## 
void ata_pio_write ( void *buffer, _i32 bytes ){
	
    __asm__ __volatile__ (\
                "cld;\
                rep; outsw": :"S" (buffer),\
                "d" ( ata.cmd_block_base_address + ATA_REG_DATA ),\
                "c" ( bytes/2 ) );
};



/**
 * Credits:
 *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.3
 *
 * Obs: O que segue s�o rotinas de suporte a IDE ATAPI.
 */

extern st_dev_t *current_dev;
extern uint8_t *dma_addr;


/*
 * atapi_pio_read:
 *
 *
 */
static inline void atapi_pio_read( void *buffer, uint32_t bytes )
{
    __asm__ __volatile__(\
                "cld;\
                rep; insw"::"D"(buffer),\
                "d"(ata.cmd_block_base_address + ATA_REG_DATA),\
                "c"(bytes/2));

};


 



/* *
 * Credits:
 *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.3
 *
 * Obs:
 *     O que segue s�o rotinas de suporte a IDE ATA.
 */

extern st_dev_t *current_dev;
_u8 *dma_addr;


/*
 * ata_set_device_and_sector:
 *
 *
 */
static inline _void ata_set_device_and_sector( _u32 count, _u64 addr,\
                                               _i32 access_type, _i8 nport )
{
    ata_assert_dever(nport);

	//
	// Access type.
	//
	
    switch( access_type )
	{
	    case 28:
            //Mode LBA28
	        outb( ata.cmd_block_base_address + ATA_REG_SECCOUNT,count);	// Sector Count 7:0
	        outb( ata.cmd_block_base_address + ATA_REG_LBA0,addr);		        // LBA 7-0   
	        outb( ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 8);          // LBA 15-8
	        outb( ata.cmd_block_base_address + ATA_REG_LBA2,addr >> 16);	        // LBA 23-16
            // Modo LBA active, Select device, and LBA 27-24
            outb( ata.cmd_block_base_address + ATA_REG_DEVSEL,0x40 |(ata.dev_num << 4) | (addr >> 24 &0x0f));
            
			// Verifique se e a mesma unidade para n�o esperar pelos 400ns.
            if( ata_record_dev != ata.dev_num && 
			    ata_record_channel != ata.channel )
			{
                ata_wait(400);
                //verifique erro
                ata_record_dev      = ata.dev_num;
                ata_record_channel  = ata.channel;
			};
            break;
			
        case 48:
            //Mode LBA48
            outb( ata.cmd_block_base_address + ATA_REG_SECCOUNT,0);	      // Sector Count 15:8
	        outb( ata.cmd_block_base_address + ATA_REG_LBA0,addr >> 24);  // LBA 31-24   
	        outb( ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 32);  // LBA 39-32
	        outb( ata.cmd_block_base_address + ATA_REG_LBA2,addr >> 40);  // LBA 47-40
	        outb( ata.cmd_block_base_address + ATA_REG_SECCOUNT,count);   // Sector Count 7:0
	        outb( ata.cmd_block_base_address + ATA_REG_LBA0,addr);		  // LBA 7-0   
	        outb( ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 8);   // LBA 15-8
	        outb( ata.cmd_block_base_address + ATA_REG_LBA2,addr >> 16);  // LBA 23-16
            outb( ata.cmd_block_base_address + ATA_REG_DEVSEL,0x40 | ata.dev_num << 4);   // Modo LBA active, Select device,        
            
			// Verifique se e a mesma unidade para n�o esperar pelos 400ns.
            if( ata_record_dev != ata.dev_num && 
			    ata_record_channel != ata.channel )
			{
                ata_wait(400);
                ata_record_dev     = ata.dev_num;
                ata_record_channel = ata.channel;
			};
            break;
			
        case 0:
            // Modo CHS
            // not supported
            break;

        // Default ??

    };
       
//done:
    // ??
};


 


/**
 * Credits:
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.3
 * Legacy Bus Master Base Address
 *
 *  TODO Nelson, j� mais se esque�a de habiliatar o // Bus Master Enable
 *  no espa�o de configura�ao PCI (offset 0x4 Command Register)
 *
 * Obs: O que segue � um suporte ao controlador de DMA para uso nas
 * rotinas de IDE.
 *
 */

 
//
// DMA support
//

// Commands dma 
#define dma_bus_start   1
#define dma_bus_stop    0
#define dma_bus_read    0
#define dma_bus_write   1


// Status dma
#define ide_dma_sr_err     0x02


// Registros bus master base address
#define ide_dma_reg_cmd     0x00
#define ide_dma_reg_status  0x02
#define ide_dma_reg_addr    0x04

// channel
#define ide_dma_primary     0x00
#define ide_dma_secundary   0x01


/*
 * ide_dma_prdt:
 *
 */
struct {
    uint32_t addr;
    uint32_t len;
}ide_dma_prdt[4];


/*
 * ide_dma_data:
 *     ??
 *
 */
void ide_dma_data( void *addr, 
                   uint16_t byte_count,
				   uint8_t flg,
				   uint8_t nport )
{
    _u8 data;
    uint32_t phy;
	
    //
    // @todo: Check limits.
    //
	
    ide_dma_prdt[nport].addr = (_u32) addr;  //@todo: (&~1)sera que e necessario?
    ide_dma_prdt[nport].len  = byte_count |0x80000000;

    phy = (uint32_t) &ide_dma_prdt[nport];

    // prds physical.
    outportl( ata.bus_master_base_address + ide_dma_reg_addr, phy );
 
    // (bit 3 read/write)
    // 0 = Memory reads.
    // 1 = Memory writes.
	
    data = inb( ata.bus_master_base_address + ide_dma_reg_cmd ) &~8;

	//
	// TODO bit 8 Confilito no Oracle VirtualBox
	// Obs: Isso foi enviado via argumento e agora foi alerado.
	//
	
    flg = 1;  
	
    outb( ata.bus_master_base_address + ide_dma_reg_cmd, data | flg << 3 );
	
    // Limpar o bit de interrup��o e 
	// o bit de erro no registo de status.
	
    data = inb( ata.bus_master_base_address + ide_dma_reg_status );
    outb( ata.bus_master_base_address + ide_dma_reg_status, data &~6 );

done:
    return;	
};


/*
 * ide_dma_start:
 *
 */
void ide_dma_start()
{
    _u8 data = inb( ata.bus_master_base_address + ide_dma_reg_cmd );
    outb( ata.bus_master_base_address + ide_dma_reg_cmd, data | 1);
};


/*
 * ide_dma_stop:
 *
 */
void ide_dma_stop()
{
    _u8 data = inb( ata.bus_master_base_address + ide_dma_reg_cmd );  
	outb( ata.bus_master_base_address + ide_dma_reg_cmd, data &~1);
	
    data = inb( ata.bus_master_base_address + ide_dma_reg_status );
    outb( ata.bus_master_base_address + ide_dma_reg_status, data &~6);
	
done:	
    return;	
};


/*
 * ide_dma_read_status:
 *     DMA read status.
 */
int ide_dma_read_status()
{
    return inb( ata.bus_master_base_address + ide_dma_reg_status );
};


//
// pci support
//


#define PCI_PORT_ADDR 0xCF8
#define PCI_PORT_DATA 0xCFC

#define CONFIG_ADDR(bus,device,fn,offset)\
                       (\
                       (((uint32_t)(bus) &0xff) << 16)|\
                       (((uint32_t)(device) &0x3f) << 11)|\
                       (((uint32_t)(fn) &0x07) << 8)|\
                       ((uint32_t)(offset) &0xfc)|0x80000000)
					   
					   
const char *pci_classes[] = {
    "Unknown [old]",
    "Mass storage",
    "Network",
    "Display",
    "Multimedia device",
    "Memory",
    "Bridge device",
    "Simple Communication",
    "Base System Peripheral",
    "Input Device",
    "Docking Station",
    "Processor",
    "Serial Bus",
    "Wireless",
    "Inteligent I/O",
    "Satellite Communications",
    "Encrypt/Decrypt",
    "Data acquisition and signal processing",
    [255]="Unknown"
};
					   
					   
/*
 * read_pci_config_addr:
 *     READ
 */
uint32_t diskReadPCIConfigAddr( int bus, 
                               int dev,
							   int fun, 
							   int offset )
{
    outportl( PCI_PORT_ADDR, CONFIG_ADDR( bus, dev, fun, offset ) );
	
done:
    return (uint32_t) inportl(PCI_PORT_DATA);
};


/*
 * write_pci_config_addr:
 *     WRITE
 */
void diskWritePCIConfigAddr( int bus, 
                            int dev,
							int fun, 
							int offset, 
							int data )
{
    outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, offset));
    outportl(PCI_PORT_DATA,data);
};




/*
 *************************************************************
 * diskATAPCIConfigurationSpace:
 *     Espa�o de configura�ao PCI Mass Storage
 */
int diskATAPCIConfigurationSpace ( char bus, char dev, char fun ){
	
    uint32_t data;

//#ifdef KERNEL_VERBOSE	
	kprintf("diskATAPCIConfigurationSpace:\n");
    kprintf("Initializing PCI Mass Storage support ...\n");
//#endif

    // Indentification Device
    data = (uint32_t) diskReadPCIConfigAddr ( bus, dev, fun, 0 );
	
	// Salvando configura��es.
    ata_pci.vendor_id = data &0xffff;
    ata_pci.device_id = data >> 16 &0xffff;
	
//#ifdef KERNEL_VERBOSE	
	kprintf("\nDisk info:\n");
    kprintf("[ Vendor ID: %X,Device ID: %X ]\n", ata_pci.vendor_id, 
	    ata_pci.device_id );
//#endif	
	
    // Classe code, programming interface, revision id
    data  = (uint32_t) diskReadPCIConfigAddr ( bus, dev, fun, 8 );
    
	//
	// #importante:
	// Salvando na estrutura as informa��es obtidas.
	//
	
	// Classe e sub-classe.
    ata_pci.classe = data >> 24 &0xff;
    ata_pci.subclasse = data >> 16 &0xff;
    
	// prog if.
	ata_pci.prog_if = data >> 8 &0xff;
   
    // Revision.
    ata_pci.revision_id = data &0xff;

	//
	// Se for classe 1 subclasse 1. (IDE)
	//
	
    if( ata_pci.classe == 1 && 
	    ata_pci.subclasse == 1 )
	{
        ata.chip_control_type = ATA_IDE_CONTROLLER; 
                   
        // IDE
	
        // Compatibilidade e nativo, primary.
        data  = diskReadPCIConfigAddr( bus, dev, fun, 8 );
        if( data &0x200 )
		{ 
	        diskWritePCIConfigAddr( bus, dev, fun, 8, data | 0x100 ); 
		};        

        // Compatibilidade e nativo, secundary.
        data = diskReadPCIConfigAddr( bus, dev, fun, 8 );
        if( data &0x800 )
		{ 
	        diskWritePCIConfigAddr( bus, dev, fun, 8, data | 0x400 ); 
		};        

        data = diskReadPCIConfigAddr( bus, dev, fun, 8 );
        if( data &0x8000 )
		{    
            // Bus Master Enable
            data = diskReadPCIConfigAddr(bus,dev,fun,4);
            diskWritePCIConfigAddr(bus,dev,fun,4,data | 0x4);
        }; 

	    // Habilitar interrupcao (INTx#)
        data = diskReadPCIConfigAddr( bus, dev, fun, 4 );
        diskWritePCIConfigAddr( bus, dev, fun, 4, data & ~0x400);

       	// IDE Decode Enable
       	data = diskReadPCIConfigAddr( bus, dev, fun, 0x40 );
       	diskWritePCIConfigAddr( bus, dev, fun, 0x40, data | 0x80008000 );

        // Synchronous DMA Control Register
	    // Enable UDMA
	    data = diskReadPCIConfigAddr( bus, dev, fun, 0x48 );
	    diskWritePCIConfigAddr( bus, dev, fun, 0x48, data | 0xf);

//#ifdef KERNEL_VERBOSE 		
        kprintf("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
            ata_sub_class_code_register_strings[ata.chip_control_type],
	        ata_pci.prog_if,
			ata_pci.revision_id );
//#endif

        //
		// Se a classe for 1 e a sbclasse for 4 (RAID)
		//

            
    }else if( ata_pci.classe == 1 && ata_pci.subclasse == 4 )
	      {
              //RAID
              ata.chip_control_type = ATA_RAID_CONTROLLER;
			  
//#ifdef KERNEL_VERBOSE              
			  kprintf("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
                  ata_sub_class_code_register_strings[ata.chip_control_type], 
				  ata_pci.prog_if,
				  ata_pci.revision_id );
//#endif
    
			  // Em avaliacao
              return PCI_MSG_AVALIABLE;
			  
			  
			  
            //
		    // Se a classe for 1 e a sbclasse for 6 (AHCI)
		    //			  
			  
          }else if( ata_pci.classe == 1 && 
	                ata_pci.subclasse == 6)
		        {
              
			        ata.chip_control_type = ATA_AHCI_CONTROLLER;
       
	                //
                    // ACHI.
                    //
		
                    // Compatibilidade e nativo, primary.
                    data = diskReadPCIConfigAddr( bus, dev, fun, 8 );
                    if( data &0x200 ){ 
		                diskWritePCIConfigAddr( bus, dev, fun, 8, data | 0x100 ); 
		            }        

                    // Compatibilidade e nativo, secundary.
                    data = diskReadPCIConfigAddr( bus, dev, fun, 8 );
                    if( data &0x800 ){ 
	                    diskWritePCIConfigAddr( bus, dev, fun, 8, data | 0x400 ); 
		            }        

		            // ??
                    data = diskReadPCIConfigAddr(bus,dev,fun,8);
                    if( data &0x8000 ) 
		            {    
                        // Bus Master Enable.
                        data = diskReadPCIConfigAddr( bus, dev, fun, 4 );
                        diskWritePCIConfigAddr( bus, dev, fun, 4, data | 0x4 );
                    } 

                    // IDE Decode Enable
                    data = diskReadPCIConfigAddr( bus, dev, fun, 0x40 );
                    diskWritePCIConfigAddr( bus, dev, fun, 0x40, data | 0x80008000 );

                    // Habilitar interrupcao (INTx#)
                    data = diskReadPCIConfigAddr( bus, dev, fun, 4 );
                    diskWritePCIConfigAddr( bus, dev, fun, 4, data & ~0x400);

#ifdef KERNEL_VERBOSE
                    kprintf("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
                        ata_sub_class_code_register_strings[ata.chip_control_type], 
		                ata_pci.prog_if,
			            ata_pci.revision_id );
#endif			

                    //
					// #importante:
					// Se o dispositivo n�o tiver nem a classe nem a subclasse desejanda.
					//
					
                    //Ok.
                }else{
					
					 //
                     // PANIC! 
                     //
					 
					 kprintf ("sm-disk-disk-diskATAPCIConfigurationSpace: PANIC DRIVER BLOCK!");
		             //die();
					 
					//panic code. 
					return 666; 
                 };
				 
				 
	//
    // Vamos salvar mais informa��es na estrutura.
    //	


    // PCI cacheline, Latancy, Headr type, end BIST
	
    data = diskReadPCIConfigAddr( bus, dev, fun, 0xC );
	
	// ??
	ata_pci.primary_master_latency_timer = data >>8 &0xff;
    
	ata_pci.header_type = data >>16 &0xff;
	
    ata_pci.BIST = data >>24 &0xff;
    
	// BARs
    ata_pci.bar0 = diskReadPCIConfigAddr( bus, dev, fun, 0x10 );
    ata_pci.bar1 = diskReadPCIConfigAddr( bus, dev, fun, 0x14 );
    ata_pci.bar2 = diskReadPCIConfigAddr( bus, dev, fun, 0x18 );
    ata_pci.bar3 = diskReadPCIConfigAddr( bus, dev, fun, 0x1C );
    ata_pci.bar4 = diskReadPCIConfigAddr( bus, dev, fun, 0x20 );
    ata_pci.bar5 = diskReadPCIConfigAddr( bus, dev, fun, 0x24 );
	
    //--------------
    
    // Interrupt
    data = diskReadPCIConfigAddr ( bus, dev, fun, 0x3C );
    
    //
    // Salvando configura��es.
    //		
	
	ata_pci.interrupt_line = data &0xff;
    ata_pci.interrupt_pin  = data >> 8 &0xff;


    // PCI command and status
    data = diskReadPCIConfigAddr ( bus, dev, fun, 4 );
    
    //
    // Salvando configura��es.
    //	
	
	ata_pci.command = data & 0xffff; 
    ata_pci.status  = data >>16 & 0xffff;
	
	
//#ifdef KERNEL_VERBOSE	
    kprintf ("[ Command %x Status %x ]\n", ata_pci.command, 
	    ata_pci.status );
		
    kprintf ("[ Interrupt Line %x Interrupt Pin %x ]\n", ata_pci.interrupt_pin, 
	    ata_pci.interrupt_line );
//#endif		
	
    data = diskReadPCIConfigAddr (bus,dev,fun,0x48);
	
//#ifdef KERNEL_VERBOSE		
    kprintf("[ Synchronous DMA Control Register %X ]\n", data );
//#endif
	
//done:

#ifdef KERNEL_VERBOSE	
    refresh_screen();
#endif 
 	
    return (PCI_MSG_SUCCESSFUL);
};



/*
 ********************************************************************
 * pci_scan_device:
 *
 *     Esta fun��o deve retornar o n�mero de barramento, 
 * o dispositivo e a fun��o do dispositivo conectado ao barramento PCI 
 * de acordo a classe.
 *
 */
uint32_t diskPCIScanDevice ( int class ){
	
    int bus, dev, fun;
    
	uint32_t data = -1;

	
#ifdef KERNEL_VERBOSE		
	printf("sm-disk-disk-diskPCIScanDevice:\n");
    refresh_screen();
#endif
    
    //
	// Probe.
	//
	
	for( bus=0; bus < 256; bus++ )
	{
        for( dev=0; dev < 32; dev++ )
		{
            for( fun=0; fun < 8; fun++ )
			{
                outportl ( PCI_PORT_ADDR, CONFIG_ADDR( bus, dev, fun, 0x8) );
				
                data = inportl (PCI_PORT_DATA);
                
				if ( ( data >> 24 &0xff ) == class )
				{
					
					
                    // Message.
//#ifdef KERNEL_VERBOSE							
					kprintf( "[ Detected PCI device: %s ]\n", 
					         pci_classes[class] );
//#endif
							 
					//
					// Done !
					//
                    
					return (uint32_t) ( fun + (dev*8) + (bus*32) );
                }
            };
     
        };
    };

	//
	// Fail !
	//
	
//fail:	
    
	kprintf("[ PCI device NOT detected ]\n");		
	refresh_screen();
	
    return (uint32_t) (-1);
};


/*
 ********************
 * diskATAInitialize:
 *     Inicializa o IDE e mostra informa��es 
 * sobre o disco.
 * Credits: Nelson Cole;
 */
int diskATAInitialize ( int ataflag ){
	
	int Status = 1;  //error
	int port;
	
	_u32 data;
	
	_u8 bus;
	_u8 dev;
	_u8 fun;
	
	
	//
	// Configurando flags do driver.
	//
	
	ATAFlag = (int) ataflag;
	
	//
	// Messages
	//
	
//#ifdef KERNEL_VERBOSE
    kprintf("\n");
    kprintf("sm-disk-disk-diskATAInitialize:\n");
    kprintf("Initializing IDE/AHCI support ...\n");
	//refresh_screen();
//#endif

    // Sondando a interface PCI para encontrarmos um dispositivo
    // que seja de armazenamento de dados.
	
	//PCI_CLASSCODE_MASS
	
    data = (_u32) diskPCIScanDevice (PCI_CLASSE_MASS);
    
	// Error.	
	if ( data == -1 )
	{
		kprintf("sm-disk-disk-diskATAInitialize: pci_scan_device fail. ret={%d} \n", (_u32) data );
		
	    // Abortar.
		Status = (int) (PCI_MSG_ERROR);
		goto fail;
	};
    
	//
	//
	//
	
    bus = ( data >> 8 &0xff );
    dev = ( data >> 3 &31 );
    fun = ( data &7 );

	//
	// Vamos saber mais sobre o dispositivo encontrado. 
	//
	
	// Nessa fun��es pegaremos v�rias informa��es e colocaremos em estrutura.
	
    data = (_u32) diskATAPCIConfigurationSpace ( bus, dev, fun );

	// Analisando o retorno. 
	
	switch (data)
	{
		//Retornou com sucesso.
		case PCI_MSG_SUCCESSFUL:
		    kprintf("diskATAPCIConfigurationSpace returned ok\n");
			break;
			
        //RAID foi encontrado mas n�o � suportado.
        case PCI_MSG_AVALIABLE:
            kprintf("sm-disk-disk-diskATAInitialize: RAID Controller Not supported.\n");
		    kprintf("RAID found but not supported\n ");
			Status = (int) 1;		
            break;

		//PANIC
		case 666:
		    kprintf("sm-disk-disk-diskATAInitialize: PANIC Not a valid class or subclass \n");
			die ();
		    break;
			
        //#bugbug 		
		//?? Esse retorno n�o existe na fun�ao.
		case PCI_MSG_ERROR:
            kprintf("sm-disk-disk-diskATAInitialize: Error Driver [%X]\n",data);
		    Status = (int) 1;		
            goto fail;
			break;
			
        default:
		    kprintf("sm-disk-disk-diskATAInitialize: ERROR Unexpected return value\n");
			die ();		
		    //Nothing.
            break;		
	};
	
	/*
	// Error.	
    if ( data == PCI_MSG_ERROR )
	{
        kprintf("sm-disk-disk-diskATAInitialize: Error Driver [%X]\n",data);
		Status = (int) 1;
		goto fail;  
	
	}else if( data == PCI_MSG_AVALIABLE )
	      {
              kprintf("sm-disk-disk-diskATAInitialize: RAID Controller Not supported.\n");
		      Status = (int) 1;
		      goto fail;  
          };
	*/

	
	//
    // Salvando informa��es.
    //	
	
	// Aqui estamos pegando informa��es na estrutura ata_pci sobre as BARs 
	// e manipulando essas informa��es.
	// ?? N�o sei o que est� fazendo aqui, talvez procurando endere�o de porta.

    // Initialize base address
    // AHCI/IDE Compativel com portas IO IDE legado
	
    ATA_BAR0 = ( ata_pci.bar0 & ~7 )   + ATA_IDE_BAR0 * ( !ata_pci.bar0 );
    ATA_BAR1 = ( ata_pci.bar1 & ~3 )   + ATA_IDE_BAR1 * ( !ata_pci.bar1 );       
    ATA_BAR2 = ( ata_pci.bar2 & ~7 )   + ATA_IDE_BAR2 * ( !ata_pci.bar2 );
    ATA_BAR3 = ( ata_pci.bar3 & ~3 )   + ATA_IDE_BAR3 * ( !ata_pci.bar3 );
    
	ATA_BAR4 = ( ata_pci.bar4 & ~0x7 ) + ATA_IDE_BAR4 * ( !ata_pci.bar4 );
    ATA_BAR5 = ( ata_pci.bar5 & ~0xf ) + ATA_IDE_BAR5 * ( !ata_pci.bar5 );
	
	//
	// Colocando nas estruturas.
	//
	
	ide_ports[0].base_port = (unsigned short) ATA_BAR0;
	ide_ports[1].base_port = (unsigned short) ATA_BAR1;
	ide_ports[2].base_port = (unsigned short) ATA_BAR2;
	ide_ports[3].base_port = (unsigned short) ATA_BAR3;
	//tem ainda a porta do dma na bar4
	
	
	//
	// De acordo com o tipo.
	//
	
	
	//
	// Se for IDE.
	//
	
	// Type
    if ( ata.chip_control_type == ATA_IDE_CONTROLLER )
	{

        //Soft Reset, defina IRQ
        outb ( ATA_BAR1, 0xff );
        outb ( ATA_BAR3, 0xff );
        outb ( ATA_BAR1, 0x00 );
        outb ( ATA_BAR3, 0x00 );

        ata_record_dev = 0xff;
        ata_record_channel = 0xff;

//#ifdef KERNEL_VERBOSE	
	    printf("Initializing IDE Mass Storage device ...\n");
	    refresh_screen();
//#endif    
	
	    //
	    // As estruturas de disco ser�o colocadas em uma lista encadeada.
	    //
	
	    //ide_mass_storage_initialize();
		
	
	

    //
    // Vamos trabalhar na lista de dispositivos.
    //	
	
	// Iniciando a lista.
	ready_queue_dev = ( struct st_dev * ) kmalloc ( sizeof( struct st_dev) );
	
	current_dev = ( struct st_dev * ) ready_queue_dev;
    current_dev->dev_id      = dev_next_pid++;
    current_dev->dev_type    = -1;
    current_dev->dev_num     = -1;
    current_dev->dev_channel = -1;
    current_dev->dev_nport   = -1;
    current_dev->next        = NULL;

    // ??
	ata_identify_dev_buf = ( _u16 * ) kmalloc (4096);


	//
	// Sondando dispositivos e imprimindo na tela.
	//
	
    //
	// #importante:
	// Sondando as quatro portas e salvando as informa��es sobre os dispositivos  
	// encontrados nela.
	//
	
	// As primeiras quatro portas do controlador IDE.    
	
	for ( port=0; port < 4; port++ )
	{
        ide_dev_init (port);
	};		
		
		
		
		//
		// Agora se for AHCI.
		//

    }else if( ata.chip_control_type == ATA_AHCI_CONTROLLER )
	      {
			  
		      //
              // Nothing for now !!!
              //			  

              // Aqui, vamos mapear o BAR5
              // Estou colocando na marca 28MB
    
//#ifdef KERNEL_VERBOSE	
              // printf("ata_initialize: mem_map para ahci\n");
              // refresh_screen();
//#endif
	
	          //mem_map( (uint32_t*)0x01C00000, (uint32_t*) ATA_BAR5, 0x13, 2);

              //kputs("[ AHCI Mass Storage initialize ]\n");
              //ahci_mass_storage_init();

          }else{
			  
			   //
			   // Panic !!
			   //
               
			   kprintf(" # Panic! # \n");
			   kprintf("sm-disk-disk-diskATAInitialize:\n");
		       kprintf("IDE and AHCI not found!\n");
			   die();
          };
	
//
// Ok
//

    Status = 0;
    goto done;
	
// fail
fail:
    kprintf("sm-disk-disk-diskATAInitialize: fail\n");    	
done:

//#ifdef KERNEL_VERBOSE 
    //#debug
	//kprintf("done!\n");
    //refresh_screen();
//#endif 
	
    return (int) Status;	
};



/*
 *******************************************
 * diskATADialog:
 *     Rotina de di�logo com o driver ATA.
 */
int diskATADialog ( int msg, 
                    unsigned long long1, 
				    unsigned long long2 )
{
    int Status = 1; //erro
	
	
    switch (msg)
    {
		//ATAMSG_INITIALIZE
		//Initialize driver.
		case 1:
		    diskATAInitialize ( (int) long1 );
		    Status = 0;
			goto done;
			break;
			
		//ATAMSG_REGISTER
        //registra o driver. 		
		//case 2:
		//    break;
			
		default:
		    goto fail;
			break;
	};

//
// Done:
//	
fail:
    printf("diskATADialog: fail\n");
done:
    return (int) Status;	
};


/*
 ***********************************
 * diskATAIRQHandler1
 *     irq 14 handler
 */
void diskATAIRQHandler1 (){
	
    ata_irq_invoked = 1;  
};


/*
 ***********************************
 * diskATAIRQHandler2
 *     irq 15 handler
 */
void diskATAIRQHandler2 (){
	
    ata_irq_invoked = 1;   
};



/*
 ******************************************************
 * show_ide_info:
 *     Mostrar as informa��es obtidas na inicializa��es 
 * do controlador.
 */
void show_ide_info (){
	
	printf("sm-disk-disk-show_ide_info:\n");
	
	
	int i;
	
	for ( i=0; i<4; i++ )
	{
		printf("\n\n");
		
		printf ("id=%d\n", ide_ports[i].id );
		
		printf ("used=%d\n", ide_ports[i].used );
		printf ("magic=%d\n", ide_ports[i].magic );
		
		printf ("type=%d\n", ide_ports[i].type );
		
		printf ("name=%s\n", ide_ports[i].name );
		
		printf ("base_port=%x\n", ide_ports[i].base_port );
	}
	
/*	
	
	// Estrutura 'ata'
	// Qual lista ??
	
	//pegar a estrutura de uma lista.
	
	//if( ata != NULL )
	//{
		printf("ata:\n");
 	    printf("type={%d}\n",                    (int) ata.chip_control_type);
	    printf("channel={%d}\n",                 (int) ata.channel);
	    printf("devType={%d}\n",                 (int) ata.dev_type);
	    printf("devNum={%d}\n",                  (int) ata.dev_num);
	    printf("accessType={%d}\n",              (int) ata.access_type);
	    printf("cmdReadMode={%d}\n",             (int) ata.cmd_read_modo);
		
	    printf("cmdBlockBaseAddress={%x}\n",     (int) ata.cmd_block_base_address);
	    printf("controlBlockBaseAddress={%x}\n", (int) ata.ctrl_block_base_address);
		printf("busMasterBaseAddress={%x}\n",    (int) ata.bus_master_base_address);
		printf("ahciBaseAddress={%x}\n",         (int) ata.ahci_base_address);
	//};
	
	
	
	printf ("\n\n ata_pci: \n");
	
	printf (" vendor %x \n", (int) ata_pci.vendor_id );
	printf (" device %x \n", (int) ata_pci.device_id );
	
	printf (" cmd %x \n", (int) ata_pci.command );
	printf (" status %x \n", (int) ata_pci.status );
	
	printf ("prog if %x \n", (int) ata_pci.prog_if );
	printf ("revision  %x \n", (int) ata_pci.revision_id );
	printf ("class %x \n", (int) ata_pci.classe );
	printf ("subclass %x \n", (int) ata_pci.subclasse );
	printf ("primary_master_latency_timer %x \n", (int) ata_pci.primary_master_latency_timer );
	printf ("header type %x \n", (int) ata_pci.header_type );
	printf ("bist %x \n", (int) ata_pci.BIST );
	
	printf ("bar 0 %x \n", (int) ata_pci.bar0 );
	printf ("bar 1 %x \n", (int) ata_pci.bar1 );
	printf ("bar 2 %x \n", (int) ata_pci.bar2 );
	printf ("bar 3 %x \n", (int) ata_pci.bar3 );
	printf ("bar 4 %x \n", (int) ata_pci.bar4 );
	printf ("bar 5 %x \n", (int) ata_pci.bar5 );
	
	printf ("subsystem vendor %x \n ", (int) ata_pci.subsystem_vendor_id );
	printf ("subsystem id %x \n ", (int) ata_pci.subsystem_id );
	
	printf ("capabilities_pointer %x \n ", (int) ata_pci.capabilities_pointer );
	printf ("int line %x \n ", (int) ata_pci.interrupt_line );
	printf ("int pin %x \n ", (int) ata_pci.interrupt_pin );
	
	// ...
	printf("more...\n");
	
	printf ("port bar 0 %x \n", (int) ATA_BAR0 );
	printf ("port bar 1 %x \n", (int) ATA_BAR1 );
	printf ("port bar 2 %x \n", (int) ATA_BAR2 );
	printf ("port bar 3 %x \n", (int) ATA_BAR3 );
	printf ("port bar 4 %x \n", (int) ATA_BAR4 );
	printf ("port bar 5 %x \n", (int) ATA_BAR5 );	
	
	// Estrutura 'atapi'
	// Qual lista ??
	
	
	// Estrutura 'st_dev'
	// Est�o na lista 'ready_queue_dev'	
	

   //...
   
  */
	
//done:
    refresh_screen();
};


//esperando pela interrup��o.
//OUT:
// 0    = ok por status da interrup��o. 
// -1   = ok por status do controlador.
// 0x80 = ok por tempo esperado.

int disk_ata_wait_irq (){
	
   _u32 tmp = 0x10000;
   _u8 data;
   
    while (!ata_irq_invoked)
    {
        data = ata_status_read ();
        
		if( (data &ATA_SR_ERR) )
		{
			
			//ok por status do controlador.
            ata_irq_invoked = 0;
            
			return (int) -1;
        }
        
		//ns
        if (tmp--)  //??
		{
			ata_wait (100);
        
		}else{
			
            //ok por tempo esperado.
			ata_irq_invoked = 0;
			
			return (int) 0x80;
        };
    };
 
    //ok por status da interrup��o.
	ata_irq_invoked = 0;
	
	// ok 
	
    return (int) 0;
};

//
// End.
//

