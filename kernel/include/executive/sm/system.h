/*
 * File: sm\system.h (System Management)
 *
 * Descri��o:
 *     Header para rotinas de gerenciamento do sistema dentro do m�dulo 
 * executive do kernel.
 *     O subsistema recebe as chamadas dos programas que est�o em use mode. 
 * � a porta de entrada para o kernel.
 *
 * Obs:
 *     Lembrando que o sistema compreende o Boot, Kernel e User.
 *
 * @todo:
 *     todos os servi�os oferecidos est�o nos 
 *     arquivos principais dos m�dulos hal microkernel e executive
 *     e come�am com 'sys_'.
 *
 * Hist�rico:
 *     Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o 1.0, 2016 - Aprimoramento geral das rotinas b�sicas.
 *     //...
 */ 
 

//
// #### Aqui devemos configurar manualmente a destina��o do sistema. ####
// 
#define SYSTEM_EDITION 1               //1,2,3,4 ... #Define a edi��o atuala do sistema. 

#define SYSTEM_DEVELOPER_EDITION    1  //Cont�m ferramentas do desnvolvedor.
#define SYSTEM_WORKSTATION_EDITION  2  //Cont�m ferramentas de escrit�rio.
#define SYSTEM_SERVER_EDITION       3  //Cont�m ferramentas de comunica��o em rede.
#define SYSTEM_IOT_EDITION          4  //Cont�m ferramentas de suporte � dispositivos externos.
//...

#define developer_edition_string    "Gramado Developer Edition" 
#define workstation_edition_string  "Gramado Workstation Edition" 
#define server_edition_string       "Gramado Server Edition" 
#define iot_edition_string          "Gramado IOT edition" 
//...


#define OS_NAME "Gramado"


//
//    #### Aqui devemos configurar manualmente a vers�o do sistema.
//

//@todo:
//Esse � padr�o de vers�o. 
//N�o mudar, apenas fazer as atualiza��es. 
#define SYSTEM_VERSIONMAJOR 1 
#define SYSTEM_VERSIONMINOR 0
#define SYSTEM_VERSIONBUILT 0
#define SYSTEM_VERSIONREVISION 1   //++ inc.


/*
 * default user name.
 */
 
#define DEFAULT_USERNAME "default-[USER]"


/*
 * Abaixo temos uma lista de variaveis de ambiente usadas
 * pelo sistema, come�adas com SYSTEM_.
 *
 */ 


/*
 * Vari�veis de ambiente. (em ordem).
 * Obs: @todo: Vari�veis de ambiente ser�o salvas
 * em banco de dados. Em arquivos de configura��o.            
 */ 


//Diretories. 
#define SYSTEM_ROOT        "/root"                  //Diret�rio raiz.
#define SYSTEM_ROOTDIR     "/root"                  //Diret�rio raiz.
#define SYSTEM_BOOTDIR     "/root/boot"             //Arquivos de inicializa��o.
#define SYSTEM_USERSDIR    "/root/users"            //Diret�rio de usu�rios.
#define SYSTEM_TMPDIR      "/root/tmp"              //Temporary files.
#define SYSTEM_TRASHDIR    "/root/trash"            //Trash dir.
#define SYSTEM_DEVICESDIR  "/root/devices"          //Devices,(as entradas representam dispositivos).
#define SYSTEM_SYSTEMDIR   "/root/os/system"           //Arquivos do sistema.
#define SYSTEM_DRIVERSDIR  "/root/os/system/drivers"   //Drivers de dispositivos.
//... 
 
//Computer.
#define SYSTEM_COMPUTER    "COMPUTER=$"

//Hadware.
//...

//User.
#define SYSTEM_USER        "USER=/$"
#define SYSTEM_USERNAME    "USERNAME="
//...

//Media.
#define SYSTEM_MEDIADISK    "MEDIA=/disk(0000)"
#define SYSTEM_MEDIAVOLUME  "MEDIA=/volume(0000)"    //Parti��o.
//...
 
//

//devices.
#define SYSTEM_DEVICE          "DEVICE=/$"
#define SYSTEM_DEVICECPU       "DEVICE=/cpu"
#define SYSTEM_DEVICEMEMORY    "DEVICE=/memory"
#define SYSTEM_DEVICENIC       "DEVICE=/nic"     //NIC (placa de rede).
#define SYSTEM_DEVICEKEYBOARD  "DEVICE=/keyboard"
#define SYSTEM_DEVICEMOUSE     "DEVICE=/mouse"
//...


//
// @todo: Continuar criando vari�veis de ambiente.
//

//...



//
//    ****    SYSTEM METRICS INDEX **** 
//


