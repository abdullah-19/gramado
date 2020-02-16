/*
 * File: gws.h 
 * 
 *     Client-side library for Gramado Window Server. 
 * 
 * History:
 *     2020 -  Created by Fred Nora.
 */



// Virtual keys.
#include "vk.h"


// Window Messages.
#include "wm.h"


// Window types.
#include "wt.h"


// Colors.
#include "colors.h"


// Windows.
#include "window.h"


// Events.
#include "events.h"


// Connecting with the ws.
#include "connect.h"


// Read and write from socket.
#include "rw.h"

// ...




//
// =============== TOP ========================
//


// #todo
// Na parte mais baixa desse documento devem ficar as abstraçẽos.
// Elas serão as rotinas de mais alto nível e devem começar com gws_


// System call.
void *gws_system_call ( unsigned long a, 
                        unsigned long b, 
                        unsigned long c, 
                        unsigned long d );



// Initialize the library.
int gws_initialize_library (void);




// Services.
// IN: service number, ...
void *gws_services ( int service,
                     unsigned long arg2,
                     unsigned long arg3,
                     unsigned long arg4 );
                     
                     
// Send message to a process.                     
int
gws_send_message_to_process ( int pid, 
                              struct window_d *window, 
                              int message,
                              unsigned long long1,
                              unsigned long long2 );

// Send message to a thread.
int 
gws_send_message_to_thread ( int tid, 
                             struct window_d *window, 
                             int message,
                             unsigned long long1,
                             unsigned long long2 );                  
                        




// Talvez vamos retonar o descritor
// dado pelo servidor.
void *gws_create_window ( unsigned long type,        //1, Tipo de janela (popup,normal,...)
                          unsigned long status,      //2, Estado da janela (ativa ou nao)
                          unsigned long view,        //3, (min, max ...)
                          char *windowname,          //4, Título.                          
                          unsigned long x,           //5, Deslocamento em relação às margens do Desktop.                           
                          unsigned long y,           //6, Deslocamento em relação às margens do Desktop.
                          unsigned long width,       //7, Largura da janela.
                          unsigned long height,      //8, Altura da janela.
                          struct window_d *pWindow,  //9, Endereço da estrutura da janela mãe.
                          unsigned long onde,        //10, Ambiente.( Est� no desktop, barra, cliente ...)
                          unsigned long clientcolor, //11, Cor da área de cliente
                          unsigned long color );     //12, Color (bg) (para janela simples).




// #bugbug:
// Nessa biblioteca devem ficar apenas as rotinas
// Que lidam com o window server.
void gws_reboot (void);







