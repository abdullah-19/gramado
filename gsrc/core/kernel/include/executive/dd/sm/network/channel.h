/*
 * Arquivo: channel.h 
 *
 * Descri��o:
 *     Header for channel manager.
 *     Channel manager. 
 *     Gerenciamento de canais Client/Server para troca de mensagens entre 
 * processos.
 *    " Um canal de comunica��o � composto de dois soquetes, um para o cliente 
 * e outro para o servidor. "
 *
 * Vers�o 1.0, 2015, 2016.    
 */



/*
#define  LISTEN_CHANNEL 0x1
#define MESSAGE_CHANNEL 0x2

typedef enum _ECHANNEL_STATE {
    ClientIdle,
    ClientSendWaitReply,
    ClientShutdown,
    ServerIdle,
    ServerReceiveMessage,
    ServerShutdown
} ECHANNEL_STATE;
*/



/*
 * Estrutura para canal.
 *
 */
typedef struct channel_d channel_t;
struct channel_d
{
	object_type_t objectType;
	object_class_t objectClass;
	
    int Used;
	int Magic;
	int Type;
	
    int State;    //Aberto ou fechado.
    int InUse;    //Em uso ou n�o.	
	
	//
	// @todo: essas estruturas est�o esquisitas , o formato deveria ser process_d.
	//
	
    //struct d_process *OwnerProcess;
    //struct d_thread *ClientThread;
    //struct d_thread *ServerThread;
	
    struct process_d *OwnerProcess;
    struct thread_d *ClientThread;
    struct thread_d *ServerThread;
	
	
	//Os dois soquetes do canal de comunica��o.
	struct sockets_d *clientSocket;
	struct sockets_d *serverSocket;
    
	//struct channel_d *severchannel;
};
channel_t *defaultChannel;
channel_t *CurrentChannel;
//...


unsigned long channelList[32];

//
// Prototypes.
//

void *CreateChannel(struct process_d *OwnerProcess,
				    struct thread_d *ClientThread,
				    struct thread_d *ServerThread);
					
int DestroyChannel(struct channel_d *channel);

int OpenChannel(struct channel_d *channel, 
                struct process_d *OwnerProcess,
				struct thread_d *ClientThread,
				struct thread_d *ServerThread);
				
int CloseChannel(struct channel_d *channel);



//
// Fim.
//