//*Importante.
#define SM_NULL 0  
#define SM_SCREENWIDTH 1
#define SM_SCREENHEIGHT 2
#define SM_BPP          3 //Bits Per Pixel.
#define SM_BACKBUFFER_VA 4  //Virtual address for backbuffer.
#define SM_BACKBUFFER_PA 5  //phisical address for backbuffer.
#define SM_FRONTBUFFER_VA 6  //Virtual address for frontbuffer.
#define SM_FRONTBUFFER_PA 7  //phisical address for frontbuffer.
#define SM_KERNELSIZE 8
#define SM_KERNELHEAPSIZE 9
#define SM_KERNELSTACKSIZE 10
#define SM_CHARWIDTH 11
#define SM_CHARHEIGHT 12
#define SM_DEVELOPER_EDITION 13

//Continua ...
//@todo.


//
//    ****    SYSTEM STATUS INDEX   **** 
//
//*Importante.
#define SS_NULL 0  
#define SS_LOGGED 1      //estamos logados.?
#define SS_USING_GUI  2  // estamos no modo gr�fico?
#define SS_KERNELSTATUS 3 //
//Continua ...
//@todo.


 
//
// Globals.
// 


//Flag para habilitar as op��es para o desenvolvedor.
int gDeveloperOptions;

//Sobre a destina��o do sistema operacional.
// 1 - Developer Edition.
// 2 - Workstation Edition.
// 3 - Server Edition.
// 4 - IOT Edition.
//...
int gSystemEdition;


int gSystemStatus; //?? Usado pelo construtor.


//
//  **** Shutdown support   ****
//

//Salvar aqui o endere�o da rotina do BM que desliga a m�quina via APM.
//O endere�o e a rotina s�o de 32bit.
unsigned long shutdown_address;

static char *default_user_name     = "default-[USER]";
static char *systemDefaultUserName = "default-[USER]";
//...



//...


/*
 * Test.
typedef struct active_environment_d active_environment_t;
struct active_environment_d
{
	unsigned long disk_number;
	unsigned long volume_number;
    unsigned long directory_number;

    const char* disk_string;
    const char* volume_string;
    const char* directory_string;	
} 
struct active_environment_d *CurrentEnvironment;
*/ 


/*
 * VERSION:
 *     Estrutura para vers�o do sistema.    
 *     O que determina a vers�o do sistema � a vers�o do Kernel.
 *
 * @todo: 
 *     Usar apenas esses 4 elementos.
 *     Dados complementares sobre a vers�o devem ficar 
 *     na estrutura versioninfo.
 *
 * Obs: Talvez usar um header. version.h.
 * #bugbug j� tem version_t tefinida em algum lugar.
 *         Aparentemente n�o deu mais problema. 
 */
typedef struct version_d version_t;
struct version_d
{
    unsigned long Major;
    unsigned long Minor;	
	unsigned long Built;
	unsigned long Revision;
};
struct version_d *Version;
//...


/*
 * Version Info:
 *     Dados complemetares sobre a vers�o.
 */
typedef struct version_info_d version_info_t;
struct version_info_d
{
    struct version_d *version;    	
	char *string;             //String para o nome da vers�o "VERSION..."
	//char *string2;
	char *copyright_string;   //"ex: (c) Copyright Nora Code"
	//...
	
	//@todo:
	//Data,
	//
	
	//...
	
};
struct version_info_d *VersionInfo;

 

/*
 * system_d:
 *     System information structure:
 *     Essa estrutura guarda as informa��es gerais sobre o sistema.
 *     Guarda ponteiros para as principais estruturas do sistema, 
 * desde informa��es de boot, at� informa��es se��es de usu�rios.
 * 
 */
typedef struct system_d system_t;
struct system_d
{
	//Seguran�a, pra sertificar que a estrutura
	//alocada e esta em uso. e que n�o esta corrompida.
	int used;
	int magic;
	
	//version
	struct version_d *version;
	struct version_info_d *version_info;
	
	
	//
	// System Main Bar:
	//     Barra no topo da tela.    
	//     Essa � a ferramenta mais importante para ter acesso aos recursos do sistema.
	//     System | Applications | Window
	//
	//System Window (sw).
	//
	struct window_d * swBar;                //Barra.
	//Left: (Software).
	struct window_d * swItemSystem;         //Item (System).
	struct window_d * swItemApplications;   //Item (Applications).  
	struct window_d * swItemWindow;         //Item (Window).
	//Right: (Hardware)
	struct window_d * swItemMore;    //Abre uma bandeija ou menu com mais op��es.(V).
	struct window_d * swItemClock;   //rel�gio.	
	
	//
	// @todo: Version info.
	//
	//struct version_d *version;
	
