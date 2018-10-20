/*
 * File: fs\fs.c 
 *
 * Descri��o:
 *    Arquivo principal do file system manager.
 *    M�dulo em kernel mode, incluido no kernel base. 
 *    
 *    *IMPORTANTE: A id�ia principal � que esse m�dulo de o suporte ao acesso 
 * a qualquer sistema de arquivo suportado, esteja ele em user mode ou em 
 * kernel mode.
 *
 *    Arquivo principal do m�dulo fs do executive do kernel..
 *    Gerenciador de arquivos do kernel.
 *    Na verdade o Kernel n�o tem atribui��es de carregar arquivos. Mas
 * ele ofere�e servi�os b�sicos que podem ser chamados pelos aplicativos.
 *
 * Objetivo:
 *    Ofere�er opera��es nos sistemas de arquivos suportados pelo kernel.
 *    Esse gerenciador chama as fun��es em kernel mode do sistema de arquivos
 * interno e chama as servi�os oferecidos pelos drivers em user mode.
 *
 *    Os drivers em user mode podem oferecer acesso � sistemas de arquivos
 * variados.
 *
 *   Ao kernel, compete as fun��es b�sicas de opera��es com hardware.
 *   O sistema de arquivos suportado pelo kernel � fat16.
 *   Continua ...
 *
 * Observa��o: 
 *    Os valores das estruturas do sistema de arquivos podem ser salvos 
 * em um registro.
 *    No registro pode ter um registro das opera��es efetuadas 
 * no sistema de arquivo.
 *   Continua ...
 *   
 * History: 
 *    2015 - Created by Fred Nora.
 *    2016 - Revision.
 *    ...
 */
 

#include <kernel.h>




//Vari�veis internas.
//int fsStatus;
//int fsError;
//...


// Checando a validade de arquivos bin�rios no formato PE.
// #bugbug: Isso n�o deve ficar no kernel, pois PE � propriet�rio.
// Nem usaremos PE no futuro.
int fsCheckPEFile ( unsigned long address ){
	
	unsigned char *buffer = (unsigned char *) address;
	
	//i386
	if ( buffer[0] != 0x4C || buffer[1] != 0x01 )
	{
		printf("fsCheckPEFile: Sig \n");
		goto fail;
	}
	
	//i486
	
	//Continua...
	
	//
	// # OK #
	//
	
//done:	
	return (int) 0;
	
fail:
    printf("fail\n");
    return (int) 1;
};


/*
 * fsListFiles:
 *     Lista os arquivos em um diret�rio, dados os �ndices de disco, 
 * volume e diret�rio.
 * #test isso funciona.
 */				  
void 
fsListFiles( int disk_id, 
             int volume_id, 
			 int directory_id )
{
	printf("fsListFiles: disk={%d} vol={%d} dir={%d}\n", 
	    disk_id, volume_id, directory_id );

	// @todo: Checar mais limites.
	
	if ( disk_id == -1 || 
	     volume_id == -1 || 
		 directory_id == -1 )
	{
		goto fail;	
	}
	
	// Show!
	// Se o diret�rio selecionado � o diret�rio raiz do VFS.
	
	if ( current_disk == 0 && 
	     current_volume == 0 &&
		 current_directory == 0 )
	{
	    vfsListFiles();
        goto done;		
	};
	
	// @todo: Suportar outros diret�rios.
	// ...
	
	goto done;
	
fail:
    printf("fail\n");
done:	
	refresh_screen();
	return;
};


