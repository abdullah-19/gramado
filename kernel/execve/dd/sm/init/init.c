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


//Onde ficam os c�digos e arquivos de configura��o usados na inicializa��o.
//A ideia � que se a inicializa��o precisar de algum arquivo, deve procur�-lo
//nos diret�rios previamente combinados.
//melhor que sejam poucos e que sempre sejam os mesmos.

#define INIT_ROOT_PATH  "root:"
#define INIT_BIN_PATH   "root:/volume2/bin"
//...


//externs
//@todo:
//isso dever� ser definido em /gramado.
//extern char *GramadoSystemRoot;
//extern char *GramadoMajorVersion;
//extern char *GramadoMinorVersion;
//extern char *GramadoVersionString;
//...


//Vari�veis internas.
//int initStatus;
//...


/*
 * save_kernel_args:
 *     Salvando em estrutura os argumentos recebidos.
 */
void save_kernel_args (){
	
    //KernelArgs.arg1 = ;
    //KernelArgs.arg2 = ;
	//KernelArgs.arg3 = ;
	//KernelArgs.arg4 = ;
	
    //return;
};


/*
 * boot:
 *     @todo: Prop�sito ??
 *            Boot de alguma parti��o.(volume) 
 *            Boot do volume sinalizado.
 *            Identificar volumes.
 */
