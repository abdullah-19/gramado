/*
 * File: builtins.c
 *     Buittin commands for Gramado Core Shell.
 *
 * History:
 *     2018 - Created by Fred Nora.
 */

 
#include "shell.h" 


void cd_buitins (){
	
	printf("cd_buitins:\n");
}


//cls
void cls_builtins (){
	
    shellClearScreen ();
}


void copy_builtins()
{	
}


void date_builtins()
{	
}


void del_builtins()
{	
}


void dir_builtins (){
	
	//
	// @todo: get set disk id e directory id.
	//

    printf ("~dir \n");		
        
	//#test 
	//173 Lista arquivos de um diretório, dado o número do disco,
    //o número do volume e o número do diretório,
	
    system_call ( 173, 0, 0, 0 );		
		
	printf ("~done\n");	
}


//echo command.
//imprimir a lista de comandos de uma linha de comandos.

void echo_builtins (char *list[]){
	
    shell_print_tokenList ( list, " ");
	//shell_print_tokenList( list, "/");
}


void exec_builtins (){
	
    printf ("@todo: ~exec\n");	
}


void exit_builtins (){
	
    //printf("~exit\n");
	printf ("Exiting shell process..\n");
	exit (0);	
}


int getpid_builtins (){
	
    shellShowPID ();	
	
	return 0; 
}


int getppid_builtins (){
	
	shellShowPPID ();
	
	return 0;
}


int getuid_builtins (){
	
	shellShowUID ();
	
	return 0;
}


int getgid_builtins (){
	
	shellShowGID ();
	
	return 0;
}


/*
 **************************
 * help_builtins:
 *     Esse comando deve oferecer uma experiência ao usuário. Mostrando
 * os comandos disponíveis para uso.
 *     --tests é a flag que mostra os testes das novas implemetações.
 */

void help_builtins (int arg){
	
	//desabilita o cursor
	system_call ( 245, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);		
	
	switch (arg)
	{
		//shell experience menu.
		case 1:
		    printf ("Show all help topics\n");
			shellShowExperienceMenu ();
		    break;
			
		//shell tests menu.
		case 2:
			shellShowTestsMenu ();
		    //printf("cls, help, exit..\n");
			break;
		
		default:
			shellShowExperienceMenu ();
			break;
			
	};
	
	//reabilita o cursor
	system_call ( 244, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);		
}


void pwd_builtins (){
	
    //printf("~pwd - print working directory\n");
    system_call ( 170, 0, 0, 0 );		
}



