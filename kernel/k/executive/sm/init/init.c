/*
 * File: init.c 
 *
 * Descri��o:
 *     Arquivo principal do m�dulo INIT kernel.
 *     Faz inicializa��es.
 *     Esse m�dulo est� em kernel base.
 *
 * In this file:
 *    +init
 *    +init_test_disk
 *    +init_video
 *    +init_globals
 *
 * Hist�rico:
 *     Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o 1.0, 2016 - Aprimoramento geral das rotinas b�sicas.
 *     //...
 */ 

 
#include <kernel.h>


//Vari�veis internas.
//int initStatus;
//...


/*
 * save_kernel_args:
 *     Salvando em estrutura os argumentos recebidos.
 */
void save_kernel_args()
{
    //KernelArgs.arg1 = ;
    //KernelArgs.arg2 = ;
	//KernelArgs.arg3 = ;
	//KernelArgs.arg4 = ;
	
    return;
};


/*
 * boot:
 *     @todo: Prop�sito ??
 *            Boot de alguma parti��o.(volume) 
 *            Boot do volume sinalizado.
 *            Identificar volumes.
 */
void boot(){    
	panic("boot:");	
};	


/*
 * init_arquitecture_dependent:
 *     Rotina de inicializa��o dependente da arquitetura atual da m�quina.
 *     A fase 1 identificou o tipo de processador. Aqui chamaremos a rotina 
 * de inicializa��o de acordo com o tipo identificado.
 *     A fase 1 inicializou a estrutura processor. Aqui checamos se ela � 
 * v�lida.
 *  GDT, IDT, ... TR ...
 *
 * Obs: Essa � a fase 2 de inicializa��o.
 *
 * Obs: Dependente significa dependente da marca do procesador.
 */
int init_arquitecture_dependent()
{
	int Status = 0;
	unsigned char Type;
	
	//
	// Fase. (Verificar se essa rotina foi chamada na fase certa de inicializa��o.)
    //
	
	if(KeInitPhase != 1){
		printf("init_arquitecture_dependent:");
        refresh_screen();
        while(1){};		
	};


	//
	// Os par�metros de tela dependem das propriedades de hardware
	// como monitor e placa de v�deo.
	//
	
	screenInit();

    //printf("init_arquitecture_dependent: #Debug");
    //refresh_screen();
    //while(1){};		
	
	
	//
	// * A estrutura para informa��es sobre o processador. 
	//
	
	// Check structure.
	if( (void*) processor == NULL ){
	    printf("init_arquitecture_dependent fail: Structure.");
		refresh_screen();
		while(1){}
    };
	
	//
	// Sonda pra ver qual � a marca do processador.
	// @todo: � a segunda vez que fazemos a sondagem ?!
	//
	
	Type = (int) hal_probe_processor_type();
	
	//Type.
	processor->Type = (int) Type;
	
	
	//
	//Inicializa de acordo com o tipo de processador..
	//
	
	
	switch(Type)
	{
	    //Intel. (pega os par�metros do processador intel e coloca na estrutura).
	    case Processor_INTEL: 
		    init_intel(); 
			break;

        //AMD. (pega os par�metros do processador amd e coloca na estrutura).			
	    case Processor_AMD: 
		    init_amd(); 
			break;

        //Continua ...
			
		//@todo: Aqui � um erro fatal.	
		default:
		    printf("init_arquitecture_dependent fail: Processor type.");
			refresh_screen();
			while(1){};
		    //KeAbort();
            break;		
	};
	
	
	//
	// @todo: GDT, IDT (usar extern).
	//	
	
	
	//Inicializando o Process manager.
	init_process_manager();
	
 //
 // Continua ...
 //
	
//
// Done.
//
done:
    return (int) Status;
};



/*
 * init_arquitecture_independent:
 *    Rotina de inicializa��o da parte do sistema que � independente da 
 * arquitetura presente. Independente da tipo de processador. 
 * Obs: Essa � a fase 1 de inicializa��o.
 */
