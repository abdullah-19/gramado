/*
 * File: tasks.c
 *
 * Descri��o:
 *     Algumas Threads em ring 0. Chamadas de task.
 *     Sa�o usadas na configura��o da TSS em headlib.s.
 *     PB - Processos inclu�dos no Kernel Base.
 *     Faz parte do Process Manager, parte fundamental do Kernel Base.
 *    OBS: @todo: Todas as rotinas abaixo podem ser threads de um mesmo processo
 *                do tipo PB, ring 0.
 *
 * Obs: O conceito de tasks est� meio bagun�ado aqui. Nesse momento, task sigifica
 * uma pequena thread em ring 0 escrita dentro do kernel base.
 *
 * Vers�o: 1.0, 2015.
 */


#include <kernel.h>


/*
 * Threads em kernel mode inclu�da no kernel base. Essa threads dever�o pertencer ao 
 * ao processo Kernel.
 */
 
 
void task0 (void)
{
//    printf("Thread started\n"); 	
    while(1){ asm("hlt"); }
};


void task1 (void)
{
//    printf("Thread started\n"); 	 	
    while(1){ asm("hlt"); }
};






//
// End.
//

