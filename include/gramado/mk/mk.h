/*
 * File: microkernel.h
 *
 */


int microkernelStatus; 
 
 
//
// Prot�tipos.
// 

int jobcontrol( int type, int id, int plane );
int jobcheck( int type, int id );


void microkernelTestLimit();
 
//inicializa o microkernel do kernel.
int init_microkernel();



	
//
// End.
//

