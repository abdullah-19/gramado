/*
 * File: fs/read.c 
 *
 * Descri��o:
 *     Rotinas de leitura
 *     As rotinas na biblioteca C devem chamar essas fun��es.
 *
 * History:
 *    2015 - Created by Fred nora.
 *    2016~2018 - Revision.
 *    ...
 */
 
 
/*
 #todo: 
     Usar estruturas para gerenciar parti��o, FAT e Root dir.
 #obs: 
     Cada parti��o deve ter uma estrutura.
 precisamos checar na estrutura o status da FAT e do 
 diret�rio raiz, para n�o carreg�los repetidas vezes.
 */ 
 
 
#include <kernel.h>


//#define read_ToUpper(C) ((((C) >= 'a') && ((C) <= 'z')) ? (C) - 'a' + 'A' : (C))
//#define read_Minimum(X,Y) ((X) < (Y) ? (X) : (Y))
//#define read_Maximum(X,Y) ((X) < (Y) ? (Y) : (X))



/*
 ************************************************
 * read_fntos:
 *     rotina interna de support.
 *     isso deve ir para bibliotecas depois.
 *     n�o tem prot�tipo ainda.
 *     Credits: Luiz Felipe 
 */
 
 // #bugbug
 // Isso modifica a string l� em ring3.
 // prejudicando uma segunda chamada com a mesma string
 // pois j� vir� formatada.
 
void read_fntos ( char *name ){

    int  i, ns = 0;
    char ext[4];

    ext[0] = 0;
    ext[1] = 0;
    ext[2] = 0;
    ext[3] = 0;


    // Transforma em mai�scula enquanto n�o achar um ponto.
    // #bugbug: E se a string j� vier mai�scula teremos problemas.

    //while ( *name && *name != '.' )
    while ( *name && *name != '.' )     // # testing
    {
        if ( *name >= 'a' && *name <= 'z' )
            *name -= 0x20;

        name++;
        ns++;
    };

 
    if ( name[0] == '\0' && ns <= 8 )
    {
        ext[0] = 'B';
        ext[1] = 'I';
        ext[2] = 'N';
        ext[3] = '\0';        
        goto _complete; //completa nome.
    }

    //if ( name[0] == '.' && ns < 8 )

    // Aqui name[0] � o ponto.
    // Ent�o constr�i a extens�o colocando
    // as letras na extens�o.

    for ( i=0; i < 3 && name[i+1]; i++ )
    {
		//Transforma uma letra da extens�o em mai�scula.
 
        //if (name[i+1] >= 'a' && name[i+1] <= 'z')
        //    name[i+1] -= 0x20;

        //ext[i] = name[i+1];
    

        //#testando
        //Se n�o for letra ent�o n�o colocamos no buffer de extens�o;
        if ( name[i+1] >= 'a' && name[i+1] <= 'z' )
        {
            name[i+1] -= 0x20;

            ext[i] = name[i+1];
        }
    };


_complete:

	//Acrescentamos ' ' at� completarmos as oito letras do nome.

    while (ns < 8)
    {
        *name++ = ' ';
        ns++;
    };


	//Acrescentamos a extens�o

    for (i=0; i < 3; i++)
        *name++ = ext[i];

    *name = '\0';
}


/*
 ***********************
 * fatClustToSect:
 *     Calcula qual � o setor inicial de um dado cluster.
 *     Informa��es para o calculo: 
 *     ~ N�mero do cluster.
 *     ~ N�mero de setores por cluster.
 *     ~ Lba inicial da area de dados.
 */
 
unsigned long 
fatClustToSect ( 
    unsigned short cluster, 
    unsigned long spc, 
    unsigned long first_data_sector )
{
    unsigned long C = (unsigned long) cluster;

    C -= 2;

	// #todo: 
	// Check limits.

    return (unsigned long) (C * spc) + first_data_sector;
}