/*
 * fsCreateVFS:
 * 
 * #bugbug : Isso est� errado:
 * agora temos rotinas de vfs em vfs.c
 *
  @todo
//  
// ** CRIANDO O DIRET�RIO RAIZ DO VFS BASEADO EM STREAMS **
//

//criando o diret�rio '/vfs'

**/
void fsCreateVFS (){
	
/*	
    FILE *vfs;
    char *vfs_root;
  
    //inicializa a estrutura ...
    vfs = ( void* ) malloc( sizeof(struct _iobuf) );
	if( (void*) vfs == NULL  ){
		//die();
	}	
	
	//aloca mem�ria para o diret�rio raiz do vfs.
    vfs_root = (char *) malloc(512*16);
	
	if( (char *) vfs_root == (char *) 0 )
	{
		//die();
	}else{
		
		//O in�cio da �rea de mem�ria reservado para o diret�rio 
		//ser� salvo na estrutura de stream.
		vfs->_base = (char *) vfs_root;
        //...
	};
*/
};


/*
void fsClearRootdirBuffer();
void fsClearRootdirBuffer()
{
    limpa_root();
	return;
}
*/


/*
void fsClearFatBuffer();
void fsClearFatBuffer()
{
    limpa_fat();
	return;
}
*/


/*
int fsIsFAT12();
int fsIsFAT12()
{
   //testa o tipo de fat.
    return 0;
};
*/


/*
int fsIsFAT16();
int fsIsFAT16()
{
    //testa o tipo de fat.
    return 0;
};
*/


/*
int fsIsFAT32();
int fsIsFAT32()
{
    //testa o tipo de fat.
    return 0;
};
*/


/*
int fs_open()
int fs_open()
{
  
    if( fs_channel == NULL)
	{
	    return 1;
	}
	else
	{
        fs_channel->opened = 1;		
	};
	
done:
    return;	
};
*/


//Listando arquivos em um diret�rio dado o nome.
int fsList ( const char *dir_name ){
	
	// #todo
	// #obs: Precisamos checar na estrutura de diret�rio as 
	// informa��es sobre o diret�rio em quest�o.
	
	//#test
	//boot volume
	if ( strncmp ( (char *) dir_name, "volume1", 7 ) == 0 )
	{
		
		//#todo:
		//pegar na estrutura do diret�rio as informa��es sobre 
		// esse diret�rio.
		
		//# improvisando os valores.
		fsFAT16ListFiles ( "Boot Volume:",              //name 
		    (unsigned short *) VOLUME1_ROOTDIR_ADDRESS, //address
			256 );		                                //number of entries
	};
	
	//...
	
	return (int) 0;
};


/*
 * fsFAT16ListFiles:
 *     Mostra os nomes dos arquivos de um diret�rio.
 *     Sistema de arquivos fat16.
 *
 * IN:
 *     dir_address = Ponteiro para um endere�o de mem�ria 
 *                   onde foi carregado o diret�rio.
 *
 */
void 
fsFAT16ListFiles( const char *dir_name, 
                  unsigned short *dir_address, 
				  int number_of_entries )
{
	//@todo: O n�mero de entradas � maior no diret�rio raiz.(512 ?)
	
    int i = 0;
	int max = number_of_entries;         // N�mero m�ximo de entradas.
    
	unsigned long j = 0;  // Deslocamento
	
	unsigned short *DirBaseAddress = (unsigned short *) dir_address;
	
	
	if ( number_of_entries <= 0 ){
		return;
	}
	
	printf("\n");
	printf("fsFAT16ListFiles: Listing names in [%s]\n\n", dir_name );
	
    
	// Mostra.
	i=0; 
	while (i < max)
	{
		// Diferente de vazio.
		if ( DirBaseAddress[j] != 0 ){
			
			//O problema � a termina��o da string '\0'
			printf ("%s\n", &DirBaseAddress[j] );
        } 
		
		//(32/2) pr�xima entrada! (16 words) 512 vezes!
        j += 16;    
        ++i;        
    }; 
	
	//...
	
	//printf ("Done\n");	
	refresh_screen();
};



/*
 * KeLoadFile:
 *     Carraga um arquivo na mem�ria.
 *     @todo: Ke_ n�o � o certo, pois ke � chamadas 
 *            � rotinas externas. 
 *
 * #bugbug a estrutura de canal ser� usada para troca 
 * de mensagens S|----|S
 */
