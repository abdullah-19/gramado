/*
 * File: ports.c
 *
 * Descri��o:
 *     Interface de abstra��o para acesso as portas.
 *     A id�ia � que um m�dulo do kernel base possa acessar as portas sem se 
 * preocupar com o tipo de arquitetura. Mas por enquanto estamos apenas 
 * suportando a arquitetura intel.
 *
 *     O acesso as portas tamb�m ser� oferecido na forma de servi�os, para
 * que processos servidores tamb�m possam configurar o hardware diretamente.
 * Logicamente algum tipo de filtro ser� criado, para conferir a permiss�o.
 *
 *     @todo: As rotinas aqui, desviar�o a chamada para para a rotina da 
 * arquitetura atual.
 *
 *    Obs: O HAL deve chamar essas rotinas e n�o os programas.
 *
 *    Obs: Por enquanto a unica arquitetura suportada aqui
 *         � a arquitetura PC x86.
 *
 * Vers�o 1.0, 2015, 2016.
 */



#include <kernel.h>




//inport byte
inline unsigned char kinportb (int port){
	
    unsigned char ret;
 
    asm (" inb %%dx, %%al " : "=a"(ret): "d"(port) );
	
	return (unsigned char) ret;
};


/*
 * koutportb:
 * Escreve um byte em uma determina porta.
 */
inline void koutportb (unsigned int port,unsigned int value){
    
	asm volatile (" outb %%al, %%dx " :: "d" (port), "a" (value) );
};


//inport byte.
inline unsigned char inb (int port){
	
    unsigned char ret;
 
    asm (" inb %%dx, %%al " : "=a"(ret): "d"(port) );
	
	return (unsigned char) ret;
};


//outpot byte
void outb (int port, int data){
    
	asm (" outb %%al, %%dx " :: "a" (data), "d" (port) );
};


inline unsigned char inportb (int port){
	
    unsigned char ret;
 
    asm(" inb %%dx, %%al " : "=a"(ret): "d"(port) );
	
	return (unsigned char) ret;
};


void outportb ( int port, int data ){
	
    asm (" outb %%al, %%dx " :: "a" (data), "d" (port) );
};


int inport8 (int port){
	
	unsigned char ret;
	
    __asm ("inb %%dx, %%al" : "=a" (ret) : "d" (port) );
	
	return (unsigned char) ret;
};


void outport8 (int port, int data){
    
	__asm ("outb %%al, %%dx" :: "a" (data), "d" (port) );
};


int inport16 (int port){
	
    unsigned short ret;
	
    __asm ("inw %%dx, %%ax" : "=a" (ret) : "d" (port) );
	
	return (unsigned short) ret;
};


void outport16 (int port, int data){
    
	__asm ("outw %%ax, %%dx" :: "a" (data), "d" (port) );
};


int inport32 (int port){
	
	unsigned long ret;
    
	__asm("inl %%dx, %%eax" : "=a" (ret) : "d" (port) );
	
	return (unsigned long) ret;
};


void outport32 (int port,int data){ 

    __asm ("outl %%eax, %%dx" :: "a" (data), "d" (port) );
};


/*
 * inportl:
 *     L� um dword de uma determina porta.
 */
inline unsigned long inportl (unsigned long port){
	
	unsigned long ret;

	asm volatile ("inl %%dx,%%eax" : "=a" (ret) : "d"(port) );
	
	return ret;
};


/*
 * outportl:
 *     Escreve um dword em uma determinada porta.
 */
inline void outportl (unsigned long port, unsigned long value){
	
	asm volatile ("outl %%eax,%%dx" :: "d" (port), "a" (value) );
};

//
//  End.
//

