/*
 * File: fs\read.c 
 *
 * Descri��o:
 *     Rotinas de leitura
 *
 * Obs:
 *    As rotinas na biblioteca C devem chamar essas fun��es.
 * Hist�rico:
 *    Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *    Vers�o 1.0: 2016 - Revis�o.
 *    ...
 */
 
 
#include <kernel.h>

    /*
	 * Teste: Funcionou, leu um arquivo.
	printf("*teste ...\n");
	refresh_screen();
	unsigned long buf[32];
	printf("Loading ...\n");
	carregar_arquivo("FREDFILETXT",(unsigned long) buf);
	printf("Showing ...\n");
    printf("%s\n",buf);  			
	printf("*done.");
	refresh_screen();
	while(1){}
	*/


/*
 ************************************************
 * read_fntos:
 *     rotina interna de support.
 *     isso deve ir para bibliotecas depois.
 *     n�o tem prot�tipo ainda.
 * Credits: Luiz Felipe
 */
void read_fntos ( char *name ){
	
    int  i, ns = 0;
    char ext[4];
	
    // Transforma em mai�scula.
	while ( *name && *name != '.' )
	{
        if( *name >= 'a' && *name <= 'z' )
            *name -= 0x20;

        name++;
        ns++;
		
		// ##bugbug: 
		// ?? E se n�o encontrarmos o ponto??
    };

    // Aqui name[0] � o ponto. 
	// Ent�o constr�i a extens�o.
	
	for ( i=0; i < 3 && name[i+1]; i++ )
	{
		// Transforma em mai�scula.
        if( name[i+1] >= 'a' && name[i+1] <= 'z' )
            name[i+1] -= 0x20;

        ext[i] = name[i+1];
    };

    //preenche com espa�o at� o limite de 8, onde deve come�ar a extens�o.
	while ( ns < 8 )
	{
        *name++ = ' ';
        ns++;
    };

    for ( i=0; i < 3; i++ )
        *name++ = ext[i];

    *name = '\0';
};
	
	
	
/*
 *****************************************************************
 * fatClustToSect:
 *     Calcula qual � o setor inicial de um dado cluster.
 *     Informa��es para o calculo: 
 *     ~ N�mero do cluster.
 *     ~ N�mero de setores por cluster.
 *     ~ Lba inicial da area de dados.
 */	
unsigned long fatClustToSect(unsigned short cluster, 
                             unsigned long spc, 
							 unsigned long first_data_sector )
{
    unsigned long C = (unsigned long) cluster;

	C -= 2;
	
	//@todo: Check limits.

done:	
	return (unsigned long) (C * spc) + first_data_sector;
};


/*
 *************************************************************
 * fatLoadCluster:
 *     Carrega um cluster.
 *     Argumentos:
 *         setor   ~ Primeiro setor do cluster.
 *         address ~ Endere�o do primeiro setor do cluster.
 *         spc     ~ N�mero de setores por cluster.
 */
void fatLoadCluster ( unsigned long sector, 
                      unsigned long address, 
					  unsigned long spc )
{
	unsigned long i;

	//Come�a do primeiro setor do cluster.	
	for ( i=0; i < spc; i++ ){
		
        read_lba( address, sector + i );
		address = address +512; 
	};
//done:
	//return;
};


/*
 *****************************************************************
 * read_lba:
 *     Carrega um um setor na mem�ria, dado o LBA.
 *     fsReadLBA(..)
 *     Obs: Talvez essa rotina tenha que ter algum retorno no caso de falhas.
 */
//void read_fat_lba( unsigned long address, unsigned long lba) 
void read_lba ( unsigned long address, unsigned long lba ){
	
    //Obs: 'fatbits' � uma vari�vel global??
	
 	//taskswitch_lock();
	//scheduler_lock();	          	
	
    switch(fatbits)
    {
	    case 32:
	        //Nothing.	    
            return;
			break;		
			
	    case 16:
		    //hdd.c
	        my_read_hd_sector( address, lba, 0, 0);	    
            return;
			break;		
			
	    case 12:
	        //Nothing.	    
            return;
			break;

        default:
		    //@todo: 
			// Precisamos fazer alguma coisa se essa vari�vel for 
			// um valor inv�lido.
		    return;
            break;		
	};	
	
	//scheduler_unlock();
	//taskswitch_unlock();
	
done:
	return;
};