int 
KeLoadFile( struct channel_d *channel, 
            unsigned char *file_name, 
			unsigned long file_address )
{
    int Status;
	
	if ( (void *) channel == NULL )
	{
	    return (int) 1;
	}else{
	    
		//Aberto?
	    if(channel->State != 1){
		    return (int) 1;
		};
		
		//esta em uso.
		channel->InUse = 1;
	
	};	
	
	//confere se o canal passado por argumento est� aberto.
	
    Status = (int) fsLoadFile ( file_name, file_address );
	
	if ( Status != 0 )
	{
		// Aqui tem que ter uma mensagem @todo.
	    channel->InUse = 0;
		channel->State = 0;
	    return (int) 1;
	};
	
	//N�o est� mais em uso.
	channel->InUse = 0;
	
	//Close channel.
	channel->State = 0;

    return (int) 0;	
};



/*
 * get_file:
 *     Obt�m o ponteiro da estrutura do arquivo, dado seu id.
 *     Com um �ndice da lista de arquivos, a fun��o retorna o ponteiro da
 *     estrutura do arquivo.
 */
 
// void *get_stream (int Index)
void *get_file (int Index){
	
	//Limits.	
	//@todo: max.
	if(Index < 0){
	    return NULL;
	};
	
    return (void *) Streams[Index];
};


/*
 * set_file:
 *     Coloca na lista de arquivos o ponteiro de estrutura,
 * dado seu id.
 * 
 */
 
//void set_stream ( void *stream, int Index ){ 
void set_file ( void *file, int Index ){
	
	//Limits.	
	if(Index < 0){
	    return;
	};
	
	//
	// @todo: Limite m�ximo da lista.
	//
	
    // Structure.
	if( (void *) file == NULL ){
		return;
	}	
	
	// Include pointer in the list.
	Streams[Index] = (unsigned long) file;
	//return;
};


void fs_test_fat_vector (){
	//Ainda n�o implementada.
};


unsigned long fs_get_fat_entry (unsigned long n){
	
    //Ainda n�o implementada.	
    return 0; 
};
											 

void fs_set_fat_entry ( unsigned long n, unsigned long value ){
	
    //Ainda n�o implementada.	
};


void fs_put_list_on_fat (){
	
	//Ainda n�o implementada.
};


void fs_set_structures (){
	
	//Ainda n�o implementada.
};


void fs_set_entry ( unsigned long id, unsigned long eid ){
	//suspensa
};


void fs_get_entry ( unsigned long id, unsigned long eid ){
	//suspensa
};


void 
fs_show_dir_entry( unsigned long id,
                   unsigned long eid )
{
    //Ainda n�o implementada.	
	//return; 
};



void fs_show_dir (unsigned long id){
	//Ainda n�o implementada.
};


//confere clusers de um diretorio.
unsigned long fs_check_cluster (unsigned long id){
	
	//Ainda n�o implementada.
	return 0; 
};



/*
 * fs_check_fat:
 *     Check FAT.
 */
unsigned long fs_check_fat (){
	
	//Ainda n�o implementada.
	return 0; 
};



void 
fs_show_entry( unsigned long id, 
               unsigned long eid )
{
	//Ainda n�o implementada.
	//return; 
};


unsigned long 
fs_get_entry_status( unsigned long id, 
                     unsigned long eid )
{
	//Ainda n�o implementada.
	return 0; 
};


void 
fs_set_entry_status( unsigned long id, 
                     unsigned long eid, 
					 unsigned long status )
{
	//Ainda n�o implementada.
    //return; 
};


