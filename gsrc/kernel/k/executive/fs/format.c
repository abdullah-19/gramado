/*
 * File: fs\format.c
 *
 * Descri��o:
 *     Sevi�os de formata��o de disco e volumes.
 *     Formata a tabela de parti��es no mbr.
 *     Altera valores no mbr e metafile. 
 *
 * @todo:
 *     Servi�os de formata��o podem ir para um aplicativo.
 * Hist�rico:
 *    Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *    Vers�o 1.0: 2016 - Revis�o.
 *    ...
 */
 

#include <kernel.h>


//@todo: mudar o nome da fun��o para fs_clear_fat.
void fs_apaga_fat()
{
    fs_init_fat();
    return;
};


void fs_delete_entry(unsigned long id, unsigned long eid){
    return;
};


/*
 * limpa_root: 
 *
 * Atribui��es:
 *     +Salvar o o conte�do da mem�ria no hd.
 *     +Zerar o buffer na mem�ria ram.
 *     Continua...
 *
 * @todo: 
 *     Mudar o nome da fun��o para fsClearRoot.
 */
void limpa_root()
{
    int i;	
	unsigned char *root = (unsigned char *) FAT16_ROOTDIR_ADDRESS;

    
	/*
     * @todo: 
	 *     O endere�o do rootdir deveria estar definido em uma
	 * vari�vel global. Por enquanto esse endere�o � constante.
	 */	
    
	/*
     * Tamanho do rootdir.
     *
     * @todo: 
	 *     O tamanho do rootdir deveria estar definido em uma
	 * vari�vel global. Por enquanto o tamanho � constante.
	 */
	i = 0;
     
	 
	
	/*
	 * Limpando o rootdir. (32 setores.) 512 entradas de 32bytes. 
     */
	while(i < (32*512)){
        root[i] = (unsigned char) 0;    
        ++i;
    };
     
    /*
	 * Salva o root dir. (size?)    
     */
	//write_lba(FAT16_ROOTDIR_ADDRESS, FAT16_ROOTDIR_LBA);
     
    return;
};


/*
 * fsClearFat:
 *      Zera o buffer e grava zeros no hd.
 */
void fsClearFat()
{
	int i;
    unsigned char *fat_base = (unsigned char *) FAT16_FAT_ADDRESS;
	
	/*
     * @todo: 
	 *     O endere�o da fat deveria estar definido em uma
	 * vari�vel global. Por enquanto esse endere�o � improviso.
	 */    
    
	/*
     * Tamanho da fat.
     *
     * @todo: 
	 *     O tamanho da fat deveria estar definido em uma
	 * vari�vel global. 
	 */	
    i = (int) (512*4);
	
	while(i > 0){
        fat_base[i] = 0;    
        i--;
    }; 

    //Salva  FAT ! 
	write_lba(FAT16_FAT_ADDRESS, FAT16_FAT_LBA);

	
    //@todo: Usar while.      
   
done:   
	return;
};

//
// End.
//

