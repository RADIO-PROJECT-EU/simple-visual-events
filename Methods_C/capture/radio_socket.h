/*
Include socket libraries
*/

#include <netinet/in.h>
#include <sys/socket.h>

/*
Define ports and data buffers
*/

#define RECEIVE_PORT 8000
#define SEND_PORT 8001
#define BUF_SIZE 2000
#define SOCKET_VERBOSE 1

/*
Define staructures to hold socket info
*/

int sss = 0; // Socketdescriptor
int cs = 0; // Current Socket
struct sockaddr_in target_host_address;
unsigned char * target_address_holder;

/*
create_upd_socket()
*/

int create_udp_socket(int port)
{
    int sd;
    struct sockaddr_in host_address;
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd < 0 ) { return -1; }
    memset((void*)&host_address,0,sizeof(host_address));
    host_address.sin_family=AF_INET;
    host_address.sin_addr.s_addr=INADDR_ANY;
    host_address.sin_port=htons(port);
    if (bind (sd,(struct sockaddr*)&host_address,sizeof(host_address))<0) { return -2; }
    return sd;
}


/*
udp_open()

dest_ip must contain four bytes of the target ip address

*/

int udp_open(unsigned char* dest_ip) 
{
    unsigned char* t;
    
    cs = create_udp_socket(SEND_PORT);
    if (cs < 0) { return -1; }
    target_host_address.sin_family=PF_INET;
    target_host_address.sin_port=htons(RECEIVE_PORT);
    target_address_holder=(unsigned char*)&target_host_address.sin_addr.s_addr;
    target_address_holder[0]=dest_ip[0];
    target_address_holder[1]=dest_ip[1];
    target_address_holder[2]=dest_ip[2];
    target_address_holder[3]=dest_ip[3];
    
    if(SOCKET_VERBOSE==1) printf("Opened UDP Socket\n");

    return 0;
}
	
/*
udp_send
*/

int udp_send_string(char* str)
{
   char buffer[BUF_SIZE];
   int sent;
   strcpy(buffer,str);
   sent = sendto( cs,buffer,strlen(buffer), 0,
                  (struct sockaddr*)&target_host_address,
		  sizeof(struct sockaddr));
   if (sent == -1) { return -1; }
   if (SOCKET_VERBOSE==1) printf("Sent string over UDP : %s\b",str);
   return 0 ;
}