void boot (){
    
	panic("sm-init-boot:");	
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
int init_arquitecture_dependent (){
	
	int Status = 0;
	unsigned char Type;
	
	//
	// Fase. (Verificar se essa rotina foi chamada na fase certa de inicializa��o.)
    //
	
	if ( KeInitPhase != 1 ){
		panic("sm-init-init_arquitecture_dependent: KeInitPhase\n");	
	}


	// Os par�metros de tela dependem das propriedades de hardware
	// como monitor e placa de v�deo.
	
	screenInit();

    //printf("init_arquitecture_dependent: #Debug");
    //refresh_screen();
    //while(1){};		
	
	
	//
	// * A estrutura para informa��es sobre o processador. 
	//
	
	// Check structure.
	if ( (void *) processor == NULL ){
	    panic("sm-init-init_arquitecture_dependent: processor\n");
    }
	
	// Sonda pra ver qual � a marca do processador.
	// @todo: � a segunda vez que fazemos a sondagem ?!
	
	Type = (int) hal_probe_processor_type();
	
	//Type.
	processor->Type = (int) Type;
	
	
	//
	//Inicializa de acordo com o tipo de processador..
	//
	
	switch (Type)
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
		    panic ("sm-init-init_arquitecture_dependent: default Type\n");
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

//done:

    return (int) Status;
};



/*
 * init_arquitecture_independent:
 *    Rotina de inicializa��o da parte do sistema que � independente da 
 * arquitetura presente. Independente da tipo de processador. 
 * Obs: Essa � a fase 1 de inicializa��o.
 */
int init_arquitecture_independent (){
	
    int Status;
		
    if (KeInitPhase != 0){
		panic("sm-init-init_arquitecture_independent: KeInitPhase\n");
	}; 

	
	// Hal:
#ifdef KERNEL_VERBOSE	
	// Obs: Nesse momento deve haver alguma sondagem de dispositivos,
	//      salvando os par�metros encontrados.
	//#bugbug @todo: Se � hal ... acho que leva em considera��o a arquitetura.
	printf("Initializing Hal..\n");
#endif	
	Status = init_hal();	
	if(Status != 0){
	    panic("sm-init-init_arquitecture_independent: init_hal\n");
	};

	// Microkernel:
#ifdef KERNEL_VERBOSE
	// Obs: O Microkernel lida com informa��es dependentes da arquitetura,
	// por�m inicializa a gerencia de processos e threads e de comunica��o
	//entre processos.
	//#bugbug @todo: Se � microkernel � processo � registrador ... acho que leva em considera��o a arquitetura.
	printf("Initializing Microkernel..\n");
#endif	
	Status = init_microkernel();
	if(Status != 0){
	    panic("sm-init-init_arquitecture_independent: init_microkernel\n");
	};
	
    // Executive:
#ifdef KERNEL_VERBOSE
	// Obs: O executive n�o � t�o dependente da arquitetura, ele �
	//uma camada mais alta, por�m ser� inicializado aqui para
	//efeito de ordem, j� que estamos inicializando os tres m�dulos
	//b�sicos do kernel base nesse momento.
	printf("Initializing Executive..\n");
#endif	
	Status = init_executive();
	if(Status != 0){
	    panic("sm-init-init_arquitecture_independent: init_executive\n"); 
	};
	
	// Gramado:
#ifdef KERNEL_VERBOSE
    printf("Initializing Gramado..\n");
#endif
	Status = init_gramado();
	if(Status != 0){
	    panic("sm-init-init_arquitecture_independent: init_gramado\n"); 
	};
	
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
#ifdef KERNEL_VERBOSE	  
    printf("sm-init-init_arquitecture_independent: init_user_info\n");
#endif
    init_user_info();       //initialize user info structure.	
    
	
	//
	// User Session, Window Station and Desktop.
	// @todo: Essas informa��es s�o independentes da arquitetura,
	//      Essa rotina pode ir pra outro lugar.
	//
	
UserSession:
#ifdef KERNEL_VERBOSE	
    printf("sm-init-init_arquitecture_independent: init_user_session\n");   
#endif    
	init_user_session();    //initialize user session.	 
	
WindowStation:
#ifdef KERNEL_VERBOSE
    printf("sm-init-init_arquitecture_independent: init_window_station\n");   
#endif    
	init_window_station();

Desktop:
#ifdef KERNEL_VERBOSE 
    printf("sm-init-init_arquitecture_independent: init_desktop\n");   
#endif    
	init_desktop(); 
 
	
	// Window manager. - (Inicializa janelas e cria o logon).
	// @todo: Essas informa��es s�o independentes da arquitetura,
	//      Essa rotina pode ir pra outro lugar.
	
WindowManager:	

	//Caso n�o se use gui.
	if(g_useGUI != 1){
	    SetProcedure( (unsigned long) &system_procedure);		
	}else{
        
		// Window manager.
#ifdef KERNEL_VERBOSE	    
		printf("sm-init-init_arquitecture_independent: init_window_manager\n");
#endif	    
		init_window_manager();				
	};
	
	// tty support.
	ttyInit();

	//
	// terminal support.
	//
	
	//init_system_terminal();


    // Continua ...	 
	
done:
#ifdef KERNEL_VERBOSE
    //debug
    printf("Done\n");	
	//refresh_screen();
    //while(1){}
#endif	
    return (int) 0;
};


/*
 **************************************************
 * Init globals: 
 *     Inicia vari�veis globais do Kernel Base.
 *     Obs: Inicializar por categorias.
 */
void init_globals (){

#ifdef KERNEL_VERBOSE	
    printf("sm-init-init_globals:\n");	
#endif

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
	if ( g_useGUI == 1 ){
		
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

		// windows, menus ...
		init_windows();
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
	
	//alocando mem�ria para as estruturas do fluxo padr�o.
	stdin = (void *) malloc( sizeof(FILE) );
	stdout = (void *) malloc( sizeof(FILE) );
	stderr = (void *) malloc( sizeof(FILE) );
	
	//kstdin  = (void*) malloc( sizeof(FILE) );
	//kstdout = (void*) malloc( sizeof(FILE) );
	//kstderr = (void*) malloc( sizeof(FILE) );
	
    //inicializa as estruturas do fluxo padr�o.	
	stdioInitialize();
	
	
	//
	// Keyboard support.
	//
	
	//ldisc_init_modifier_keys();
	//ldisc_init_lock_keys();	
	
	//Continua ...

// Done.
//done:	
    //return;
};


/*
 ******************************************
 * init:
 * Base initializations. (Four phases).
 */ 
int init (){
	
    int Status = 0;
	
	//Check kernel phase.
	if ( KeInitPhase != 0 ){
		printf("sm-init-init: KeInitPhase\n");
        die();		
	}
 
    //Globals.
#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: init_globals\n");     
#endif    
	init_globals();
	
    //Object manager.	
#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: init_object_manager\n");
#endif	
	init_object_manager();
	
	//i/o Manager.
#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: ioInit\n");	
#endif	
	ioInit();
	
	//Disk manager, volume manager and fs manager.
	
#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: disk_init\n");
#endif    
	disk_init();
	
#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: volume_init\n");
#endif    
	volume_init();
	
	
	
#ifdef KERNEL_VERBOSE	
	printf("fsInit: VFS..\n");
#endif
	
	vfsInit();
	
	
	
#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: fsInit\n");
#endif    
	fsInit();
	
		
    //System folders.
    // ??@todo: /ux4 .../ux1	
#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: create_system_folders\n");
#endif	
	create_system_folders(); 
    

#ifdef KERNEL_VERBOSE	
	printf("sm-init-init: initialize_system_message_queue\n");
#endif	
	initialize_system_message_queue(); 
	
    //
	// Initialize Platform structure.
	//
	
#ifdef KERNEL_VERBOSE
	printf("sm-init-init: Platform\n");	
#endif

	Platform = (void *) malloc( sizeof(struct platform_d) );
	
	if( (void *) Platform ==  NULL )
	{
		// # This is the Root struct #
		panic("sm-init-init: Platform\n");	
	
	}else{
		
		//Hardware
	    Hardware = (void *) malloc( sizeof(struct hardware_d) );
	    if( (void *) Hardware ==  NULL ){
		    printf("sm-init-init: Hardware\n");	
	        die();
		}else{
		    Platform->Hardware = (void *) Hardware;
            //printf(".");			
		};
		
		//Firmware
	    Firmware = (void *) malloc( sizeof(struct firmware_d) );
	    if( (void *) Firmware ==  NULL ){
		    printf("sm-init-init: Firmware\n");	
	        die();
		}else{
		    Platform->Firmware = (void *) Firmware;
            //printf(".");  			
		};

		
		//System (software)
	    
		//
		// *IMPORTATE: Aqui estamos inicializando a estrutura do systema.
		//
		
		System = (void *) malloc( sizeof(struct system_d) );
	    if( (void *) System ==  NULL ){
		    printf("sm-init-init: System\n");	
	        die();
		}else{
			
			System->used = 1;    //Sinaliza que a estrutura esta em uso.
			System->magic = 1234; //sinaliza que a estrutura n�o esta corrompida.
			
		    Platform->System = (void *) System;
            //printf(".");			
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
//Logon:

	//
	// Logon. 
    // Cria Background, main window, navigation bar.
    // de acordo com predefini��o.
    //

	if ( g_useGUI == 1 ){
		
#ifdef KERNEL_VERBOSE		
		printf("sm-init-init: Logon\n");
#endif	    
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
//done:
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