/*
 ************************************************************************
 * fsLoadFile:
 *    Carrega um arquivo na mem�ria dado o nome e o endere�o.
 *    Apenas FAT16.
 *
 *  @todo: 
 *      Deve-se alocar mem�ria dinamicamente para o root dir e 
 * para a fat caso eles n�o estejam carregados e n�o tenham seus 
 * endere�os salvos.
 * Obs: Pode existir uma estrutura pra dizer se o root dir e fat est�o 
 * carregados na mem�ria e qual o endere�o o tipo o tamanho etc.
 * + O argumento nome poderia ser const char* para compatibilidade 
 * com a libC.
 */
//int fsLoadFile( const char *name, unsigned long address ) 
unsigned long 
fsLoadFile( unsigned char *file_name, 
            unsigned long file_address )
{
    int Status;		
	int i;
    unsigned short next;

	// #importante:
	// Poder�amos usar malloc ou alocador de p�ginas ??
    unsigned short *root = (unsigned short *) VOLUME1_ROOTDIR_ADDRESS;
    unsigned long max = 64;    //?? @todo: rever. N�mero m�ximo de entradas.
    unsigned long z = 0;       //Deslocamento do rootdir 
    unsigned long n = 0;       //Deslocamento no nome.
	char NameX[13];	           //??Nome. 

	// #importante:
	// Poder�amos usar malloc ou alocador de p�ginas ??	
    unsigned short *fat = (unsigned short *) VOLUME1_FAT_ADDRESS;
	unsigned short cluster;    //Cluster inicial

    //??	
	unsigned long S;  //Primeiro setor do cluster.
	int Spc;

    //...
	
	// Lock ??.
	
	//taskswitch_lock();
	//scheduler_lock();	
	
	// Root dir.

//loadRoot:
	
	//Carrega o diret�rio raiz na mem�ria.
	
#ifdef KERNEL_VERBOSE	
	printf("fsLoadFile: Loading root..\n"); 
#endif	
	
	fs_load_rootdirEx();
	
	//Checa se � v�lida a estrutura do sistema de arquivos.
	if( (void *) filesystem == NULL )
	{
	    printf("fs-read-fsLoadFile: filesystem\n");
		goto fail;
	}else{
		
	    //Setores por cluster.
	    Spc = filesystem->spc;
	    if(Spc <= 0){
	        printf("fs-read-fsLoadFile: Spc\n");
		    goto fail;
	    };
	
	    //Max entries ~ N�mero de entradas no rootdir.
	    max = filesystem->rootdir_entries;	
	    if(max <= 0){
	        printf("fs-read-fsLoadFile: max\n");
			goto fail;
	    };
		
	    // More?! 
		// ...
	};
	
	// Continua ... 
	// Pegar mais informa��es sobre o sistema de arquivos.
	
	
    /*
	 * Busca simples pelo arquivo no diret�rio raiz.
	 *
	 * @todo: Essa busca pode ser uma rotina mais sofisticada. Uma fun��o
	 * auxiliar.
	 */

    /*
	 * Primero caractere da entrada:
	 *   0 = entrada vazia.
	 *   $ = entrada de arquivo deletado.
	 *   outros ...
	 *
	 *  ATEN��O:
     *      Na verdade a vari�vel 'root' � do tipo short.	 
	 */
	 
	i = 0; 
	
	// Procura o arquivo no diret�rio raiz.
	
//search_file:

    size_t size = (size_t) strlen(file_name); 
	
	//Compara.
	while ( i < max )
	{
		//Se a entrada n�o for vazia.
		if ( root[z] != 0 )
        {
			// Copia o nome e termina incluindo o char 0.
			memcpy( NameX, &root[z], size );
			NameX[size] = 0;
			
            // Compara 11 caracteres do nome desejado, 
			// com o nome encontrado na entrada atual.
			Status = strncmp( file_name, NameX, size );
            if ( Status == 0 ){ 
			    goto found; 
			}
			// Nothing.
        }; 
		
		//(32/2) pr�xima entrada! (16 words) 512 vezes!
        z += 16;    
        ++i;        
    }; 
	
	// Sai do while. O arquivo n�o foi encontrado.
	
    // O arquivo n�o foi encontrado.	
//notFound:
    printf ("fs-read-fsLoadFile: %s not found\n", file_name );  
    //printf ("fs-read-fsLoadFile: %s not found\n", NameX );
	//printf("root: %s ",root);	
    goto fail;
	
    // O arquivo foi encontrado.	
found:

    // #debug
    // printf("arquivo encontrado\n");
    // refresh_screen();
	// while(1){}
	
    //Pega o cluster inicial. (word)
	cluster = root[ z+13 ];    //(0x1A/2) = 13.	
	
	
	// Cluster Limits.
	// Checar se 'cluster' est� fora dos limites.
	// +S�o 256 entradas de FAT por setor. 
	// +S�o 64 setores por FAT. 
	// Isso varia de acordo com o tamanho do disco.
	// O n�mero m�ximo do cluster nesse caso � (256*64).
	
	if ( cluster <= 0 || cluster > 0xfff0 ){
		
	    printf("fs-read-fsLoadFile: Cluster limits %x \n", cluster );
		goto fail;
	}	
	
	
//loadFAT:
	
    //Carrega fat na mem�ria.
	
#ifdef KERNEL_VERBOSE		
	printf("loading FAT..\n");
#endif 
	
	fs_load_fatEx();
	
    // Carregar o arquivo, cluster por cluster.
    // @todo: Por enquanto, um cluster � igual � um setor, 512 bytes.
 
    //Debug:
#ifdef KERNEL_VERBOSE		
    printf("Loading clusters..\n");
    //refresh_screen();
#endif

    //
	// Loop: 
	//     Loop de entradas na FAT.
    //     @todo: Esse loop � provis�rio, while pode ser problema.
	//
	
	
	//
	// Carregar o arquivo.
	//
	
//Loop.	
//proxima_entrada:
next_entry:
	
	/*
	while(1)
	{	
	    //Calcula.
		//Primeiro setor do cluster. 
		S = fatClustToSect(cluster, Spc, VOLUME1_DATAAREA_LBA); 
		
		//Carrega 'x' setores come�ando de S.
		fatLoadCluster( S, file_address, Spc);
		
		//Pegar o pr�ximo cluster na FAT.
		//O cluster atual cont�m o n�mero do pr�ximo.
		cluster = fat[cluster];
		
		//Ver se o cluster carregado era o �ltimo.
	    if(cluster == 0xFFFF || cluster == 0xFFF8){ goto done; };
		//Nothing.
    };
	*/
	
	read_lba( file_address, VOLUME1_DATAAREA_LBA + cluster -2 ); 
	
	//Incrementa o buffer. +512;
	//SECTOR_SIZE;
	file_address = (unsigned long) file_address + 512;    	
	
	
	//Pega o pr�ximo cluster na FAT.
	next = (unsigned short) fat[cluster];		
	
	
	//Configura o cluster atual.
	cluster = (unsigned short) next;	
	
	//Ver se o cluster carregado era o �ltimo cluster do arquivo.
	if ( cluster == 0xFFFF || cluster == 0xFFF8 ){ 
	    goto done; 
	}

	//
	// Loop: 
	// Vai para pr�xima entrada na FAT.
	//
	
	//goto proxima_entrada;
	goto next_entry;
	
	//Nothing.
	
//Falha ao carregar o arquivo.
fail:
    printf("fs-read-fsLoadFile fail: file={%s}\n", 
	    file_name );	
    refresh_screen();
	return (unsigned long) 1;
//Done. 	
done:
    
	//#debug support
	//printf("fsLoadFile: done\n");
	//refresh_screen(); 
    
	return (unsigned long) 0;
};



