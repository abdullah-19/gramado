/*
 * File: pipe.c
 *     Pipe support for klibc.
 *     ?? Pipes, socketpairs and FIFOs ??
 *     ?? pipe() and mkfifo(). ??
 *     pipe() which "returns" two file descriptors. 
 *     Internally the pipe is (normally) a circular buffer/queue.
 */


#include <kernel.h>


// Duplicate a file stream.
int sys_dup ( int oldfd ){

	file *f_old;
	file *f_new;

	struct process_d *Process;

    int i;
    int slot = -1;	


	Process = (void *) processList[current_process];
	
	if ( (void *) Process == NULL ){
		return -1;

	}else{
	
	     if ( Process->used != 1 || Process->magic != 1234 )
		 {
		     return -1;
		 }
		
		 //ok
	};
	
	
	for ( i=3; i< NUMBER_OF_FILES; i++ )
	{
	    if ( Process->Objects[i] == 0 )
		{
			//reserva.
			Process->Objects[i] = 216;
			
		    slot = i;
			break;
		}
	}	
	
	
	if ( slot == -1 ){
		Process->Objects[i] = (unsigned long) 0;
	    return -1;
	}	
	

    // #todo: 
    // Filtrar oldfd

	f_old = (file *) Process->Objects[oldfd];
	
    if ( (void *) f_old == NULL ){
        Process->Objects[i] = (unsigned long) 0;
        return -1;
 
    }else{
        
		f_new = (void *) kmalloc ( sizeof(file) );
		
		if ( (void *) f_new == NULL ){
		    Process->Objects[i] = (unsigned long) 0;
	        return -1;			
		}


        f_new->used = 1;
		f_new->magic = 1234;	
		
		f_new->_base = f_old->_base;	
		f_new->_p    = f_old->_p;
		
		f_new->_tmpfname = f_old->_tmpfname;
		
		f_new->_lbfsize = f_old->_lbfsize; 
		
		//quanto falta é igual ao tamanho.
		f_new->_cnt = f_old->_cnt; 
		

		Process->Objects[slot] = (unsigned long) f_new;


        return (int) slot;
	}

	// On success, these system calls return the new file descriptor.  
	// On error, -1 is returned, and errno is set appropriately.	
	
fail:
	//errno = ?;
	return -1;
 }


int sys_dup2 (int oldfd, int newfd){

    file *f_old;
    file *f_new;

    struct process_d *Process;



	Process = (void *) processList[current_process];
	
    if ( (void *) Process == NULL ){
		return -1;

	}else{
	
	     if ( Process->used != 1 || Process->magic != 1234 )
		 {
		     return -1;
		 }
		
		 //ok
	};

	
    int slot = newfd;	


    if ( slot == -1 ){
		Process->Objects[slot] = (unsigned long) 0;
	    return -1;
    }	


	//#todo: filtrar oldfd
	
    f_old = (file *) Process->Objects[oldfd];
	
	if ( (void *) f_old == NULL ){
		Process->Objects[slot] = (unsigned long) 0;
	    return -1;
	    
	}else{
        
				
		f_new = (file *) Process->Objects[slot];
		
		if ( (void *) f_new == NULL ){
		    Process->Objects[slot] = (unsigned long) 0;
	        return -1;			
		}

        f_new->used = 1;
        f_new->magic = 1234;			

		f_new->_base = f_old->_base;	
		f_new->_p    = f_old->_p;
		
		f_new->_tmpfname = f_old->_tmpfname;
		
		f_new->_lbfsize  = f_old->_lbfsize; 
		
		//quanto falta é igual ao tamanho.
		f_new->_cnt = f_old->_cnt; 
				
		return (int) slot;
	}


	// On success, these system calls return the new file descriptor.  
	// On error, -1 is returned, and errno is set appropriately.	
	
fail:
	//errno = ?;
    return -1;
 }


int sys_dup3 (int oldfd, int newfd, int flags){
	
	//#todo: flags.

	file *f_old;
	file *f_new;
	
	struct process_d *Process;


	Process = (void *) processList[current_process];
	
    if ( (void *) Process == NULL ){
		return -1;

	}else{
	
	     if ( Process->used != 1 || Process->magic != 1234 )
		 {
		     return -1;
		 }
		
		 //ok
	};

	
    int slot = newfd;	
	
	if ( slot == -1 ) {
		Process->Objects[slot] = (unsigned long) 0;
	    return -1;
	}	
	
	//#todo: filtrar oldfd
	
	f_old = (file *) Process->Objects[oldfd];
	
    if ( (void *) f_old == NULL ){
		Process->Objects[slot] = (unsigned long) 0;
	    return -1;

    }else{

	
        f_new = (file *) Process->Objects[slot];
		
		if ( (void *) f_new == NULL ){
		    Process->Objects[slot] = (unsigned long) 0;
	        return -1;			
		}


        f_new->used = 1;
        f_new->magic = 1234;

		f_new->_base = f_old->_base;	
		f_new->_p = f_old->_p;
		
		f_new->_tmpfname = f_old->_tmpfname;
		
		f_new->_lbfsize = f_old->_lbfsize; 
		
		//quanto falta é igual ao tamanho.
		f_new->_cnt = f_old->_cnt; 
				
		return (int) slot;
	}

	// On success, these system calls return the new file descriptor.  
	// On error, -1 is returned, and errno is set appropriately.	


fail:
	//errno = ?;
	return -1;
 }