/*
 *************
 * fsCheckMbrFile:
 *     Checamos um mbr carregado em algum endere�o de mem�ria.
 *
 *     Checa o registro do disco. (MBR). (disco do sistema).
 *
 *     Obs: O MBR desse sistema n�o tem o bpb no inicio do primeiro setor e 
 * sim no final do primeiro setor, antes da tabela de parti��es.
 *
 * Endere�o da mem�ria onde o MBR � carregado na inicializa��o. 0x00020000.   
 *
 * @todo: Talvez essa rotina pudesse fornecer o endere�o do buffer como 
 * argumento.
 * @todo: O mbr do sisco do sistema precisa de uma estruura que 
 * coordene o acesso a ele.
 *
 * #todo: precisa colocar tudo na estrutura que est� em fs.h
 */
//void fsCheckMBR(unsigned char* buffer)  //@todo
void fsCheckMbrFile ( unsigned char *buffer ){
	
	unsigned char *mbr = (unsigned char *) buffer; 

	
	my_read_hd_sector( (unsigned long) &mbr[0] , 0, 0 , 0 );  //setor 0.
	
	
	//message:
	printf("\n");
	printf("fsCheckMbrFile: testing MBR ...\n");
	
	// @todo:
	// Checar uma estrutura do mbr do disco do sistema,
	// para validar o acesso � ele.	
	
	// Check signature.
	if ( mbr[0x1FE] != 0x55 || mbr[0x1FF] != 0xAA )
	{
		
	    printf("fsCheckMbrFile: Sig FAIL \n" );
        goto fail;		
	}
	
	//jmp
	printf("JMP: [ %x ", mbr[ BS_JmpBoot + 0 ] );
	printf("%x ", mbr[ BS_JmpBoot + 1 ] );
	printf("%x ]\n", mbr[ BS_JmpBoot + 2 ] );
	
	
	//name
	int i;
	
	printf("OS name: [ ");
	for ( i=0; i<8; i++ )
	{
	    printf("%c", mbr[ BS_OEMName + i ] );
	};
	printf(" ]\n");
	
	//...
	
	
	printf("Signature: [ %x %x ] \n" , mbr[0x1FE], mbr[0x1FF] );
	
	// Continua ...
    goto done;

	
fail:	
    printf("fail\n");	
done:
    printf("Done\n");
	refresh_screen();
	return;
};


/*
 * fsCheckVbr:
 *     Checa o registro de boot de um volume.
 *     Vai no endere�o onde est� armazenado o VBR do volume atual
 *     e confere as informa��es sobre o volume.
 */
void fsCheckVbrFile( unsigned char *buffer ){
	
	unsigned char *vbr = (unsigned char *) buffer; 

    //
	// @todo:
	// Checar uma estrutura do mbr do disco do sistema,
	// para validar o acesso � ele.
	//
	
	
	// Check signature.
	if( vbr[0x1FE] != 0x55 || vbr[0x1FF] != 0xAA )
	{
	    printf("fsCheckVbrFile: Sig. Fail\n");
        goto fail;		
	};
	
	//
	// Continua ...
	//

    goto done;

	
fail:	
    printf("fsCheckVbrFile: fail\n");	
done:
    printf("Done\n");
	refresh_screen();
	return;
};



/*
 * fs_check_disk:
 *    Checa elementos do disco.
 *    Disco, Volume.
 *    Mbr. Vbr.
 * @todo: Mudar para fsCheckDisk().
 */
void fs_check_disk (){
	
    printf("fs_check_disk: Initializing..\n");	
	//fsCheckMbrFile();
	//fsCheckVbrFile();
    //...
	
    printf("Done\n");
	//refresh_screen();	
};


/*
 **************************************************
 * MountShortFileName:
 *     This function parses a directory entry name 
 * which is in the form 
 * of "FILE   EXT" and puts it in Buffer in the form of "FILE.TXT".
 * @todo fsMountShortFileName(...)
 */
void 
MountShortFileName( char *buffer, 
                    struct dir_entry_d *entry )
{
    int i = 0;
    
    // Get the file name.
    while (i < 8)
    {
        if ( entry->FileName[i] == ' '){
            break;
        }

        buffer[i] = entry->FileName[i];
        ++i;
    };
 
