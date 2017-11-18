/*
 * File: ram\bank.h 
 *
 * Descri��o:
 *     Gerente de banco de dados em mem�ria e em disco.
 */


typedef enum {
    banktypeLDB,    // LDB - Local Data Base.   (private)
    banktypeGDB,    // GDB - Global Data Base.  (shareable)
    banktypeKDB,    // KDB - Kernel Data Base.  (kernel only)
	banktypeFDB,    // FDB - Free Data Base.    (free memory)
}bank_type_t;


typedef struct bank_d bank_t;
struct bank_d
{
	//Object.
	object_type_t objectType;
	object_class_t objectClass;

	int used;
	int magic;

	bank_type_t type;
	//...

	struct user_info_d *user;    // Que usu�rio � dono desse banco.

	//
	// * Listas de contas nesse banco.
	// Obs: 
	//     As listas ser�o colocadas em um banco de acordo com o tipo 
	// de banco e tipo de conta.
	//
	struct aspace_d *aspaceListHead;
	struct dspace_d *dspaceListHead;	


	// Lista de processos utilizando esse objeto.
	struct process_d *processListHead;
	//struct process_d *currentProcess;
	//int processCount;

    //...	
	
	//Navega��o.
	struct bank_d *Next;  
};
bank_t *Bank;
//bank_t *KernelBank;
//...

