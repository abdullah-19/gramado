/*
 * File: executive.h 
 *
 * Descri��o:
 *     Rotinas exportadas pelo /executive para os outros 2 m�dulos do kernel base.
 *
 *     Header principal do m�dulo 'executive' do kernel.
 *     Os componentes do m�dulo executive s�o: /sm, /uigm e /uitm.
 *
 *
 *     @todo: Para drivers de rede, protocolos de rede: TCP/IP, FTP, Telnet.
 *
 * Vers�o 1.0, 2015.
 */
 
 
int init_executive();

int executive_gramado_core_init_execve( const char *filename, 
                                        const char *argv[], 
                                        const char *envp[] );

//
// End.
//