/*
 * sys_pipe:
 *     It creates two structures of stream that point 
 * to the same buffer.
 *     It has no name.
 *     It return two file descriptors.
 */

// #bugbug
// Maybe we need just one stream and two pointer.
// So we need to control the reader and the writer.

int sys_pipe ( int *pipefd ){
	
    file *f1;
    file *f2;

    struct process_d *Process;

    int i;
    int slot1 = -1;
	int slot2 = -1;


	Process = (void *) processList[current_process];
	
    if ( (void *) Process == NULL ){
		return -1;

	}else{
	
	     if ( Process->used != 1 || Process->magic != 1234 )
		 {
		     return -1;
		 }
		
		 //ok
	};
	
	//#todo
	//temos que criar uma rotina que procure slots em Process->Streams[]
	//e colocarmos em process.c
	//essa é afunção que estamos criando.
	// process_find_empty_stream_slot ( struct process_d *process );
	
	// procurar 2 slots livres.
	
	// #improvisando
	// 0, 1, 2 são reservados para o fluxo padrão.
	// Como ainda não temos rotinas par ao fluxo padrão,
	// pode ser que peguemos os índices reservados.
	// Para evitar, começaremos depois deles.
	
	for ( i=3; i< NUMBER_OF_FILES; i++ )
	{
	    if ( Process->Objects[i] == 0 )
		{
			//reserva.
			Process->Objects[i] = 216;
			
		    slot1 = i;
			break;
		}
	}	

	for ( i=3; i< NUMBER_OF_FILES; i++ )
	{
	    if ( Process->Objects[i] == 0 )
		{
			//reserva.
			Process->Objects[i] = 216;
			
		    slot2 = i;
			break;
		}
	}	

	if ( slot1 == -1 || slot2 == -1 ) 
	{
		
		Process->Objects[i] = (unsigned long) 0;
		Process->Objects[i] = (unsigned long) 0;
		
	    return -1;
	}
	
	
	// buffer
	
	char *buff = (char *) kmalloc (BUFSIZ);
	//char *buff = (char *) newPage ();
	
    if ( (void *) buff == NULL )
	{
		 Process->Objects[i] = (unsigned long) 0;
		 Process->Objects[i] = (unsigned long) 0;		
	     return -1;
	}
	
	//estruturas 
	f1 = (void *) kmalloc ( sizeof(file) );
	f2 = (void *) kmalloc ( sizeof(file) );
	
	if ( (void *) f1 == NULL || (void *) f2 == NULL ){
		Process->Objects[i] = (unsigned long) 0;
		Process->Objects[i] = (unsigned long) 0;		
	    return -1;

	}else{


		// As duas estruturas compartilham o mesmo buffer.
		
        f1->used = 1;
		f1->magic = 1234;
		f2->used = 1;
		f2->magic = 1234;
			
		f1->_base = buff;	
	    f2->_base = buff;
		f1->_p = buff;
		f2->_p = buff;
		
		f1->_tmpfname = NULL;
		f2->_tmpfname = NULL;
		
		f1->_lbfsize = BUFSIZ; 
		f2->_lbfsize = BUFSIZ; 
		
		//quanto falta é igual ao tamanho.
		f1->_cnt = f1->_lbfsize;   
		f2->_cnt = f2->_lbfsize; 
		
		
		Process->Objects[i] = (unsigned long) f1;
		Process->Objects[i] = (unsigned long) f2;
		
		// #importante
		// Esse é o retorno esperado.
		// Esses índices representam o número do slot
		// na lista de arquivos abertos na estrutura do processo atual.
		
		pipefd[0] = slot1;
		pipefd[1] = slot2; 	
		
		//OK
		return 0;
	};
}


//#todo: rotina provisória
int sys_read_pipe ( int fd, int count ){
    return -1;
}

//#todo: rotina provisória
int sys_write_pipe ( int fd, int count ){
    return -1;
}







