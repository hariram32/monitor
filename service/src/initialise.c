#include <header.h>

int initialise_timers ( )
{
	int i;
	clock_t current_clock = clock ( );
	g_reinitialise_timer = current_clock;
	for ( i = 0; i < g_host_count; i++ )
	{
		g_hosts [ i ] . clock_at_send = current_clock;
	}	

}

int initialise_hosts ( )
{
	int i;
	
	memset ( &g_hosts, 0, MAX_HOSTS * sizeof ( struct host_info ) );
	
	g_host_count = read_csv ( HOSTS_DATA_PATH, g_host_data );
	
	if ( g_host_count == 0 )
	{
		printf ( "No host data. Now exiting.\n" );
		exit ( 1 );
	}
	
	printf ( "Host count: %d\n", g_host_count );
	
	for ( i = 0; i < g_host_count; i++ )
	{
		strcpy ( g_hosts [ i ].canonical_name, g_host_data [ i ][ 0 ] );
	}
	struct protoent *proto = NULL;
	proto = getprotobyname ( "ICMP" );
	struct addrinfo hints;
	memset ( &hints, 0, sizeof ( struct addrinfo ) );
	hints.ai_family = AF_INET;    			/* IPv4 */
	hints.ai_socktype = SOCK_RAW; 			/* Raw socket */
	hints.ai_flags = AI_PASSIVE;    		/* For wildcard IP address */
	hints.ai_protocol = proto -> p_proto; 	/* ICMP protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	
	g_sd_send = create_socket ( &hints );
	g_sd_receive = create_socket ( &hints );
	
	/* Initialise host variables for all monitored hosts */
	for ( i = 0; i < g_host_count; i++ )
	{
		g_hosts [ i ] . previous_status = uninitialised;
		g_hosts [ i ] . status = uninitialised;
		g_hosts [ i ] . ping_status = sending;
		int s = 0;
		s = getaddrinfo ( g_hosts [ i ] . canonical_name, NULL, &hints, &( g_hosts [ i ] . address_info ) );
		if ( s != 0 ) 
		{
			 printf ( "getaddrinfo: %s\n", gai_strerror ( s ) );
			 g_hosts [ i ] . status = not_found;
			 continue;
		}
		char host_ip [ 16 ];
		inet_ntop ( AF_INET, ( void * ) &( ( ( struct sockaddr_in * ) ( g_hosts [ i ] . address_info -> ai_addr ) ) -> sin_addr . s_addr ), host_ip, 16 );
		strcpy ( g_hosts [ i ] . ip, host_ip );
		printf ( "Get address info: %s\n", g_hosts [ i ] . canonical_name );
		printf ( "IP address: %s\n", host_ip );
	}
}