    //
    // System  info.
    //	
	int ProcessorArchitecture;
	int NumberOfProcessors;
	int PageSize;
	
	
	//
	// Observa��o: Recentemente a CPU assumiu funcionalidades que 
	//              antes eram da Ponte Norte.
	//
	
	//
	// North Bridge (Device number) word.
	//
	unsigned long NorthBridge;
	
	//
	// South Bridge (Device number) word.
	//
	unsigned long SouthBridge;
	
	//
	// Estruturas fundamentais do sistema.
	//
	
	//Ponteiro para 3 estruturas b�sicas de arquivos.
	struct bootmanager_d  *Boot;      //Boot Manager.
	struct bootloader_d   *Loader;    //Loader Manager.
	struct kernel_d       *Kernel;    //Kernel Manager.
	
	//M�dulos e servi�os.
	//struct bootmanager_d  *systemBoot;      //Boot Manager.
	//struct bootloader_d   *systemLoader;    //Loader Manager.
	//struct kernel_d       *systemKernel;    //Kernel Manager.
	
	//Recursos.
	//struct memory_info_d *systemMemoryInfo;
	//struct gui_d *systemGui;
	//struct usession_d *systemUserSession; //Essa estrutura tem muitas informa��es.
	//...
	
	//
	// Obs: A estrutura de user session ter� muitas informa��es sobre recursos do sistema
	//      tem pool de diret�rios de p�ginas, pool de objetos, pool de desktops(wstation).
	//
	
	//
	//Lista de ponteiros de estrutura usadas para gerenciamento do sistema.
	//
	//@todo: Isso nao precisa ser assim, pode ser um ponteiro para lista de estruturas
	// de dispositivos.
    unsigned long handleList[256];
	struct devices_d *deviceList;
	
    //@todo: criar uma lista de defini��es de indices para esses handles.	
	
	/*
     * ring0_exported_d:
     *     Ponteiros para tabelas de fun��es exportadas pelos
     *     programas do sistema em ring 0.
     *     S�o 3 processos: BM.BIN, BL.BIN. KERNEL.BIN.
     *
     */
	struct ring0_exported_d *ring0_exported;    //Fun��es exportadas.
	
	//...
};
system_t *System;


//Boot Loader.
// Estrutura para guardar informa��es sobre o Boot Loader (BL.BIN).
typedef struct bootloader_d bootloader_t;
struct bootloader_d
{
    //
	// Informa��es b�sicas (nome do arquivo e endere�o na mem�ria).
    //
	
    char *name;               //File name BL.BIN
	unsigned long address;    //Endere�o.
	

	//
	// Ponteiro para bloco de informa��es.
	//
	
    //struct loader_block_d *LoaderBlock;  
	
	//
	// Outras informa��es
	//
	
	
	//...	
};
bootloader_t *BootLoader; 



//Boot Manager.
// Estrutura para guardar informa��es sobre o Boot Manager (BM.BIN).
typedef struct bootmanager_d bootmanager_t;
struct bootmanager_d
{
    //
	// Informa��es b�sicas (nome do arquivo e endere�o na mem�ria).
    //
	
	char *name;               //File name. (BM.BIN).
	unsigned long address;    //Endere�o.
	
	//
	// Ponteiro para bloco de informa��es.
	//
	
	//struct boot_block_d *BootBlock;  
	
	//
	// Outras informa��es
	//
	
	
	//...
};
bootmanager_t *BootManager;


//
// Vari�veis do sistema.
//



//
// Listas de cores para elemetos gr�ficos do sistema.
//

//@todo: Criar �ndices de cores.
//unsigned long system_colors[32];
//unsigned long kernel_colors[32];
//unsigned long window_colors[32];
//...
 
 
//
// Prototypes.
//



/*
 * services:
 *    Atende as chamadas das interrup��es do sistema.
 *    � a int 200.
 *    Obs: Essa rotina est� em service.c
 */
void *services( unsigned long number, 
                unsigned long arg2, 
			 	unsigned long arg3, 
				unsigned long arg4 );
				

/*
 * system_procedure:
 *     O procedimento default do sistema.
 *     Obs: Essa rotina est� em procedure.c.  
 */				
unsigned long system_procedure( struct window_d *window, 
                                int msg, 
								unsigned long long1, 
								unsigned long long2); 
													   

/*
 * systemcall:
 *     @todo: Essa fun��o deve ser chamada via kernel request quando em kernel mode.
 * Obs:
 *     As chamadas ao sistema s�o feitas via interrup��o, ou via kernel request.
 *     ??? @todo: Onde est� essa rotina ???
 */
int systemcall( unsigned long numero, 
                unsigned long ax, 
				unsigned long bx, 
				unsigned long cx, 
				unsigned long dx ); 