int init_arquitecture_independent()
{
    int Status;
		
    if(KeInitPhase != 0){
		printf("init_arquitecture_independent error: Phase..\n");
        refresh_screen();
        while(1){};		
	}; 

	// Hal:
	// Obs: Nesse momento deve haver alguma sondagem de dispositivos,
	//      salvando os par�metros encontrados.
	printf("Initializing Hal..\n");	
	Status = init_hal();	
	if(Status != 0){
	    printf("init_arquitecture_independent error: Hal.\n");
		KiAbort(); 
	};

	// Microkernel:
	// Obs: O Microkernel lida com informa��es dependentes da arquitetura,
	// por�m inicializa a gerencia de processos e threads e de comunica��o
	//entre processos.
	printf("Initializing Microkernel..\n");	
	Status = init_microkernel();
	if(Status != 0){
	    printf("init_arquitecture_independent error: Microkernel.\n");
		KiAbort(); 
	};
	
    // Executive:
	// Obs: O executive n�o � t�o dependente da arquitetura, ele �
	//uma camada mais alta, por�m ser� inicializado aqui para
	//efeito de ordem, j� que estamos inicializando os tres m�dulos
	//b�sicos do kernel base nesse momento.
	printf("Initializing Executive..\n");	
	Status = init_executive();
	if(Status != 0){
	    printf("init_arquitecture_independent error: Executive.\n");
		KiAbort(); 
	};
	
	
	//@todo:
	//Status = init_gramado();
	
	//
	// User Info:
	// =========
	//
	// @todo: Usu�rio e sess�o devem ser independentes do modo de v�deo. 
	//        Text ou Graphics.
	//

	//
	// @todo: Essas informa��es s�o independentes da arquitetura,
	//      Essa rotina pode ir pra outro lugar.
	//
UserInfo:	  
    printf("init_arquitecture_independent: User..\n");
    init_user_info();       //initialize user info structure.	
    
	
	//
	// User Session, Window Station and Desktop.
	// @todo: Essas informa��es s�o independentes da arquitetura,
	//      Essa rotina pode ir pra outro lugar.
	//
	
UserSession:	
    printf("init_arquitecture_independent: User Session..\n");   
    init_user_session();    //initialize user session.	 
	
WindowStation:
    printf("init_arquitecture_independent: Window Stations..\n");   
    init_window_station();

Desktop: 
    printf("init_arquitecture_independent: Desktops..\n");   
    init_desktop(); 
 
	//
	// Window manager. - (Inicializa janelas e cria o logon).
	// @todo: Essas informa��es s�o independentes da arquitetura,
	//      Essa rotina pode ir pra outro lugar.
	//
	
WindowManager:	

	//Caso n�o se use gui.
	if(g_useGUI != 1){
	    SetProcedure( (unsigned long) &system_procedure);		
	}else{
        
		// Window manager.
	    printf("init_arquitecture_independent: Initializing window manager..\n");
	    init_window_manager();				
	};
	
 
	
    // Continua ...	 
	
	//
	// tty support.
	//
	
	ttyInit();
	
done:
    //debug
    printf("Done!\n");	
	//refresh_screen();
    //while(1){}
	
    return (int) 0;
};


/*
 * Init globals: 
 *     Inicia vari�veis globais do Kernel Base.
 *     Obs: Inicializar por categorias.
 */
void init_globals()
{
	//Atalho para a pr�xima mensagem de teclado..(test) debug
	gNextKeyboardMessage = (int) 0;
	
	//Essa flag bloqueia e impede que uma janela obtenha o foco.
	gFocusBlocked = (int) 0;
	
	//
	// As globais relativas � usu�rio s�o independentes do ambiente gr�fico.
	//
	
	//Group, User.
	current_group = 0;
	current_user  = 0;
	
	//User Session, Window Station, Dasktop.
	current_usersession = 0;
	current_windowstation = 0;
	current_desktop = 0;
	
	//Process, Thread.
	current_process = (int) 0;
	current_thread  = (int) 0;	   
	
	// Kernel.
	kernel_request = 0;
	
	// System.
	// @todo:
	
	
	//Globais no ambiente GUI.
	if(g_useGUI == 1)
	{		        
		//Pr�ximo procedimento de janela.
	    g_next_proc = (unsigned long) &system_procedure; 
		
		//Windows globals.
		current_usersession = (int) 0;    //>Current User Session.
		current_windowstation = (int) 0;  //>>Current Window Station.
	    current_desktop = (int) 0;        //>>>Current Desktop.
		current_window = (int) 0;         //>>>>Current Window.
	    windows_count = (int) 0;          //Window count.
	    window_with_focus = (int) 0;      //Window with focus.
		current_menu = (int) 0;           //Current Menu.
        //Continua...	

		//Inicializa globais relativas � janelas.
		init_windows();
	    
        //Menus
        init_menus();	
	
        //Continua...		
	};
		
	
	//Messages.
	g_nova_mensagem = 0;
	g_new_message = 0;
	
    //FS type.	
	g_file_system_type = FS_TYPE_FAT16;	//tipo 1, fat16.
	fatbits = (int) 16;
	
	//Outros.
	errno = 0;
	
	
	//Inicializando as estruturas do fluxo padr�o.
	stdin   = (void*) malloc( sizeof(FILE) );
	stdout  = (void*) malloc( sizeof(FILE) );
	stderr  = (void*) malloc( sizeof(FILE) );
	kstdin  = (void*) malloc( sizeof(FILE) );
	kstdout = (void*) malloc( sizeof(FILE) );
	kstderr = (void*) malloc( sizeof(FILE) );
	
	
	//Continua ...

// Done.
done:	
    return;
};