/*
 ******************************************
 * fatLoadCluster:
 *     Carrega um cluster.
 *     Argumentos:
 *         setor   ~ Primeiro setor do cluster.
 *         address ~ Endere�o do primeiro setor do cluster.
 *         spc     ~ N�mero de setores por cluster.
 * Come�a do primeiro setor do cluster.
 */
 
void 
fatLoadCluster ( 
    unsigned long sector, 
    unsigned long address, 
    unsigned long spc )
{
    unsigned long i=0;

    for ( i=0; i < spc; i++ ){
        read_lba ( address, sector + i );
        address = (address +512); 
    };
}


/*
 *****************************************************************
 * read_lba:
 *     Carrega um um setor na mem�ria, dado o LBA.
 *     Obs: 
 *     Talvez essa rotina tenha que ter algum retorno no caso de falhas. 
 */
 
void read_lba ( unsigned long address, unsigned long lba ){


    // #todo
    // Fazer algum filtro de argumentos ??

    // See: volume.h
    switch (g_currentvolume_fatbits){

        case 32:
            debug_print ("read_lba: FAT32 not supported\n");
            return;
            break;

        case 16:
           // hdd.c
            my_read_hd_sector ( address, lba, 0, 0 );
            return;
            break;

        // Nothing.
        case 12:
            debug_print ("read_lba: FAT12 not supported\n");
            return;
            break;

        default:
            debug_print ("read_lba: g_currentvolume_fatbits not supported\n");
            break;
    };
}


/**
 * @brief Breaks a path
 * 
 * @details Parses the path pointed to by @p pathname extracting the first
 *          path-part from it. The path-part is stored in the array pointed to
 *          by @p filename.
 * 
 * @param pathname Path that shall be broken.
 * @param filename Array where the first path-part should be save.
 * 
 * @returns Upon successful completion, a pointer to the second path-part is 
 *          returned, so a new call to this function can be made to parse the
 *          remainder of the path. Upon failure, a #NULL pointer is returned 
 *          instead.
 */
 
 
/* 
// Credits: Penna.
const char *break_path (const char *pathname, char *filename); 
const char *break_path (const char *pathname, char *filename)
{

    char *p2;          // Write pointer. 
    const char *p1;    // Read pointer.  


	p1 = pathname;
	p2 = filename;
	
	//Skip those. 
	while (*p1 == '/')
		p1++;
	
	//Get file name.
	while ((*p1 != '\0') && (*p1 != '/'))
	{
		//File name too long. 
		if ((p2 - filename) > NAME_MAX)
			return (NULL);
		
		*p2++ = *p1++;
	}
	
	*p2 = '\0';
	
	return (p1);
}
*/




/*
 **************
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
 *
 * #obs: A fat deve estar o tempo todo na mem�ria, mas precisamos carregar 
 * na mem�ria o dret�rio atual para lermos um arquivo.
 * Essa fun��o de leitura n�o precisa carregar a FAT toda vez que for chamada.
 * ela deve apenas checar se a fat est� na mem�ria atrav�s de uma flag.
 * Mas essa fun��o precisa carregar o diret�rio atual na mem�ria para 
 * procurar pelo nome indicado.
 * #bugbug: No momento estamos apenas procurando no diret�rio raiz, 
 * ent�o devemos substituir o ponteiro *root por *current_dir.
 * IN:
 *     Endere�o do diret�rio, 
 *     Nome do arquivo, 
 *     Endere�o do arquivo.
 */


// #obs
// Rotina espec�fica para FAT16.
// Podemos mudar o nome para fsFat16LoadFile().
// Ou fs_Fat16_SFN_LoadFile()



// IN: ?? #todo: Descrever os argumentos de entrada.
// OUT: 
//    1=fail 
//    0=ok.

