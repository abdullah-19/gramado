/*
 * File: storage/volume.h
 *
 * Descri��o:
 *     Gerenciador de volumes.
 *     Header para o Volume Manager.
 *     Ger�ncia de volumes.
 *
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2018 - Some new structures.
 */

 
#define VOLUME_COUNT_MAX 1024


// volume atual ??
// Tipo de sistema de arquivos, fat16, ext2 ...
int g_filesystem_type;   //use this one.


char *current_volume_string;


// volume atual do tipo fat???
// Se � fat32, 16, 12.
int fatbits;   




/*
 *****************************************************
 * volume_type_t:
 *     Enumerando os tipos de volume.
 */

typedef enum {


    VOLUME_TYPE_NULL,            

    // Parti��o em disco f�sico.
    VOLUME_TYPE_DISK_PARTITION,  

    // Parti��o em disco virtual.
    VOLUME_TYPE_VIRTUAL_DISK_PARTITION,  

    // Arquivo.
    // Um arquivo qualquer. Sem formata��o.
    VOLUME_TYPE_RAW,           

    // Buffer.
    // Um arquivo qualquer. Sem formata��o.
    // Usado por banco de dados.
    VOLUME_TYPE_BUFFER,

    // Parti��o de swap.
    VOLUME_TYPE_SWAP

    //...

}volume_type_t;



/*
 * vbr_d:
 *     Structure for VBR parameters.
 */  

struct vbr_d
{
	//copiar mbr, � parecido;
}; 
struct vbr_d *vbr; 
 



/*
 * volume_d:
 *     Estrutura para acesso r�pido a volumes.
 *     Deve ser simples e com poucos elementos.
 */

struct volume_d
{
    object_type_t objectType;
    object_class_t objectClass;

    volume_type_t volumeType;
    
    //filesystem_type_t filesystemType;

    int id;

    int used;
    int magic;

    
    //
    // strings
    //


    //label
    char *name;

    //pathname
    char *cmd;

    //string usada no path para identificar o disco.
    //isso n�o existe.
    char path_string[32];  



    // Se o volume tiver um sistema de arquivos.
    // See: fs.h
    struct filesystem_d *fs;



    struct disk_d *disk;

    struct volume_d *next;
};

struct volume_d *volume_vfs;             // volume 0
struct volume_d *volume_bootpartition;   // volume 1
struct volume_d *volume_systempartition; // volume 2

//#importante:
//Esses s�o os tr�s volumes b�sicos do sistema 
//mesmo que o disco s� tenha um volume, essas 
//estruturas v�o existir.


//
// Lista de volumes.
//

unsigned long volumeList[VOLUME_COUNT_MAX];


//show info for all volumes in the list.
void volume_show_info (void);

void volumeShowCurrentVolumeInfo (void);

void *volume_get_volume_handle( int number );

void *volume_get_current_volume_info (void);

int volumeShowVolumeInfo( int descriptor );

int volume_init (void);


//
// End.
//

