/*
 * File: x86.c
 *
 * Descri��o:
 *     Suporte �s portas na arquitetura x86.
 *
 *     O acesso as portas tamb�m ser� oferecido na forma de servi�os, para
 * que processos servidores tamb�m possam configurar o hardware diretamente.
 * Logicamente algum tipo de filtro ser� criado, para conferir a permiss�o.
 *
 *  @todo: Muita fun��o aqui deveria ter o retorno do tipo void.
 *
 */

 
#include <kernel.h>


// habilita as interrup��es
int enable()
{ 
    asm("sti");

done:
    //@todo: Porque habilitar as interrup��es deveria mudar o valor do registrador eax.
    //talvez deveria ser void o tipo da fun��o.	
    return (int) 0;
};


// desabilita as interrup��es
int disable()
{ 
    asm("cli"); 
	
done:
    //@todo: Porque desabilitar as interrup��es deveria mudar o valor do registrador eax.
    //talvez deveria ser void o tipo da fun��o.		
    return (int) 0;
};


int stopCpu()
{      
    __asm(" cli \n \t " 
		  " hlt ");
};


int intReturn(){ 
    __asm("iret");
};


int farReturn(){
    __asm("lret");
};


int getFlags(int variable) 
{
    __asm(" pushfl \n \t "     
          " popl %0 "         
          : "=r" (variable));
};


int setFlags(int variable)
{
    __asm("pushl %0 \n\t"   
          "popfl"           
          : : "r" (variable));
};


int kernelProcessorInPort8( int port, int data){
    asm("inb %%dx, %%al" : "=a" (data) : "d" (port));
};
    

int kernelProcessorOutPort8(int port, int data){
    __asm("outb %%al, %%dx" : : "a" (data), "d" (port));
};


int kernelProcessorInPort16(int port,int data){ 
    __asm("inw %%dx, %%ax" : "=a" (data) : "d" (port));
};


int kernelProcessorOutPort16(int port, int data){
    __asm("outw %%ax, %%dx" : : "a" (data), "d" (port));
};


int kernelProcessorInPort32(int port, int data){
    __asm("inl %%dx, %%eax" : "=a" (data) : "d" (port));
};


int kernelProcessorOutPort32(int port,int data){ 
    __asm("outl %%eax, %%dx" : : "a" (data), "d" (port));
};


int Push(int value){
    __asm("pushl %0" : : "r" (value) : "%esp");
};


int Pop(int variable){
    __asm("popl %0" : "=r" (variable) : : "%esp");
};


int pushRegs(){ 
    __asm("pushal" : : : "%esp");
};


int popRegs(){ 
    __asm("popal" : : : "%esp");
};


int pushFlags(){ 
    __asm("pushfl" : : : "%esp");
};


int popFlags(){ 
    __asm("popfl" : : : "%esp");
};


int getStackPointer(int addr){
    __asm("movl %%esp, %0" : "=r" (addr));
};


int setStackPointer(int addr){
    __asm("movl %0, %%esp" : : "r" (addr) : "%esp");
};

//
// End.
//

