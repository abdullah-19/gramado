/*
 * File: microkernel.h
 *
 * Descri��o:
 *     Rotinas exportadas pelo /executive para os outros 2 m�dulos do kernel base.
 *
 *     Header da m�dulo "/microkernel" dentro do kernel base.
 *     Apenas ger�ncia de processos,threads, scheduler, sem�foro, ipc ...
 *
 * @todo: 
 *     Os servi�os oferecidos por esse m�dulo aparecer�o aqui 
 *     come�ando por 'sys_'.
 *
 * Hist�rico:
 *     Version 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 */


int microkernelStatus; 
 
 
//
// Prot�tipos.
// 

void microkernelTestLimit();
 
//inicializa o microkernel do kernel.
int init_microkernel();



	
//
// End.
//

