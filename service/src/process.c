#include <header.h>

int process_reinitialise_timer ( )
{
	clock_t current_clock = clock ( );
	if ( current_clock - g_reinitialise_timer > REINITIALISE_TIMEOUT )
	{
		g_reinitialise_timer = current_clock;
		return 1;
	}
	else
	{
		return 0;
	}
}

int process_timers ( )
{
	printf ( "Processing timers\n" );
	int i;
	//unsigned long long current_clock = clock ( );
	clock_t current_clock = clock ( );
	for ( i = 0; i < g_host_count; i++ )
	{
		if ( g_hosts [ i ] . status != not_found && g_hosts [ i ] . ping_status == sent )
		{
			//unsigned long long clock_difference = 0, host_clock = g_hosts [ i ] . clock_at_send;
			clock_t host_clock = g_hosts [ i ] . clock_at_send;
			int clock_difference = current_clock - host_clock;
			//int millisecond_time_difference = ( clock_difference * 1000 ) / CLOCKS_PER_SEC;
			/*printf ( "Millisecond time difference for host %d is %d, current clock: %d, host clock: %d\n",
				i, millisecond_time_difference, current_clock, host_clock );*/
			if ( clock_difference > PING_TIMEOUT )
			{
				if ( i == 1 || i == 619 )
				{
					printf ( "Host %d timed out, clock difference: %d\n", i, clock_difference );
				}
				if ( g_hosts [ i ] . status == up || g_hosts [ i ] . status == uninitialised )
				{
					g_hosts [ i ] . previous_status = g_hosts [ i ] . status;
					g_hosts [ i ] . status = just_lost_contact;
				}
				else
				{
					g_hosts [ i ] . previous_status = g_hosts [ i ] . status;
					g_hosts [ i ] . status = timed_out;
				}
				g_hosts [ i ] . ping_status = sending;
				g_hosts [ i ] . clock_at_send = current_clock;
			}
			/*if ( i == 1 || i == 649 )
			{
				printf ( "Clock difference for host %d is %d\n", i, clock_difference );
			}*/
		}
	}	
}

int process_receive ( void *buf, int bytes )
{
	int i;
	struct iphdr *ip = buf;
	struct icmphdr *icmp = buf + ip -> ihl * 4;
	char src_addr [ 16 ];
	char dst_addr [ 16 ];
	int type = icmp -> type;
	int id = icmp -> un.echo.id;
	char *stored_ip_address = 0;
	
	if ( id >= g_host_count )
	{
		printf ( "Error: Received id (%d) exceeds total host count.\n", id );
		return 1;
	}
	
	inet_ntop ( AF_INET, ( void * )&( ip -> saddr ), src_addr, 16 );
	inet_ntop ( AF_INET, ( void * )&( ip -> daddr ), dst_addr, 16 );
	
	switch ( type )
	{
		case 0:
		{
			stored_ip_address = g_hosts [ id ] . ip;
			if ( !strcmp ( stored_ip_address, src_addr ) )
			{
				if ( id == 1 || id == 619 )
				{
					unsigned long long current_clock = clock ( );
					clock_t host_clock = g_hosts [ id ] . clock_at_send;
					int clock_difference = current_clock - host_clock;
					printf ( "Received ping for host %d at clock %lld, clock difference %d\n", id, current_clock, clock_difference );
				}
				g_hosts [ id ] . previous_status = g_hosts [ id ] . status;
				g_hosts [ id ] . status = up;
				g_hosts [ id ] . ping_status = sending;
				g_hosts [ id ] . reply_count++;
			}
			break;
		}
		case 3:
		{
			struct iphdr *ip_original = buf + ip -> ihl * 4 + sizeof ( struct icmphdr );
			struct icmphdr *icmp_original = buf + ip -> ihl * 4 + sizeof ( struct icmphdr ) + ip -> ihl * 4;
			int original_id = icmp_original -> un.echo.id;
			if ( original_id >= g_host_count )
			{
				printf ( "Error: Received id (%d) exceeds total host count.\n", original_id );
				return 1;
			}
			stored_ip_address = g_hosts [ original_id ] . ip;
			char original_src_addr [ 16 ];
			char original_dst_addr [ 16 ];
			inet_ntop ( AF_INET, ( void * )&( ip_original -> saddr ), original_src_addr, 16 );
			inet_ntop ( AF_INET, ( void * )&( ip_original -> daddr ), original_dst_addr, 16 );
			/*printf ( "Host %d unreachable ( original src: %s, original dst: %s, ip: %s )\n",
				original_id, original_src_addr, original_dst_addr, stored_ip_address );*/
			if ( !strcmp ( stored_ip_address, original_dst_addr ) )
			{
				if ( g_hosts [ original_id ] . status == up || g_hosts [ original_id ] . status == uninitialised )
				{
					g_hosts [ original_id ] . previous_status = g_hosts [ original_id ] . status;
					g_hosts [ original_id ] . status = just_lost_contact;
				}
				else
				{
					g_hosts [ original_id ] . previous_status = g_hosts [ original_id ] . status;
					g_hosts [ original_id ] . status = unreachable;
					//printf ( "Status for host %d unreachable\n", original_id );
				}
				g_hosts [ original_id ] . ping_status = sending;
			}			
			break;
		}
		default:
		{
		}
	}
}

int process_hosts ( )
{
	int i, j;
	for ( i = 0; i < g_host_count; i++ )
	{
		if ( i % 100 == 0 )
		{
			process_timers ( );
			sleep ( 1 );
		}
		if ( g_hosts [ i ] . status != not_found )
		{
			if ( g_hosts [ i ] . ping_status == sending )
			{
				if ( !ping ( i ) )
				{
					unsigned long long current_clock = clock ( );
					g_hosts [ i ] . clock_at_send = current_clock;
					g_hosts [ i ] . send_count++;
					g_hosts [ i ] . ping_status = sent;
					if ( i == 1 || i == 619 )
					{
						printf ( "Sending ping for host %d at clock %lld \n", i, current_clock );
					}
				}
			}
		}
		for ( j = 0; j < 2; j++ )
		{		
			receive ( );
		}
		//printf ( "Send count for host %d: %d\n", i, g_hosts [ i ] . send_count );
		//printf ( "Host status for host %d: %d\n", i, g_hosts [ i ] . status );
	}
}