unsigned long 
fsLoadFile ( 
    unsigned long fat_address,
    unsigned long dir_address,
    unsigned char *file_name, 
    unsigned long file_address )
{

    int i=0;
    int Status;  //??

    int SavedDirEntry = 0;
    unsigned short next;

    unsigned long max = 64;    //?? @todo: rever. N�mero m�ximo de entradas.
    unsigned long z = 0;       //Deslocamento do rootdir 
    unsigned long n = 0;       //Deslocamento no nome.


    char tmpName[13];


    //int IsDirectory;


    // Cluster inicial
    unsigned short cluster; 

    // ?? 
    // Primeiro setor do cluster.
    unsigned long S;  

    // Usado junto com o endere�o do arquivo.
    unsigned long SectorSize;


    int Spc;

    // Updating fat address and __dir address.

    unsigned short *  fat = (unsigned short *) fat_address;
    unsigned short *__dir = (unsigned short *) dir_address;



    //
    // Initialize variables.
    //


    /*
    if (____IsCdRom) {
        SectorSize = 2048;
    } else {
        SectorSize = SECTOR_SIZE;
    }
    */

    SectorSize = SECTOR_SIZE;
    // ...


    // DIR
    // N�o carregaremos mais um diret�rio nesse momento
    // usaremos o endere�o passado por argumento.
    // Esperamos que nesse endere�o tenha um diret�rio carregado.

//load_DIR:
    debug_print ("fsLoadFile:\n");


    // File system structure.
    // + Checa se � v�lida a estrutura do sistema de arquivos.
    // + Pega a quantidade de setores por cluster.
    // + Pega o tamanho do diret�rio raiz. Ou seja, pega o n�mero 
    //    m�ximo de entradas.
    // ...

    if ( (void *) root == NULL ){
        panic ("fsLoadFile: No root file system.\n");

    }else{

        // #todo
        // Check if the root is initialized.
        //if (root->used != 1 || root->magic != 1234){
        //    panic ("fsLoadFile: validation");
        //}

        Spc = root->spc;

        if (Spc <= 0){
            panic ("fsLoadFile: Spc\n");
        }

        // Max entries 
        // N�mero de entradas no rootdir.
        // #bugbug: 
        // Devemos ver o n�mero de entradas no diret�rio corrente.

        max = root->rootdir_entries;

        if (max <= 0){
            panic ("fsLoadFile: max root entries \n");
        }

        // ...
    };



	// Continua ... 
	// Pegar mais informa��es sobre o sistema de arquivos.
	// Busca simples pelo arquivo no diret�rio raiz.
	// #todo: 
	// Essa busca pode ser uma rotina mais sofisticada. Uma fun��o auxiliar.
	// Primero caractere da entrada:
	// 0 = entrada vazia.
	// $ = entrada de arquivo deletado.
	// outros ...
	// ATEN��O:
	// Na verdade a vari�vel 'root' � do tipo short.
	 


    //
    // file name
    //
    
    // #todo
    // Ponteiro inv�lido para o nome do arquivo.
    // if ( (void *) file_name == NULL ){
    //    return 1;
    // }
    
    
    // Se o path come�a com / ent�o � absoluto.
    // Devemos come�ar pelo diret�rio raiz.
    //if ( *file_name == '/' )
    //{
        //Absolute = 1;
        //file_name++;



    //while (*file_name == '/')
        //file_name++;
        

    
    //#debug
    //vamos mostrar a string.
    //printf ("fsLoadFile: file_name={%s}\n", file_name);


    // name size.
    // Se o tamanho da string falhar, vamos ajustar.

    size_t size = (size_t) strlen (file_name); 

    if ( size > 11 ){
         printf ("fsLoadFile: size [FAIL] %d\n", size ); 
         size = 11;
    }
    
    
    // We are opening the root dir.
    //if ( file_name[0] == '/' && size == 1 )
    //{
    //}
    
    
    //
    // Search dirent.
    //



	//
	// Compare.
	//

    // #bugbug
    // #todo:
    // Para a vari�vel 'max' estamos considerando o n�mero de
    // entradas no diret�rio raiz. Mas precisamos considerar
    // o n�mero de entradas no diret�rio atual.
    // >> Para isso vamos precisar de uma estrutura de diret�rio
    // >> talvez a mesma usada em arquivos. (FILE)


    // Descri��o da rotina:
    // Procura o arquivo no diret�rio raiz.
    // Se a entrada n�o for vazia.
    // Copia o nome e termina incluindo o char '0'.
    // Compara 'n' caracteres do nome desejado, 
    // com o nome encontrado na entrada atual.
    // Se for encontrado o nome, ent�o salvamos o n�mero da entreda.
    // Cada entrada tem 16 words.
    // (32/2) pr�xima entrada! (16 words) 512 vezes!
    
    i = 0; 
    while ( i < max )
    {
        if ( __dir[z] != 0 )
        {
            memcpy ( tmpName, &__dir[z], size );
            tmpName[size] = 0;

            Status = strncmp ( file_name, tmpName, size );

            if ( Status == 0 )
            {
                SavedDirEntry = i; 
                goto __found; 
            }
        }; 
        z += 16;    
        i++;        
    }; 


    // Not found.
    // Sai do while. 
    // O arquivo n�o foi encontrado.
    // O arquivo n�o foi encontrado.

//notFound:
    printf ("fsLoadFile 1: %s not found\n", file_name );  
    goto fail;


    // Found.
    // O arquivo foi encontrado.

__found:

    // #debug
    // printf ("file FOUND!\n");
    // refresh_screen();
    // while(1){}
    
    
    //
    // Cluster.
    //

    // Get the initial cluster. 
    // Check cluster Limits.
    // (word). 
    // (0x1A/2) = 13.
    // Checar se 'cluster' est� fora dos limites.
    // +S�o 256 entradas de FAT por setor. 
    // +S�o 64 setores por FAT. 
    // Isso varia de acordo com o tamanho do disco.
    // O n�mero m�ximo do cluster nesse caso � (256*64).
    // #todo
    // Na verdade os dois primeiros clusters est�o indispon�veis.


    cluster = __dir[ z+13 ];

    if ( cluster <= 0 || cluster > 0xFFF0 ){
        printf ("fsLoadFile: Cluster limits %x \n", cluster );
        goto fail;
    }



    // FAT
    //Carrega fat na mem�ria.
    // #bugbug: 
    // N�o devemos carregar a FAT na mem�ria toda vez que 
    // formos carregar um arquivo. 
    // Talvez ela deva ficar sempre na mem�ria.
    // Precisamos de estruturas para volumes que nos d� esse 
    // tipo de informa��o

//loadFAT:

    fs_load_fat();


    // Load clusters.
    // Carregar o arquivo, cluster por cluster.
    // #todo: 
    // Por enquanto, um cluster � igual � um setor, 512 bytes.
    // Loop de entradas na FAT.
    // #todo: 
    // Esse loop � provis�rio, while pode ser problema.
    
    // #todo
    // Tabela tempor�ria para salvar os n�meros dos clusters
    // usados pelo arquivo.
    // ?? Qual ser� o tamanho dessa tabela ??
    //unsigned short tmp_table[1024];

__loop_next_entry:

    // #todo
    // Esse while � para o caso de termos mais de um setor por cluster.
    // Mas n�o � nosso caso at� o momento.

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


    // #todo
    // Poderia ter uma vers�o dessa fun��o para ler
    // um dado n�mero de setores consecutivos.


    // #todo
    // #importante
    // Esse � o momento em que vamos registrar na estrutura de arquivos
    // quais foram os clusters usados pelo arquivo, para assim podermos
    // salvar somente somente os setores modificados e n�o sempre o 
    // arquivo todo.
    // >> Mas n�o temos a estrutura de arquivos no momento.
    // Mesmo assim, talvez ja possomos salvar os n�meros dos clusters
    // em uma tabela tempor�ria.
    
    //tmp_table[tmp_table_index] = cluster;
    //tmp_table_index++;
    
    //
    // Read LBA.
    //

    read_lba ( file_address, VOLUME1_DATAAREA_LBA + cluster -2 ); 


    // Caution!
    // Increment buffer base address.
    // Pega o pr�ximo cluster na FAT.
    // Configura o cluster atual.
    // Ver se o cluster carregado era o �ltimo cluster do arquivo.
    // Vai para pr�xima entrada na FAT.

    file_address = (unsigned long) file_address + SectorSize; 

    next = (unsigned short) fat[cluster];

    cluster = (unsigned short) next;

    // ?? done
    if ( cluster == 0xFFFF || cluster == 0xFFF8 )
    { 
        // ?? message  
        // salvar a tabela na estrutura de arquivo.
        // Onde est� a estrutura de arquivos ??
        // Em que momento ela � criada ?
        // #bugbug: tem arquivo carregado pelo kernel
        // sem ter sido registrado na estrutura do processo kernel.
        return (unsigned long) 0; 
    }

    goto __loop_next_entry;


    //
    // Fail
    //

fail:

    printf ("fsLoadFile fail: file={%s}\n", file_name );
    refresh_screen ();
    return (unsigned long) 1;
}



