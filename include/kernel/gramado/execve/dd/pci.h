/*
 * File: pci.h
 *
 * Descri��o:
 *     Peripheral Component Interconnect - (PCI).
 *     Header do driver de PCI do kernel base.
 *     Refer�ncia site: https://pci-ids.ucw.cz/read/PD
 *
 * Hist�rico:
 *     Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o 1.0, 2016 - Revis�o.
 *     ...
 */

#ifndef __PCI_H__
#define __PCI_H__




#define PCI_PORT_ADDR 0xCF8
#define PCI_PORT_DATA 0xCFC


					   

//
// Vari�veis internas.
//

unsigned long pci_handler_address;

//
// PCI class struct
//
/*
typedef struct pci_class_d pci_class_t;
struct pci_class_d
{
	unsigned long pci_class;
	unsigned long pci_subclass;
    char *name;	
};
pci_class_t *PciClass;
//
*/

//
// PCI device structure.
//
 
typedef struct pci_device_d pci_device_t;
struct pci_device_d 
{
	object_type_t objectType;
	object_class_t objectClass;		

	//callback 
	
	//Identifica��o.
	int deviceId;
    
	int deviceUsed;   
	int deviceMagic;   
    
	char *name;         

	//unsigned long flag;   
    //unsigned long error;  
	
	// Status do dispositivo.
	//int device_status;
	//char *device_status_string;
	
	//
	// @todo: Completar com os elementos que faltam.
	//        Seguindo a ordem padr�o.
	
	
	//Salvar tamb�m qual bus o device pertence.	
	//Localiza��o do dispositivo.
	unsigned char bus;
	unsigned char dev;   
    unsigned char func;
	
	
	//Primeiros.
	unsigned short Device;	
	unsigned short Vendor;

	unsigned short Status;	
	unsigned short Command;

	unsigned char classCode;
	unsigned char subclass;
	unsigned char progif;	
	unsigned char revisionId;


	unsigned char bist;
	unsigned char header_type;
	unsigned char latency_timer;	
	unsigned char cache_line_size;



	
	//Address.
	unsigned long BAR0;
	unsigned long BAR1;
	unsigned long BAR2;
	unsigned long BAR3;
	unsigned long BAR4;
	unsigned long BAR5;
	
	
	//Sub-system vendor id
	//sub-system device id	
	unsigned short subsystem_Vendor;
	unsigned short subsystem_Device;

	//�ltimos.

	unsigned char max_latency;
	unsigned char min_grant;
	unsigned char irq_pin;     //??
    unsigned char irq_line;    //Qual IRQ ser� usada pelo PIC.	
	
    //continua ...
	
	
	//estrutura para o driver do dispositivo.
	struct pci_driver_d *driver;
    
	struct pci_device_d* next;	
};
pci_device_t *pci_device;
pci_device_t *current_pci_device;    //Current.
//...


//
// PCI structure.
// Estrutura para gerenciar a interface pci.
// apenas uma inst�ncia dessa estrutura deve existir.
// 
//
// 

typedef struct pci_d pci_t;
struct pci_d
{	
	int devicesFound;    //N�mero de dispositivos encontrados.
	
	int	max;
	pci_device_t* deviceList;	
};
pci_t *Pci;



//
// Lista as estruturas de dispositivos pci.
//

unsigned long pcideviceList[32];    //@todo tamanho provisorio. 

//
// DRIVER.
//

/*
 * pci_driver_d:
 *     Estrutura para drivers de dispositivos pci.     
 *
 */
typedef struct pci_driver_d pci_driver_t;
struct pci_driver_d
{
	object_type_t objectType;
	object_class_t objectClass;	
	
	
	int id;    //id do driver de dispositivo pci.
	int used;
	int magic;
	
	//status do driver de dispositivo.
	//Initialized, ... @todo: Criar enum. pci_device_status_t;
	//int status;
	
	//Nome do driver do dispositivo PCI.
    const char *name;

	//Pathname para a imagem do driver.
    const char *pathname;	
	
	//unsigned long driver_version;
	
	//Endere�o da rotina de tratamento de interrup��o.
	//unsigned long irq_handler;
	
	//
	// Buffers !!!
	//
	
	//
	// Os endere�os de mem�ria encontrados na configura��o
	// de PCI poder�o ser salvos aqui.
	//
	
	//unsigned long io_buffer_address1;
	//unsigned long io_buffer_address2;
	//unsigned long io_buffer_address3;
	//unsigned long io_buffer_address4;
	
	//Estrutura do dispositivo pci gerenciado pelo driver.
	struct pci_device_d *pci_device;

    // ...

    //struct pci_driver_d *next;
};
pci_driver_t *PciDrivers;    //@todo: Lista.Igual menuitens.
//...


//Lista de drivers de dispositivos pci.
//unsigned long pcidriversList[32];





/*
 * The PCI interface treats multi-function devices as independent
 * devices.  The slot/function address of each device is encoded
 * in a single byte as follows:
 *
 *	7:3 = slot
 *	2:0 = function
 *
 * PCI_DEVFN(), PCI_SLOT(), and PCI_FUNC() are defined in uapi/linux/pci.h.
 * In the interest of not exposing interfaces to user-space unnecessarily,
 * the following kernel-only defines are being added here.
 */

//#define PCI_DEVID(bus, devfn)  ((((u16)(bus)) << 8) | (devfn))

/* return bus from PCI devid = ((u16)bus_number) << 8) | devfn */

//#define PCI_BUS_NUM(x) (((x) >> 8) & 0xff)






//
// Prot�tipos de fun��es.
//

int pciInfo();
int pciInit();


//Inicia o pci.
int init_pci();


//
// Read.
//

unsigned char pciConfigReadByte( unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset);
unsigned short pciConfigReadWord( unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset);
unsigned long pciConfigReadDWord( unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset);

//
// Offsets.
//
					  
unsigned short pciCheckVendor(unsigned char bus, unsigned char slot);    //Offset 0.	
unsigned short pciCheckDevice(unsigned char bus, unsigned char slot);    //Offset 2.
unsigned char pciGetSubClass(unsigned char bus, unsigned char slot);     //Offset 0x0A. 
unsigned char pciGetClassCode(unsigned char bus, unsigned char slot);    //Offset 0x0B.
unsigned long pciGetBAR(unsigned char bus, unsigned char slot, int number);    //get BARs (v�rios offsets.)
unsigned char pciGetInterruptLine(unsigned char bus, unsigned char slot);    //Offset 0x3C.
unsigned char pciGetInterruptPin(unsigned char bus, unsigned char slot);     //Interrut pin
//...


//Handlers para PCI. Cada handler pode ser compartilhado por at�
//4 dispositivos.
unsigned long KiPciHandler1();
unsigned long KiPciHandler2();
unsigned long KiPciHandler3();
unsigned long KiPciHandler4();
//...								
	

#endif 

//
// End.
//