/*
 * registra_procedimento:
 *     Registra um procedimento de janela.
 *
 */				
unsigned long registra_procedimento( unsigned long arg1, 
                                     unsigned long arg2, 
									 unsigned long arg3, 
									 unsigned long arg4);
									 
									 

/*
 * SendMessage:
 *     Envia mensagem para um procedimento de janela.
 */									 
void SendMessage( struct window_d *window, 
                  int msg, 
				  unsigned long long1, 
				  unsigned long long2 );


/*
 * procedureHelp:
 *     Help messages.
 *     Obs: Essa rotina est� em procedure.c
 */
void procedureHelp();

/*
 * systemAbort:
 *     Abort system.
 */
void systemAbort();

/*
 * SetProcedure:
 *     Atualiza o procedimento atual.
 */
void SetProcedure(unsigned long proc);

//
// System Server support.
//
int init_systemserver();




/*
 * Classes:
 *     O que segue s�o as fun��es principais do Kernel. S�o seis rotinas 
 * chamadoras de servi�os.
 * 
 * @todo: Esse esquema de rotians de servi�os organizadas em classe precisa 
 * ser replicado para outros sistemas, mesmo em user mode.
 *
 * 1) systemRam
 * 2) systemIoCpu
 * 2) systemIoDma
 * 3) systemDevicesUnblocked
 * 3) systemDevicesBlocked
 * 4) systemThings
 *
 */

void *systemRam( int number, 
                 unsigned long arg1,  
		         unsigned long arg2,  
		         unsigned long arg3,  
		         unsigned long arg4 );
void *systemIoCpu( int number, 
                   unsigned long arg1,  
		           unsigned long arg2,  
		           unsigned long arg3,  
		           unsigned long arg4 );
void *systemIoDma( int number, 
                  unsigned long arg1,  
		          unsigned long arg2,  
		          unsigned long arg3,  
		          unsigned long arg4 );
void *systemDevicesUnblocked( int number, 
                              unsigned long arg1,  
		                      unsigned long arg2,  
		                      unsigned long arg3,  
		                      unsigned long arg4 );
void *systemDevicesBlocked( int number, 
                            unsigned long arg1,  
		                    unsigned long arg2,  
		                    unsigned long arg3,  
		                    unsigned long arg4 ); 
void *systemThings( int number, 
                    unsigned long arg1,  
		            unsigned long arg2,  
		            unsigned long arg3,  
		            unsigned long arg4 );
					
void *systemNull();  //Metodo nulo.
	
/*
 * systemLinkDriver:
 *     Linkando um driver ao sistema operacional
 */
void *systemLinkDriver(unsigned long arg1, unsigned long arg2, unsigned long arg3);

/*
 * systemShowDevicesInfo:
 *     Mostrar informa��es sobre o sistema, seguindo a ordem de
 *     velocidade dos dispositivos e barramentos.
 *     Ex: CPU, Mem�ria, PCIE (video), South Bridge, Super Io ...
 *     Apresentar em ordem alfab�tica.
 */
void systemShowDevicesInfo();
 
void *systemCreateSystemMenuBar();  //Cria a barra de menu do sistema.

/*
 * systemStartUp:
 *     Inicializa��o do sistema.
 */
int systemStartUp();

/*
 * systemCheck3TierArchitecture:
 *     ?? Checa as servi�os oferecids nas 3 camadas.
 */
void systemCheck3TierArchitecture();
void systemSetupVersion();  //version

/*
 * system_dispatch_to_procedure:
 *     Despacha uma mensagem para o procedimento de janela do sistema.
 *     Obs: Esse procedimento fica no kernel base em procedure.c
 */
int system_dispatch_to_procedure( struct window_d *window, 
                                int msg, 
								unsigned long long1, 
								unsigned long long2);
/*
 * SystemMenu: System Menu para manipular a janela ativa.
 * Interface que chama o control menu da janlea ativa.
 */
int SystemMenu();
/*
 * SystemMenuProcedure:
 *     O procedimento do Control Menu principal.
 *     Menu do sistema, manipula a janela ativa.
 */																
unsigned long SystemMenuProcedure( struct window_d *window, 
                                   int msg, 
								   unsigned long long1, 
								   unsigned long long2);
void systemReboot();	    //systemReboot: Reboot stuff.						   
void systemShutdown();      //systemShutdown: Shut down stuff.
//Chamar a fun��o de 32 bit herdado do BM.
void systemShutdownViaAPM(); 
//Pega informa��es de medida de elementos do sistema.
void *systemGetSystemMetric(int number);
//Pega informa��es de status de elementos do systema.
void *systemGetSystemStatus(int number);
int systemInit();     //Inicializador.
void systemSystem();  //Construtor.

			
//
// Fim.
//