/*
 ****************************************
 * fs_load_fat:
 *    Carrega a fat na mem�ria.
 *    Sistema de arquivos fat16.
 *    ? qual disco ?
 *    ? qual volume ? 
 *    #obs: Essa rotina poderia carregar a fat do volume atual do 
 * disco atual. � s� checar na estrutura.
 *
 * current disk, current volume, fat status.
 *
 * + Se o status da fat para o vulume atual indicar que 
 * ela j� est� carregada, ent�o n�o precisamos carregar novamente.
 */

// #todo
// Precisamos de uma estrutura com as informa��es sobre
// a FAT atual.

void fs_load_fat(void){

    unsigned long i=0;
    unsigned long b=0;


	// #bugbug 
	// Estamos atribuindo um tamanho, mas tem que calcular.
	// Salvo engano o tamanho � 246 setores.
    unsigned long szFat = 128;


    debug_print ("fs_load_fat:\n");

	//#todo:
	//+checar qual � o disco atual.
	//+checar qual � o volume atual.
	//+checar o status da FAT. Talvez ela j� esteja na mem�ria.
	//obs: a estrutura deve informar onde est� a fat do volume,
	//caso ja esteja na mem�ria.
	//obs: padronizaremos alguns endere�os, e alocaremos outros.

    //
    // Check cache state.
    //
    
    // Se ja est� na mem�ria, ent�o n�o precisamos carregar novamente.
    if (fat_cache_loaded==CACHE_LOADED){
         debug_print("fs_load_fat: FAT cache already loaded!\n");
         return;
    }


	// Carregar fat na mem�ria.
    for ( i=0; i < szFat; i++ ){
        
        my_read_hd_sector ( 
            VOLUME1_FAT_ADDRESS + b, 
            VOLUME1_FAT_LBA + i, 
            0, 
            0 );

        // Incrementa buffer para o pr�ximo setor.
        b = (b +512);    
    };
    
    // Changing the status
    fat_cache_loaded = CACHE_LOADED;
}


