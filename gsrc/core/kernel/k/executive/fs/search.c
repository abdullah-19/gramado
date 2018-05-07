/*
 * File: fs\search.c 
 * 
 * Descri��o:
 *     Rotinas de procura de arquivos, nomes...
 *
 * History:
 *    2015 - Created by Fred Nora.
 *    2016 - Revision.
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
	int Status = 1;
    unsigned long i = 0;        // Deslocamento do dir. 
    unsigned long j = 0;        // Deslocamento no nome.
    unsigned long NumberOfEntries = 512;    // N�mero m�ximo de entradas no diret�rio.	
    
	char NameX[13];	
	
	// Buffer.
	char *dir = (char *) address;

	
	if(  file_name[0] == '/' || 
	     file_name[0] == 0 )
	{
		goto fail;
	};
	
	
    //
    // Address Limits:
	// Endere�o de mem�ria onde o diret�rio est�.
    //
	
	if(address == 0){
	    return (int) 1;
	}; 	
	
	//Compare.
    for( i=0; i < NumberOfEntries; i++ )
	{
        // FAT_DIRECTORY_ENTRY_FREE
		if(  dir[j] == (char) 0xE5 )
        {
		    j += 0x20;
            continue;			
		}	

		// diret�rio atual ou diret�rio pai.
		// '.' ou '..'
		if(  dir[j] == '.' )
        {
		    j += 0x20;
            continue;			
		}	
		
		// Entrada normal. Diferente de zero.
        if( dir[j] != 0 )
		{
			
			// Copia o nome e termina incluindo o char 0.
			memcpy( NameX, &dir[j], 11 );
			NameX[11] = 0;
			
		    Status = (int) strncmp( file_name, NameX, 11 );
			if(Status == 0){ 
			    goto done; 
			}; 
        };   
		
		//Pr�xima entrada. Repete 512 vezes.
        j += 0x20;              
    };
	
	//
	// Fail!
	//
	
fail:
    Status = (int) 1;
    printf("fs-search-KiSearchFile: File not found!\n");       	
done:
    return (int) Status;
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
	int Status = 1;	
	unsigned long i = 0;
    unsigned long j = 0;        //Deslocamento do rootdir. 
    unsigned long NumberOfEntries = 512;    //N�mero m�ximo de entradas em fat16.
	//...
	
	char NameX[13];	
	char *dir = (char *) VOLUME1_ROOTDIR_ADDRESS; //rootDir->address;	
	
	 
	if(  file_name[0] == '/' || 
	     file_name[0] == 0 )
	{
		goto fail;
	};
	 
    
	//Obs:
    //0x00      Entry never used
    //0xe5      File is deleted
    //0x2e      (A ".") Directory
	
	//Compare.
    for( i=0; i < NumberOfEntries; i++ )
	{
        // FAT_DIRECTORY_ENTRY_FREE
		if(  dir[j] == (char) 0xE5 )
        {
		    j += 0x20;
            continue;			
		}	

		// diret�rio atual ou diret�rio pai.
		// '.' ou '..'
		if(  dir[j] == '.' )
        {
		    j += 0x20;
            continue;			
		}	
		
		// Entrada normal. Diferente de zero.
        if( dir[j] != 0 )
		{
			
			// Copia o nome e termina incluindo o char 0.
			memcpy( NameX, &dir[j], 11 );
			NameX[11] = 0;
			
		    Status = (int) strncmp( file_name, NameX, 11 );
			if(Status == 0){ 
			    goto done; 
			}; 
        };   
		
		//Pr�xima entrada. Repete 512 vezes.
        j += 0x20;              
    };
	
	//More?!
	
fail:
    Status = (int) 1;
    printf("fs-search-fsSearchFile: File not found!\n");       	  
done:
    return (int) Status;
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
