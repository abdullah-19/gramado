/*
 * Aquivo: systemcall.c
 *
 * Estabelece contato como servidor de servi�os do kernel via interrup��o.
 *
 * @todo: 
 *     N�o faz sentido chamar os servi�os do kernel, de dentro dele,
 *     via interrup��o. Essa fun��o pode apenas chamar os servi�os diretamente,
 *     via fun��o.
 *
 */


#include <kernel.h>

/*
 * systemcall:
 *     Chamadas ao sistema. 
 *    (@todo: Essa fun��o nao faz sentido aqui.
 *            N�o precisa chamar o kernel via interrup��o 
 *            de dentro do kernel).
 *   N�o sei se pode chamar interrup��es estando em kernel mode.
 */
int systemcall( unsigned long numero, 
                unsigned long ax, 
				unsigned long bx, 
				unsigned long cx, unsigned long dx)
{
    int ret_val;
	
	unsigned long *int_args = (unsigned long *) 0x00900000;
	
    //Salvando os argumentos.
    int_args[0] = numero;    //arg0 - N�mero do servi�o.
	int_args[1] = ax;        //arg1 ... 
    int_args[2] = bx;        //arg2
	int_args[3] = cx;        //arg3
	int_args[4] = dx;        //arg4
		
    //chamando a interrup��o
    asm volatile( "int %1\n" 
	              : "=a"(ret_val) 
				  : "i"(200), "a"(ax), "b"(bx), "c"(cx), "d"(dx) ); 
//Done.
done:
	return ret_val;
};





/*
 * jmp_address:
 *     Transfere o comando para alguma rotina. Sem volta.
 *     @todo: Check if we got a prototype.
 */
int jmp_address( unsigned long arg1, 
                 unsigned long arg2, 
				 unsigned long arg3, 
				 unsigned long arg4 )  
{       
    asm(" pushl %0" :: "r" (arg4) : "%esp");    //Window.
    asm(" pushl %0" :: "r" (arg3) : "%esp");    //Msg.
    asm(" pushl %0" :: "r" (arg2) : "%esp");    //Long1.       
    asm(" pushl %0" :: "r" (arg1) : "%esp");    //Long2.
	 
	asm(" jmpl *_next_address ");           
	 	

    //    
	// no return !
	//
	
	//@todo: essa fun��o deve ser do tipo void.
	
	return (int) 0;
};  

//
//fim.
//