/*
 ***********************************************************
 * load_directory:
 *    Carrega o diret�rio na mem�ria,
 * dados o endere�o, o lba inicial e o n�mero de setores.
 */

// #bugbug
// Aqui estamos falando de uma sequ�ncia de setores.
// Isso serve para carregar o diret�rio raiz em fat16.

int
load_directory ( 
    unsigned long address, 
    unsigned long lba, 
    unsigned long sectors )
{
    unsigned long i=0;
    unsigned long b=0;


    debug_print ("load_directory:\n");
    
    for ( i=0; i < sectors; i++ ){
        my_read_hd_sector ( address + b, lba + i, 0, 0 );
        b = (b +512);    
    };
    
    return 0;
}



/*
 ****************************** 
 * fs_load_rootdir:
 * 
 */

// Carrega o diret�rio raiz.
// address, lba, number of sectors.

// #todo
// Precisamos de uma estrutura com as informa��es sobre
// o diret�rio raiz atual.

void fs_load_rootdir (void)
{
    debug_print ("fs_load_rootdir:\n");
    load_directory ( VOLUME1_ROOTDIR_ADDRESS, VOLUME1_ROOTDIR_LBA, 32 );
}



/*
 * fs_load_dir:
 *     Carrega um dado diret�rio da lista de arquivos, 
 * dado o �ndice da lista de streams do kernel.
 */
 