    // Get extension.
    if ( (entry->FileName[8] != ' ') )
    {
        buffer[i++] = '.';
        buffer[i++] = (entry->FileName[ 8] == ' ') ? '\0' : entry->FileName[ 8];
        buffer[i++] = (entry->FileName[ 9] == ' ') ? '\0' : entry->FileName[ 9];
        buffer[i++] = (entry->FileName[10] == ' ') ? '\0' : entry->FileName[10];
    };
};




/*
 * set_spc:
 *     Configura spc, 'Sector Per Cluster' em vari�vel global.
 */
void set_spc(int spc)
{
    g_spc = (int) spc;
};


/*
 * get_spc:
 *     Pega spc, Sector Per Cluster.
 */
int get_spc()
{
    return (int) g_spc;
};


/*
 * get_filesystem_type:
 *     Pega o tipo de sistema de arquivos.
 */
int get_filesystem_type()
{
    return (int) g_filesystem_type;
};


/*
 * set_filesystem_type:
 *     Configura o tipo de sistema de arquivo.
 */
void set_filesystem_type(int type)
{
    g_filesystem_type = (int) type;
};



/*
 ***********************************************
 * fs_init_fat:
 *     Inicializa a estrutura usada no sistema 
 * de arquivos.
 *     fsInitFat()
 */
void fs_init_fat (){
	
	// File system structure.
	if ( (void *) filesystem == NULL ){
	    printf("fs_init_fat error: fs\n");
	    return;
	}

	// FAT structure.
	fat = (void *) malloc( sizeof(struct fat_d) );
	if ((void *) fat == NULL ){
	    printf("fs_init_fat error: fat\n");
	    return;
	}
	
	// Info.
	fat->address = filesystem->fat_address; 
	fat->type = filesystem->type;
	//Continua ...
	
	// Continua a inicializa��o da fat.
	
//done:
//	return;
};	
	

/*
 ******************************************************
 * fs_init_structures:
 *     Inicializa a estrutura do sistema de arquivos.
 *     fsInitStructures
 */
void fs_init_structures (){
	
    int Type;
	
	// Structure.
    filesystem = (void *) malloc( sizeof(struct filesystem_d) );
	
	if ( (void *) filesystem == NULL ){
	    printf("fs_init_structures:");
        die();
	}
	
	//Type.
	Type = (int) get_filesystem_type();   
	if ( Type == 0 ){
	    printf("fs_init_structures error: Type");
        die();
	}
	
	filesystem->type = (int) Type;
	
    switch (Type)
	{
	    case FS_TYPE_FAT16:
	        
			//Rootdir.
			filesystem->rootdir_address = VOLUME1_ROOTDIR_ADDRESS;
	        filesystem->rootdir_lba = VOLUME1_ROOTDIR_LBA;
	        
			//Fat.
			filesystem->fat_address = VOLUME1_FAT_ADDRESS;
	        filesystem->fat_lba = VOLUME1_FAT_LBA;
	        
			//Dataarea.
			//filesystem->dataarea_address = ??;
	        filesystem->dataarea_lba = VOLUME1_DATAAREA_LBA;
	        
			//sectors per cluster.
			filesystem->spc = (int) get_spc(); //vari�vel
	        filesystem->rootdir_entries = FAT16_ROOT_ENTRIES;
	        filesystem->entry_size = FAT16_ENTRY_SIZE;
		    break;
			
	    case FS_TYPE_EXT2:
		    //nothing for now.
		break;
		
		//case FS_TYPE_EXT3: break;
		
        default:
		    //nothing for now.
            break;		
	};

//done:	
//	return;
};


/*
 ********************************************************
 * fsInit:
 *     Inicializa o file system manager.
 *
 */
