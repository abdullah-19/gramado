/*
 * File: ipc/ipc.c 
 *
 * Descri��o:
 *     IPCM - Inter-Process Communication Manager.
 *     IPC - Inter-Process Communication.
 *     MB - M�dulos inclu�dos no Kernel Base.
 *     Faz parte do Process Manager, parte fundamental do Kernel Base.
 *
 * Atribui��es:
 *     + Comunica��o entre processos e threads.
 *     + Mensagens.
 *     + �rea de tranfer�ncia.
 *     + Sem�foros. (temos um m�dulo para isso).
 *       ...
 *
 * @todo: Criar estruturas.
 * Obs: Haver� troca de mensagens entre processos e o compartilhamento de dados.
 * V�rios m�todos ser�o usados, entre eles o uso da fila de mensagens na estrutura 
 * da janela do processo ou do processo ou da thread e o uso de �reas compartilhadas
 * de mem�rias.
 * Devemos considerar a sincroniza��o de acesso aos recursos. Para isso esse m�dulo
 * dever� interagir com o gerente de objetos, que estrutura os acessos aos objetos
 * concedendo permiss�es e contabilidade de acessos e utilizadores.
 *
 * History:
 *     2015 - Created by Fred Nora.
 */



#include <kernel.h>


// Encapsulation.
int ipcStatus; 




/*
 * init_ipc:
 */

int ipc_init (void)
{  
    return -1;   //#todo.
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





//
// End.
//