int 
fs_load_dir ( 
    unsigned long address, 
    unsigned long lba, 
    unsigned long sectors )
{
    debug_print ("fs_load_dir:[Testing]\n");
    
    // #todo:
    // Limits
    
    return (int) load_directory ( address, lba, sectors );
}




/*
 ***************************** 
 * fsGetFileSize: 
 * 
 */

// #bugbug: Isso d� problemas na m�quina real.
// Essa rotina � chamada pela fun��o fopen, por isso precisamos dela.
// Pega o tamanho de um arquivo que est� no diret�rio raiz.
// #todo: 
// Podemos alterar para pegar de um arquivo que esteja no diret�rio alvo.

// #todo:
// Antes de carregar um arquivo o sistema de arquivos
// precisa preencher uma estrutura com informa��es sobre ele. 
// se j� existir um registro � melhor.

// #bugbug
// Estamos com problemas na string do nome.

unsigned long fsGetFileSize ( unsigned char *file_name ){

    unsigned long FileSize=0;
    int i=0;
    
    int Status;   //??

    unsigned short next;

    unsigned long max = 64;    //?? @todo: rever. N�mero m�ximo de entradas.
    unsigned long z = 0;       //Deslocamento do rootdir 
    unsigned long n = 0;       //Deslocamento no nome.

    // #bugbug
    // Estamos com problemas na string do nome.
    
    //??Nome. 
    char NameX[13];

	// #importante:
	// Poder�amos usar malloc ou alocador de p�ginas ??	
	// A FAT permanece a mesma para a situa��od e termos apenas uma parti��o.
	//mas se tivermos mai de uma parti��o tamb�m precisamos carregar a FAT 
	//da parti��o atual.
	//unsigned short *fat = (unsigned short *) VOLUME1_FAT_ADDRESS;
    unsigned short cluster;    //Cluster inicial

    //??
    unsigned long S;  //Primeiro setor do cluster.
    int Spc;

	// #importante:
	// Poder�amos usar malloc ou alocador de p�ginas ??
	// #todo: Devemos carregar o diret�rio atual.

    unsigned short *root_dir = (unsigned short *) VOLUME1_ROOTDIR_ADDRESS;

	// #todo: Devemos carregar o diret�rio atual.
	//unsigned long current_dir_address = (unsigned long) Newpage();
    //#todo: devemos checar se o endere�o � v�lido.
	//unsigned short *current_dir = (unsigned short *) current_dir_address;	
	// #todo: 
	// devemos chamar uma fun��o que carregue um diret�rio no endere�o passado 
	//via argumento.
    //...
	
	// Lock ??.
	
	//taskswitch_lock();
	//scheduler_lock();	
		
		
	//	
	// ## ROOT ##
    //
    
    
//loadRoot:
	
	//Carrega o diret�rio raiz na mem�ria.
	
//#ifdef KERNEL_VERBOSE	
	//printf ("fsGetFileSize: Loading root..\n"); 
//#endif	
	
	//#bugbug
    //pega o tamanho de um arquivo que est� no diret�rio raiz.
    //#todo: 
    //podemos alterar para pegar de um arquivo que esteja no diret�rio alvo.	
	
	//carregando o diret�rio raiz.
	load_directory ( VOLUME1_ROOTDIR_ADDRESS, VOLUME1_ROOTDIR_LBA, 32 );	
	//fs_load_rootdirEx ();
	
	//#todo:
	//precisamos na verdade carregarmos o diret�rio corrente.
	
	// Continua ... 
	// Pegar mais informa��es sobre o sistema de arquivos.
	
	//#obs
	//Checa se � v�lida a estrutura do sistema de arquivos.
    //A inten��o � obtermos a quantidade de entradas no diret�rio raiz.
	//#bugbug: Mas isso deveria ser feito para o diret�rio atual.


    if ( (void *) root == NULL ){
        panic ("fsGetFileSize: No root file system\n");

    }else{

        // Setores por cluster.
        Spc = root->spc;

        if (Spc <= 0){
            panic ("fsGetFileSize: Spc\n");
        }
	
	    //Max entries ~ N�mero de entradas no rootdir.
		//#bugbug: Devemos ver o n�mero de entradas no diret�rio corrente.
	    max = root->rootdir_entries;	
		
        if (max <= 0){
            panic ("fsGetFileSize: max root entries\n");
        }

        // More?! 
        // ...
    };


    //
    // file name
    //
    
    //#debug
    //vamos mostrar a string.
    //printf ("fsGetFileSize: file_name={%s}\n", file_name);
	
	//Busca simples pelo arquivo no diret�rio raiz.
	//todo: Essa busca pode ser uma rotina mais sofisticada. 
	//Uma fun��o auxiliar.

	//Primero caractere da entrada:
	//0 = entrada vazia.
	//$ = entrada de arquivo deletado.
	//outros ...
	//ATEN��O:
    //Na verdade a vari�vel 'root' � do tipo short.	 

    i=0; 

	// Procura o arquivo no diret�rio raiz.
	
//search_file:

    //
    // file name limit.
    //

    size_t size = (size_t) strlen (file_name); 
    
    // o tamanho da string falhou
    //vamos ajustar.
    if ( size > 11 ){
        printf ("fsGetFileSize: name size fail %d\n",size );   
        size = 11;
    }


	//
	// Compare.
	//
	
	while ( i < max )
	{
		//Se a entrada n�o for vazia.
		if ( root_dir[z] != 0 )
        {
			// Copia o nome e termina incluindo o char 0.
			memcpy ( NameX, &root_dir[z], size );
			NameX[size] = 0;
			
            // Compara 11 caracteres do nome desejado, 
			// com o nome encontrado na entrada atual.
			Status = strncmp ( file_name, NameX, size );
			
            if ( Status == 0 ){ goto found; }
			// Nothing.
        }; 
		
		//(32/2) pr�xima entrada! (16 words) 512 vezes!
        z += 16;    
        i++;        
    }; 
	
	// Sai do while. 
	// O arquivo n�o foi encontrado.
		
//notFound:

    //#debug
    printf ("fsGetFileSize: %s not found\n", file_name );  
	printf ("fsGetFileSize: %s not found\n", NameX );  

//Falha ao carregar o arquivo.
fail:
    printf ("fsGetFileSize: file={%s}\n", file_name );	
    refresh_screen ();
	return (unsigned long) 0;
	
	//
	// Found !
	//
	
found:

    // #debug
    // printf("arquivo encontrado\n");
    // refresh_screen();
	// while(1){}
	
    //Pega o cluster inicial. (word)
	//cluster = root[ z+13 ];    //(0x1A/2) = 13.	
	
	//#debug
	//pegando o tamanho do arquivo.
    // 28 29 30 31

    FileSize = *(unsigned long*) (VOLUME1_ROOTDIR_ADDRESS + (z*2) + 28 );
	
	//printf ("%d \n" , root[ z+14 ]);
	//printf ("%d \n" , root[ z+15 ]);
	//printf ("done: FileSize=%d \n" , FileSize);
	
	//#debug
	//refresh_screen();
	//while(1){ asm("hlt"); }
	
	//#debug
	printf ("fsGetFileSize: FileSize=%d \n" , FileSize);
    refresh_screen ();


    return (unsigned long) FileSize;
}


//
// End.
//