int fsInit (){
	
#ifdef KERNEL_VERBOSE
    printf("fsInit: Initializing FS support..\n");
#endif 
 
	// Type - Configura o tipo de sistema de arquivos usado. 
	// No caso, (fat16).
	//
	// @todo: Deve-se checar o volume ativo e ver qual sistema de arquivos est�
	//        sendo usado, ent�o depois definir configurar o tipo.
	//        O sistema operacional pode salvar o tipo usado. Nesse caso 
	//        apenas checar se ouve altera��es nas configura��es de sistema de arquivos.
	//        O registro de configura��es de disco pode ser armazenado em arquivos de metadados.
	
	set_filesystem_type(FS_TYPE_FAT16);
	
	
	// SPC ~ Configura o n�mero de setores por cluster.
	//       Nesse caso, s�o(512 bytes por setor, um setor por cluster).
	
	set_spc(1);
	
   
    // ## initialize currents ##
	
	
	//selecionando disco, volume e diret�rio.
	//estamos resetando tudo e selecionando o diret�rio raiz 
	//do vfs ... mas na verdade o diret�rio selecionado 
	//deveria ser o diret�rio onde ficam a maioria dos aplicativos.
	//para que o usu�rio possa chamar o maior n�mero de apps usando 
	//apenas comandos simples.
    //#bugbug: isso deveria se passado pelo boot ??	
	
	
	//#bugbug: Deixaremos cada m�dulo inicializar sua vari�vel.
	//current_disk = 0;
	//current_volume = 0;
	//current_directory = 0;
	

	// Structures and fat.
    
#ifdef KERNEL_VERBOSE	
	printf("fsInit: Structures..\n");
#endif
	
	fs_init_structures();

#ifdef KERNEL_VERBOSE	
	printf("fsInit: FAT..\n");
#endif
	
	fs_init_fat();
	

	//Agora inicialzamos as stream 4 e 5.
	//As anteriores foram inicializadas em stdio,
	//pois s�o o fluxo padr�o.	
	
    //  ## volume 1 root dir  ##
	
	//foi definido em stdio.h
	//FILE *volume1_rootdir;
	
	volume1_rootdir = (FILE *) malloc( sizeof(FILE) );
	
	volume1_rootdir->_base = (char *) VOLUME1_ROOTDIR_ADDRESS;
	volume1_rootdir->_ptr = (char *) VOLUME1_ROOTDIR_ADDRESS;
	volume1_rootdir->_cnt = (32 * 512) ;
	volume1_rootdir->_file = 0; //?
	volume1_rootdir->_tmpfname = "volume1-stream";
	
	Streams[4] = (unsigned long) volume1_rootdir;
	
	
    //  ## volume 2 root dir  ##
	
	//foi definido em stdio.h
	//FILE *volume2_rootdir;
	
	volume2_rootdir = (FILE *) malloc( sizeof(FILE) );
	
	volume2_rootdir->_base = (char *) VOLUME2_ROOTDIR_ADDRESS;
	volume2_rootdir->_ptr = (char *) VOLUME2_ROOTDIR_ADDRESS;
	volume2_rootdir->_cnt = (32 * 512) ;
	volume2_rootdir->_file = 0; //?
	volume2_rootdir->_tmpfname = "volume2-stream";
	
	Streams[5] = (unsigned long) volume2_rootdir;
	
	
	// ## Inicializando os pipes usados em execve ## 
	
	//gramado core init execve 
	
	//aloca mem�ria para a estrutura.
	pipe_gramadocore_init_execve = (FILE *) malloc( sizeof(FILE) );
	
	
	//aloca mem�ria para o buffer.
	unsigned long pipe0base = (unsigned long) malloc(512);
	
	pipe_gramadocore_init_execve->_base = (char *) pipe0base;
	pipe_gramadocore_init_execve->_ptr  = (char *) pipe0base;
	pipe_gramadocore_init_execve->_cnt  = 512;
	pipe_gramadocore_init_execve->_file = 0; //??
	pipe_gramadocore_init_execve->_tmpfname = "pipe0";
	
	//0
	Pipes[0] = (unsigned long) pipe_gramadocore_init_execve;	
	
	//
	// ## PWD ##
	//
	
	// Inicializando o diret�rio de trabalho.
	
	/*
	struct dir_d *d;
	
	d = (void*) kmalloc( sizeof( struct dir_d ) );
	if( (void*) d == NULL )
	{
		//fail
		printf("fsInit: d");
		die();
	}else{
		
		//@todo: object
		
		d->id = 0;   //??
		d->used = 1;
		d->magic = 1234;
		
		d->stream = NULL; //@todo
		
		
		
		//d->entry_size_in_bytes
		//d->totalentries_size_in_bytes
		//d->fileMax
		//d->fileTotal
		//d->address
		//d->inMemory
		
		d->current = NULL;
		
		current_directory = 0;
		volume0RootDir = (struct dir_d *) d;
	};
	
	*/
	
	//inicializa p pwd support.
	fsInitializeWorkingDiretoryString();
	
	//
	// @todo: Continua ...
	//
	
done:

#ifdef KERNEL_VERBOSE
    printf("Done\n");
#endif 

    return (int) 0;    	
};