/*
 ***********************************************************
 * load_directory:
 *    Carrega o diret�rio na mem�ria, dados o endere�o, 
 * o lba inicial e o n�mero de setores.
 *
 */
void 
load_directory( unsigned long address, 
                unsigned long lba, 
				unsigned sectors )
{
	unsigned long i;
	unsigned long b = 0;
	
	//Carregar root dir na mem�ria.
	for ( i=0; i < sectors; i++ )
	{
	    read_lba ( address + b, lba + i );    
		
		// Incrementa buffer.
		b = b+512;    
	};
    //return;
};


/*
 ********************************************************
 * fs_load_rootdirEx:
 *    Carrega o diret�rio raiz na mem�ria.
 *    Sistema de arquivos fat16.
 *    ? qual disco ?
 *    ? qual volume ? 
 */
void fs_load_rootdirEx (){
	
	unsigned long i;
	unsigned long b = 0;
	unsigned long szRoot = 32;
	
	//Carregar root dir na mem�ria.
	for ( i=0; i < szRoot; i++ )
	{
	    //read_lba( VOLUME1_ROOTDIR_ADDRESS + b, VOLUME1_ROOTDIR_LBA + i );
        my_read_hd_sector (VOLUME1_ROOTDIR_ADDRESS + b, VOLUME1_ROOTDIR_LBA + i, 0, 0 );
		
		//Incrementa buffer.
		b = b+512;    
	};
//done:	
//    return;
};


