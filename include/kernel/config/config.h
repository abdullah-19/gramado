/*
 * File: config.h
 *     
 *      Setup compiling stuff.
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_


#define FRED_VHD_32MB

#if	defined(FRED_VHD_32MB)
#define VHD_32MB_CHS { 862, 4, 19,  0 }  
//#define VHD_32MB_CHS { 0x35E, 0x4, 0x13, 0 }  
#else 
#error "must define vhd"	
#endif


/* configuração de compilação */

// Algumas adaptações precisam ser feitas 
// para que as ferramentas de compilação 
// sejam usadas corretamente.


// gcc version:
// gcc 8 on manjaro

// #importante:
// O kernel tem três possíveis processos usados na inicialização.
// #obs: cada um deles tem sua própria estrutura de processo e suas 
// próprias threads, mas até o momento todos os três estão usando 
// o diretório de páginas do processo kernel.  
// Com essas flags podemos habilitar qual dos três processos 
// irão rodar na inicialização.
// Se a configuração não indicar nenhum processo então o kernel deve 
// falhar na inicialização, por enquanto. 



#define ENTRY_INIT_INIT    1
#define ENTRY_INIT_SHELL   1
#define ENTRY_INIT_TASKMAN 1

//Cria uma thread para o processo kernel, que irá rodar em ring0.
#define ENTRY_INIT_KERNELTHREAD_RING0  1  

//testando carregar uma segunda thread no mesmo endereço virtual.
//dessa forma mais de uma thread rodará no mesmo endeereço virtual,
//mas em processos diferentes.
#define ENTRY_INIT_THREAD_400000 1


//Autoriza rotinas que usam varia'veis importadas 
//de makefiles e arquivos .ld.
#define IMPORTED_VARIABLES 1


//Essa flag aciona a rotina que vai checar o máximo de 
//configurações e inicializações válidas.

#define ENTRY_DEBUG_CHECK_VALIDATIONS 1




//
// Set up what kind of debug message we wanna see.
//

//Esse é usado para exibir somente os marcadores pprincipais,
//que são mesnagens escritas em maiúculas.
//fnciona como uma visão panorâmica, usa espinha dorsal, um esqueleto.
//#define KERNEL_VERBOSE 1
//Esse é usado para trabalhar a rotina de entrypoint da arquitetura alvo.
//#define ENTRY_VERBOSE 1
//Esses quatro são as camadas principais.
//#define HAL_VERBOSE 1
//#define MK_VERBOSE 1
//#define EXECVE_VERBOSE 1
//#define GWS_VERBOSE 1


//
// ## breack points ##
// Set up what what is the breakpoint.
//

//Seriam inicializações parciais programadas. 
 

//#todo
//Criar um breakpoint apo's a sondagem de dispositivos pci.

//#define BREAKPOINT_TARGET_AFTER_VIDEO 1
//#define BREAKPOINT_TARGET_AFTER_SYSTEM 1
//#define BREAKPOINT_TARGET_AFTER_RUNTIME 1
//#define BREAKPOINT_TARGET_AFTER_INIT 1
//#define BREAKPOINT_TARGET_AFTER_LOGON 1
//#define BREAKPOINT_TARGET_AFTER_LOGOFF 1
//#define BREAKPOINT_TARGET_AFTER_HAL 1
//#define BREAKPOINT_TARGET_AFTER_MK 1
//#define BREAKPOINT_TARGET_AFTER_ENTRY 1



//
// ## targets ##
//

//Também pretendo fazer a inicialização mudar de direção dependendo do alvo programado.

//Isso inicializa os três aplicativos do gramado core.
//#define TARGET_GRAMADOCORE_APPS 1

//Isso inicializa apenas o app init do gramado core.
//#define TARGET_GRAMADOCORE_INIT 1

// CONTINUA
//...


#endif 