/*
 *****************************************
 * fsInitializeWorkingDiretoryString:
 *     Atualiza a string do diret�rio de trabalho.
 * Essa � a string que ser� mostrada antes do prompt.
 * 'pwd'> 
 * ?? isso deve sser todo o pathname do pwd ?? 
 * ex: root:/volume0>
 */
void fsInitializeWorkingDiretoryString (){
	
	//get info
	
	//test: get current volume id.
	//current_volume_id = (int) system_call(171,0,0,0);
	//current_volume = (int) 
	
	//global usada para string do nome do volume.
	current_volume_string = (char *) FS_VOLUME1_STRING;
	
    //  ## volume list ##
    //primeiro colocamos a string que indica a lista de volumes.
    sprintf ( current_workingdiretory_string, FS_VOLUMELIST_STRING ); 
	
	// ## separador ##
	strcat ( current_workingdiretory_string, FS_PATHNAME_SEPARATOR );
	
	//  ## volume root dir ##
	strcat ( current_workingdiretory_string, current_volume_string );
		
	// ## separador ##
	strcat ( current_workingdiretory_string, FS_PATHNAME_SEPARATOR );		

	//More ?...
    pwd_initialized = 1;
    //return;
};


/*
 *****************************************
 * fsUpdateWorkingDiretoryString:
 *     Atualiza a string do diret�rio de trabalho.
 * Essa � a string que ser� mostrada antes do prompt.
 * 'pwd'> 
 * ?? isso deve sser todo o pathname do pwd ?? 
 * ex: root:/volume0>
 * #bugbug o prot�tipo deveria estar em fs.h;
 */ 
void fsUpdateWorkingDiretoryString ( char *string ){
	
	if ( pwd_initialized == 0 )
	{
		goto fail;
	}else{
		
        if( (void *) string == NULL )
	    {
		    goto fail;
	    }else{
	    
	        // ## separador ##
		    //strcat( current_workingdiretory_string, 
			//    FS_PATHNAME_SEPARATOR );		
		
	        // ## separador ##		
		    strcat( current_workingdiretory_string, 
			    string );	

	        // ## separador ##
		    strcat( current_workingdiretory_string, 
			    FS_PATHNAME_SEPARATOR );				
		};
	};
	//...
fail:	
done:
    return;	
};


/* 
 ************************************************
 * fs_pathname_backup:
 *     Remove the last N directories from PATH.  
 * Do not leave a blank path.
 * PATH must contain enough space for MAXPATHLEN characters. 
 * Credits: bash 1.05
 */
void fs_pathname_backup ( char *path, int n ){
	
    register char *p = path + strlen(path);

    if (*path)
        p--;

    while (n--)
    {
        while (*p == '/')
	        p--;

        while (*p != '/')
	        p--;

        *++p = '\0';
    };	
};


//
// End.
//

