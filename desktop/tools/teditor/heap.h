/*
 * File: heap.h
 *
 * Descri��o:
 *     Suporte ao gerenciamento de heap na api de 32bit.
 *     O prop�sito desse header � dar suporte ao gerenciamento de
 * do heap que ser� usado para a aloca��o de mem�ria para um programa
 * em user mode. Suporte oferecido pela API.
 *     Obs: O kernel poder� usar tanto o heap do processo quanto o heap 
 * do desktop ao qual o processo pertence.
 * 
 * Hist�rico:
 *     Vers�o 1.0, 2016 - Esse arquivo foi criado por Fred Nora.
 */
 
/*
 * Sobre Heap em user mode:
 * =======================
 *     @todo: Quando um processo � inicializado, deve-se criar um Heap de
 * 1MB para ele.
 *     @todo: Fun��es de ger�ncia: heapCreate, heapAlloc, heapFree, heapSize ...    
 *    
 */ 
 
static unsigned long Heap;
static unsigned long ProcessHeap;
static unsigned long DesktopHeap;
//...


//@todo: Criar a estrutura. (copiar do kernel)

//
// Fim.
//