/*
 * init:
 * Base initializations. (Four phases).
 */ 
int init()
{
    int Status = 0;
	
	if(KeInitPhase != 0){
		printf("init:");
        refresh_screen();
        while(1){};		
	}; 
 
    //Globals.
	printf("init: Initializing globals..\n");     
    init_globals();
	
    //Object manager.	
	printf("init: Initializing Object Manager..\n");
	init_object_manager();
	
	//i/o Manager.
	printf("init: Initializing i/o manager..\n");	
	ioInit();
	
	//Disk manager, volume manager and fs manager.
	printf("init: Initializing disk manager..\n");
    disk_init();
	printf("init: Initializing volume manager..\n");
    volume_init();
	printf("init: Initializing file system manager..\n");
    fsInit();
	
    //System folders.	
	printf("init: Initializing system folders..\n");
	create_system_folders(); 
    
	//Shell. (O Shell do kernel base).
	//printf("init: Initializing kernel shell variables ...\n");	
	//init_shell();
	
    //
	// Initialize Platform structure.
	//
	
	printf("init: Initializing Platform Struct..");	
	
	Platform = (void*) malloc( sizeof(struct platform_d) );
	if( (void*) Platform ==  NULL ){
		printf("init error: Platform Struct!\n");	
	    refresh_screen();
		while(1){};
	}
	else
	{
		//Hardware
	    Hardware = (void*) malloc( sizeof(struct hardware_d) );
	    if( (void*) Hardware ==  NULL ){
		    printf("init error: Hardware Struct!\n");	
	        refresh_screen();
		    while(1){};
		}else{
		    Platform->Hardware = (void*) Hardware;
            printf(".");			
		};
		
		//Firmware
	    Firmware = (void*) malloc( sizeof(struct firmware_d) );
	    if( (void*) Firmware ==  NULL ){
		    printf("init error: Firmware Struct!\n");	
	        refresh_screen();
		    while(1){};
		}else{
		    Platform->Firmware = (void*) Firmware;
            printf(".");  			
		};

		
		//System (software)
	    
		//
		// *IMPORTATE: Aqui estamos inicializando a estrutura do systema.
		//
		
		System = (void*) malloc( sizeof(struct system_d) );
	    if( (void*) System ==  NULL ){
		    printf("init error: System Struct!\n");	
	        refresh_screen();
		    while(1){};
		}else{
			
			System->used = 1;    //Sinaliza que a estrutura esta em uso.
			System->magic = 1234; //sinaliza que a estrutura n�o esta corrompida.
			
		    Platform->System = (void*) System;
            printf(".");			
		};
		
		//printf(" Done!\n");	
		//...
	};

	
	//Fase 1: Inicia a parte independente da arquitetura.
	Status = (int) init_arquitecture_independent();
	if(Status != 0){
	   //Nothing for now.
	};
	KeInitPhase = 1;
    
	//Fase 2: Inicia a parte de arquitetura especifica da m�quina atual.
	//        Ou seja, considera a marca do processador.
    Status = (int) init_arquitecture_dependent();	 
    if(Status != 0){
	    //Nothing for now.
	};
	KeInitPhase = 2;

	
	//
	// Logon.
	//
	
	//Fase3: Logon. 
Logon:
	//
	// Logon. 
    // Cria Background, main window, navigation bar.
    // de acordo com predefini��o.
    //

	if(g_useGUI == 1){
		printf("init: Initializing Logon structs..\n");
	    create_logon();
		init_logon(0,0);    //Libera. (Aceita argumentos).

        //Obs: *IMPORTANTE Usa-se o procedimento de janela do Logon.		
	};	
	KeInitPhase = 3; 
			
	// Continua ...
	

// * Fase 3:
// *     classe system.device.unblocked.
// *	   @todo: Inicializar dispositivos LPC/super io.
// *            Keyboard, mouse, TPM, parallel port, serial port, FDC. 
// *
 //* Fase 4:
 //*     classe system.device.unblocked.
 //*     @todo: Dispositivos PCI, ACPI ...
 //*
	
	
// Done.
done:
    //printf("Done!\n");	
	//refresh_screen();
	//@todo: Deve retornar a vari�vel Status.
    return (int) 0;  
};


/*
int initInit()
{
	
}
*/

//
// End.
//