/*
 ****************************************
 * fs_load_fatEx:
 *    Carrega a fat na mem�ria.
 *    Sistema de arquivos fat16.
 *    ? qual disco ?
 *    ? qual volume ? 
 */
void fs_load_fatEx (){
	
	unsigned long i;
	unsigned long b = 0;
	
	//#bugbug 
	//Estamos atribuindo um tamanho, mas tem que calcular.
	unsigned long szFat = 128;
	
	//Carregar root dir na mem�ria.
	for ( i=0; i < szFat; i++ )
	{
	    //read_lba( VOLUME1_FAT_ADDRESS + b, VOLUME1_FAT_LBA + i ); 
		my_read_hd_sector( VOLUME1_FAT_ADDRESS + b, VOLUME1_FAT_LBA + i, 0 , 0 );
		b = b+512;    //Incrementa buffer.
	};
//done:	
//    return;
};
 
//preenche o diretorio raiz com informa��es guardadas em disco. 
void fs_load_rootdir()
{
    fs_load_dir (0);	
	//return;
}; 


/*
 * fs_load_dir:
 *     Carrega um dado diretorio da lista de arquivos, dado o �ndice.
 */
void fs_load_dir ( unsigned long id ){
	
	unsigned long i;
	unsigned long n = 0;
	unsigned long lba;
	
    struct dir_d *File;

	File = (void *) Streams[id];	
	
	if ( (void *) File == NULL ){
	    printf("fs_load_dir fail: Struct\n");
	    return;
	}
	
	if (File->address == 0){
	    printf("fs_load_dir: Null address\n");
		return;
	}
	
	//buffer 
	
	unsigned long buffer = (unsigned long) File->address;  
	
	//na inicializa��o so temos a lba inicial do diretorio raiz
	if (id == 0)
	{
	    lba = VOLUME1_ROOTDIR_LBA;
	}else{
	    //lba = filesystem->dir[id].lba_inicial;
	};
	
	//64??
	for ( i=0; i < 64; i++ )
	{
	    //lba inicial do root dir + deslocamento.
		read_lba ( buffer, lba + n); 
		//read_lba( &filesystem->dir[lba], lba + n); 
 
	    //prennche a estrutura com os valores do buffer.
	    //o diretorio '0' � o diretorio raiz. 
		fs_set_entry(id,i);
		n++;
	};
	
//done:	
//	return;
};


//
// End.
//

