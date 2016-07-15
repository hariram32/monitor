#include <header.h>

/*--------------------------------------------------------------------*/
/*--- checksum - standard 1s complement checksum                   ---*/
/*--------------------------------------------------------------------*/
unsigned short checksum ( void *b, int len )
{	
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *( unsigned char * ) buf;
	sum = ( sum >> 16 ) + ( sum & 0xFFFF );
	sum += ( sum >> 16 );
	result = ~sum;
	return result;
}

/*--------------------------------------------------------------------*/
/*--- display - present echo info                                  ---*/
/*--------------------------------------------------------------------*/
void display ( void *buf, int bytes )
{	
	int i;
	struct iphdr *ip = buf;
	struct icmphdr *icmp = buf + ip -> ihl * 4;
	char src_addr [ 16 ];
	char dst_addr [ 16 ];
	
	inet_ntop ( AF_INET, ( void * )&( ip -> saddr ), src_addr, 16 );
	inet_ntop ( AF_INET, ( void * )&( ip -> daddr ), dst_addr, 16 );
	printf( "IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s dst=%s\n",
		ip -> version, ip -> ihl * 4, ntohs( ip -> tot_len ), ip -> protocol,
		ip -> ttl, src_addr, dst_addr );
	printf ( "ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]\n",
		icmp -> type, icmp -> code, ntohs ( icmp -> checksum ),
		icmp -> un.echo.id, icmp -> un.echo.sequence );
	if ( icmp -> type == 3 )
	{	
		struct icmphdr *icmp_original = buf + ip -> ihl * 4 + sizeof ( struct icmphdr ) + ip -> ihl * 4;
		//struct icmphdr *icmp_original = buf + 112 - 28;
		printf ( "Original ICMP header: id[%d] seq[%d]\n",
			icmp_original -> un.echo.id, icmp_original -> un.echo.sequence );
		//printf ( "Original ICMP header...\n" );
	}
}

int ping ( int index )
{
	struct sockaddr *addr = g_hosts [ index ] . address_info -> ai_addr;
	struct packet pckt;
	int i;
	
	
	bzero ( &pckt, sizeof ( pckt ) );
	pckt.hdr.type = ICMP_ECHO;
	pckt.hdr.un.echo.id = ( unsigned short ) index;
	for ( i = 0; i < sizeof ( pckt.msg ) - 1; i++ )
		pckt.msg [ i ] = i + '0';
	pckt.msg [ i ] = 0;
	//printf ( "Sequence: %d\n", g_hosts [ index ] . send_count + 1 );
	pckt.hdr.un.echo.sequence = g_hosts [ index ] . send_count + 1;
	pckt.hdr.checksum = checksum ( &pckt, sizeof ( pckt ) );
	if ( sendto ( g_sd_send, &pckt, sizeof ( pckt ), 0, addr, sizeof ( *addr ) ) <= 0 )
	{
		perror ( "sendto" );
		return 1;
	}
	else
	{
		return 0;
	}
}

void receive ( )
{
	//printf ( "Receive: sd[%d]\n", g_sd );
	struct sockaddr_in addr;
	int bytes, len = sizeof ( addr );
	unsigned char buf [ 1024 ];

	bzero ( buf, sizeof ( buf ) );
	bytes = recvfrom ( g_sd_receive, buf, sizeof ( buf ), 0, ( struct sockaddr* )&addr, &len );
	if ( bytes > 0 )
	{
		//display ( buf, bytes );
		process_receive ( buf, bytes );
	}
}

int create_socket ( struct addrinfo *address_info )
{
	int sd = 0;
	const int val=255;

	sd = socket ( address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol );
	if ( sd < 0 )
	{
		perror( "socket" );
		return;
	}
	if ( setsockopt ( sd, IPPROTO_IP, IP_TTL, &val, sizeof ( val ) ) != 0 )
		perror ( "Set TTL option" );
	if ( fcntl ( sd, F_SETFL, O_NONBLOCK ) != 0 )
		perror ( "Request nonblocking I/O" );
	
	return sd;
}

int free_heap_data ( )
{
	int i;
	for ( i = 0; i < g_host_count; i++ )
	{
		freeaddrinfo ( g_hosts [ i ] . address_info );
	}
}