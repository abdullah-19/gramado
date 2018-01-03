/*
 *  File: gdevice.h
 *
 * Defini��o: 
 * Informa��es globais sobre o dispositivo onde o sistema operacional foi instalado.
 * Informa��es sobre o host device.
 * N�o � a qui o lugar para se detalhar todos os dispositivos instalados na m�quina.
 * Aqui � lugar de classificar a plataforma alvo para o sistema operacional. Para
 * que as aplica��es possam se adaptar �s caracter�sticas do dispositivo que suporta
 * o sistema operacional.
 *
 * Vers�o 1.0, 2016 - Created.   
 */
 
 
 
//Classes mais comuns de dispositivos.
//Outras poder�o ser inclu�das futuramente. 
typedef enum {
	HostDeviceTypePC,        //PC.
	HostDeviceTypeMobile,    //Mobile.
	HostDeviceTypeIOT,       //Iot.
	//...
}host_device_type_t; 
 
typedef struct host_device_info_d host_device_info_t; 
struct host_device_info_d
{
	//Type.
    host_device_type_t type;
	
	
	//Screen size.
	unsigned long host_device_screen_width;
	unsigned long host_device_screen_height;

    //Continua ...
	
};
host_device_info_t *HostDeviceInfo; 
//
 
// 
// Globais de f�cil acesso.
// devem refletir os valores armazenados na estrutura.
//

//Screen sizes.
unsigned long g_device_screen_width; 
unsigned long g_device_screen_height; 

 
//
// End.
//

