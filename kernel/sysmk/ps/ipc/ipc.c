/*
 * File: ipc/ipc.c 
 *
 * Description:
 *     IPC - ( Inter-Process Comunication ).    
 *     ring 0
 * 
 * History:
 *     2015 - Created by Fred Nora.
 */


// See:
// https://www.drdobbs.com/interprocess-communication-the-l4-micro/184402030
// ...



#include <kernel.h>


// Encapsulation.
int ipcStatus; 




int
ipc_send_to_ws ( struct window_d *window, 
                 int msg, 
                 unsigned long long1, 
                 unsigned long long2 )
{

    struct process_d *__p;

    // #todo
    // Se o ws est� rodando, ent�o mandaremos a mensagem para 
    // a a fila de mensagem dele.
        
    // #isso funcionou.
    // o ws recebeu a mensagem de teclado.
    // E � r�pido.

    if ( (void *) CurrentDesktop == NULL )
        return -1;
        
  
    if ( CurrentDesktop->desktopUsed != 1 || 
         CurrentDesktop->desktopMagic != 1234 )
    {
        return -1;
    }

    
    // PID
    // Ainda n�o sabemos se esse PID � v�lido.
    if (CurrentDesktop->ws <= 0)
        return -1;   
    
    
    __p = (struct process_d *) processList[ CurrentDesktop->ws  ];
                     
    __p->control->window_list[ __p->control->tail_pos ] = window;
    __p->control->msg_list[ __p->control->tail_pos ] = msg;
    __p->control->long1_list[ __p->control->tail_pos ] = long1;
    __p->control->long2_list[ __p->control->tail_pos ] = long2;
        
    __p->control->tail_pos++;
    if ( __p->control->tail_pos >= 31 )
        __p->control->tail_pos = 0;
        
        
    //ok    
    return 0;
}







/*
 *********************
 * pty_send_message_to_process:
 * 
 *     Sends a message to a control thread of a given process.
 */

void ipc_send_message_to_process ( unsigned long msg_buffer, int pid ){

    struct process_d *p;
    struct thread_d *t;

	//#debug
	//printf ("ipc_send_message_to_process: PID=%d \n", pid );
	//refresh_screen ();

    if ( pid < 0 || pid >= PROCESS_COUNT_MAX ){
        // #debug
        printf ("ipc_send_message_to_process: PID \n");
        refresh_screen ();
        return;
    }


    p = ( void *) processList[pid];

    if ( (void *) p == NULL ){
        // #debug
        printf ("ipc_send_message_to_process: struct \n");
        refresh_screen ();
        return;

    }else{
	
		 if ( p->used != 1 || p->magic != 1234 )
		 {
		     // #debug
			 printf ("ipc_send_message_to_process: p validation \n");
		     refresh_screen ();
			 return;
		 }
		
		 //
		 // Thread de controle.
		 //
		
		 t = p->control; 

		 if ( (void *) t == NULL )
		 {
		     // #debug
			 printf ("ipc_send_message_to_process: t struct \n");
		     refresh_screen ();
			 return;

		 }else{

            if ( t->used != 1 || t->magic != 1234 ){
                // #debug
                printf ("ipc_send_message_to_process: t validation \n");
                refresh_screen ();
                return;
            }

			//
			// Send message to the control thread.
			//
 
             ipc_send_message_to_thread ( (unsigned long) msg_buffer, 
                 (int) t->tid );   
        }
    };
}


/*
 ****************
 * pty_send_message_to_thread:
 * 
 *     Sends a message to a given thread.
 */

void ipc_send_message_to_thread ( unsigned long msg_buffer, int tid ){

    struct thread_d *t;

    unsigned long *buffer = (unsigned long *) msg_buffer;


	//printf ("ipc_send_message_to_thread: TID=%d \n", tid);
	//refresh_screen ();	

    //
    // TID
    //

    if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
        printf ("ipc_send_message_to_thread: TID \n");
        refresh_screen ();
        return;
    }


	//
	// BUFFER
	//


	// #importante
	// Temos que checar o endere�o andes de acess�-lo.

    if ( &buffer[0] == 0 ){
        printf ("ipc_send_message_to_thread: buffer\n");
        refresh_screen ();
        return;

    }else{

        t = (void *) threadList[tid];

        if ( (void *) t != NULL )
        {
            if ( t->used != 1 && t->magic != 1234 ){
                printf ("ipc_send_message_to_thread: validation\n");
                refresh_screen ();
                return;
            }

            t->window_list[ t->tail_pos ] = (struct window_d *) buffer[0];
            t->msg_list[ t->tail_pos ]    = (int) buffer[1];
            t->long1_list[ t->tail_pos ]  = (unsigned long) buffer[2];
            t->long2_list[ t->tail_pos ]  = (unsigned long) buffer[3];
        
            t->tail_pos++;
            if ( t->tail_pos >= 31 )
                t->tail_pos = 0;
        }
    };
}


/*
 * init_ipc:
 */

int ipc_init (void)
{  
    return -1;   //#todo.
}



//
// End.
//

