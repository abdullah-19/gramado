/*
 * File: fs\search.c 
 * 
 * Descri��o:
 *     Rotinas de procura de arquivos, nomes...
 * Hist�rico:
 *    Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *    Vers�o 1.0: 2016 - Revis�o.
 *    ...
 */


#include <kernel.h>


/*
 **********************************************************************
 * KiSearchFile:
 *    Procura por um arquivo, dado o endere�o do diret�rio.
 *    Com o diret�rio raiz j� carregado na mem�ria.
 *
 *   file_name: Nome do arquivo.
 *     address: Endere�o na mem�ria onde o diret�rio est�.
 *
 * @todo: 
 *    Atender � especifica��es diferentes de sistemas de arquivos, como:
 *    +Tamanho do cluster
 *    +Tamanho do disco
 *    +Tipo de sistema de arquivos. 
 *    +...
 *
 */ 
int KiSearchFile( unsigned char *file_name, 
                  unsigned long address )
{
	int Status;
    unsigned long i = 0;        //Deslocamento do dir. 
    unsigned long j = 0;        //Deslocamento no nome.
    unsigned long max = 512;    //N�mero m�ximo de entradas no diret�rio.	
    unsigned char *dir = (unsigned char *) address;

    //
    // Address Limits:
	// Endere�o de mem�ria onde o diret�rio est�.
    //
	
	if(address == 0){
	    return (int) 1;
	}; 	
	
	//Compare.
    while(i < max)
    {     
        if(dir[j] != 0)
		{
		    Status = strncmp( file_name, &dir[j], 11 );
			if(Status == 0){ 
			    goto done; 
			}; 
        };   
		
		//Pr�xima entrada. Repete 512 vezes.
        j += 0x20;    
        ++i;          
    };
	
	//More?!
	//fsSearchFile(unsigned char *file_name)
	
fail:
    printf("fs-search-KiSearchFile: File not found!\n");       	
	return (int) 1; 
done:
    return (int) 0;
};



/*
 **********************************************************************
 * fsSearchFile:
 *    Procura por um arquivo no diret�rio raiz de uma parti��o.
 *    Com o diret�rio raiz j� carregado na mem�ria.
 *
 * @todo: 
 *    Atender � especifica��es diferentes de sistemas de arquivos, como:
 *    +Tamanho do cluster
 *    +Tamanho do disco
 *    +Tipo de sistema de arquivos. (nao por enquanto)
 *    +...
 */
//int fsSearchFile( const char *name ) 
int fsSearchFile(unsigned char *file_name)
{
	int Status;	
	unsigned long i = 0;
    unsigned long z = 0;        //Deslocamento do rootdir. 
    unsigned long n = 0;        //Deslocamento no nome.
    unsigned long max = 512;    //N�mero m�ximo de entradas em fat16.
	//...
	
	//Testa a estrutura.
	if( (void*) rootDir == NULL ){
		return (int) 1;
	};
	
	//Define se a estrutura for v�lida.
	unsigned char *root = (unsigned char *) rootDir->address;	
	
	    
	//
    // Checa par�metros.
    //	
		
	//Checa se o root dir est� na mem�ria.	
    if(rootDir->inMemory != 1){
	    printf("fs-search-fsSearchFile: Root dir is NOT in memory.\n");
	    return (int) 1;
	};

	//Checar se o endere�o � v�lido.
    if(rootDir->address == 0){
	    printf("fs-search-fsSearchFile: Root dir address fail.\n");
	    return (int) 1;
	};
    
	//Obs:
    //0x00      Entry never used
    //0xe5      File is deleted
    //0x2e      (A ".") Directory
	
	//Compara.
    while(i < max)
    {    
        if( root[z] != 0 )
		{
			Status = strncmp(file_name, &root[z], 11);
            if(Status == 0){ 
			    goto done; 
			}; 
        };   
        
		//Pr�xima entrada. Repete 512 vezes.
        z += 0x20;    
        ++i;          
    };
	
	//More?!
	
fail:
    printf("fs-search-fsSearchFile: File not found!\n");       	
	return (int) 1;    
done:
    return (int) 0;
};



/*
 * fs_search_empty_entry:
 *     Procurar uma entrada vazia em um dado diretorio na estrutura do sistema
 * de arquivos menos a primeira. 0 � erro.
 *
 */
unsigned long fs_search_empty_entry(unsigned long id){
    return (unsigned long) 0; 	
};


/*
 * fs_find_empty_entry:
 *     Encontrar uma entrada vazia na fat.
 */
unsigned long fs_find_empty_entry(){
	return (unsigned long) 0;
};


/*
 * fs_find_not_empty_entry:
 *     Encontrar uma entrada N�O vazia na fat.
 */
unsigned long fs_find_not_empty_entry()
{	
    return (unsigned long) 0;
};


/*
 * fs_find_n_empty_entries:
 *     Encontrar uma quantidade espec�fica de entradas na fat.
 *     Pois um arquivo demanda uma ou varias entradas, 
 *  dependendo do tamanho.
 * 
 */
unsigned long fs_find_n_empty_entries(unsigned long n)
{
    unsigned long i;
	unsigned long l;
	unsigned long empty;
	
	// Limits.
	if( n < 0 || n > 1024 ){
	    goto fail;
	};
	
	// Loop ~ Procurar uma quantidade de entradas vazias.
	for( i=0; i < n; i++ )
	{	
		empty = fs_find_empty_entry();
		
		// Preenche a lista de entradas vazias.	
		if( empty != 0 && empty < 1024 )
		{
		    file_cluster_list[l] = empty;
            l++;
		}else{
		    goto fail;
		};		
	};
		
	//Finaliza a lista com uma assinatura.
    file_cluster_list[l] = 0x0000FFF8;    
	
done:
    //Retorna o primeiro da lista.	
	return (unsigned long) file_cluster_list[0];
fail:	
    return (unsigned long) 0;
};


/*
 * fsSearchFileName:
 *     Procurar um nome de arquivo em uma pasta.
 *     No caso � o diret�rio raiz.
 */
//int fsSearchFileName( const char *name ) 
unsigned long fsSearchFileName(unsigned char *name)
{
    return (unsigned long) fsSearchFile(name);
};


//
// End.
//
