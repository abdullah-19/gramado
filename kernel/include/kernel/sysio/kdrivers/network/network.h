
/*
 * File: network.h 
 *     header para o gerenciamento de rede.
 * 
 *  History: 
 *      2019 - Created by Fred Nora.
 */


// >> The register at offset 0x00 is the "IOADDR" window. 
// >> The register at offset 0x04 is the "IODATA" window. 


//packet format
// Ethernet IPv4 TCP/UDP DATA FCS



#define ETH_TYPE_IP   0x0800  
#define ETH_TYPE_ARP  0x0806
 
#define ARP_OPC_REQUEST  0x01
#define ARP_OPC_REPLY    0x02


//little endian
#define ToNetByteOrder16(v) ((v >> 8) | (v << 8))
#define ToNetByteOrder32(v) (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))
#define FromNetByteOrder16(v) ((v >> 8) | (v << 8))
#define FromNetByteOrder32(v) (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))


/*
 * network_info_d:
 *     Estrutura de rede.
 */ 


struct network_info_d
{
    int id;

    int used;
    int magic;

    int networkIdentifier;  // Número identificador da rede.
    char *networkName;      // Nome da rede.
    char *networkVersion;   // string mostrando a versão. ex: (1.1.1234)
    unsigned short networkVersionMajor;
    unsigned short networkVersionMinor; 
    unsigned short networkVersionRevision;

	//adaptador de rede.
	//struct intel_nic_info_d *nic;
	
	//user ??
	//struct user_info_d *networkUserinfo;
	
	//..


    struct network_info_d *next;
};

//struct network_info_d *Network;





int networkInit (void);


unsigned long 
network_procedure ( struct window_d *window,
                    int msg,
                    unsigned long long1,
                    unsigned long long2 );


// Tests.

void network_test(void);
void testNIC (void);



void networkSetstatus (int status);

int networkGetStatus (void);


// Show info.
void show_network_info (void);
void show_current_nic_info (void);


//manipular o pacote ipv6 recebido pelo handle do e1000.
int handle_ipv6 ( struct intel_nic_info_d *nic, struct ipv6_header_d *header );


// Send!

void 
SendARP ( uint8_t source_ip[4], 
          uint8_t target_ip[4], 
          uint8_t target_mac[6] );

int
network_SendIPV4_UDP ( uint8_t source_ip[4], 
                       uint8_t target_ip[4], 
                       uint8_t target_mac[6], 
                       uint8_t data[32],
                       unsigned short port );




// dialogo para o driver de rede.
unsigned long 
network_driver_dialog ( struct window_d *window, 
                        int msg, 
                        unsigned long long1, 
                        unsigned long long2 );

int network_decode_buffer ( unsigned long buffer_address );



// Handling requests.
int do_ipv4 ( unsigned long buffer );
int do_ipv6 ( unsigned long buffer );
int do_arp ( unsigned long buffer );




//
// End.
//

