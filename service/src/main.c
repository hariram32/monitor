#include <header.h>

int g_sd_send;
int g_sd_receive;
struct host_info g_hosts [ MAX_HOSTS ];
int g_host_count = 0;
char g_host_data [ MAX_HOSTS ][ MAX_CSV_COLUMNS ][ MAX_CSV_ENTRY_LENGTH ];
MYSQL *g_conn;
clock_t g_reinitialise_timer = 0;
int g_consecutive_database_updates = 0;

main ( )
{
	while ( 1 )
	{
		/* Initialise host information */
		initialise_hosts ( );
		
		/* Initialise database */
		initialise_database ( );
		
		/* Initialise timers */
		initialise_timers ( );
		
		// Main loop
		while ( !process_reinitialise_timer ( ) )
		{	
			unsigned long long current_clock = 0;
			current_clock = clock ( );
			printf ( "Main loop started at clock %lld\n", current_clock );
			process_timers ( );
			current_clock = clock ( );
			printf ( "Hosts processing started at clock %lld\n", current_clock );
			process_hosts ( );
			current_clock = clock ( );
			printf ( "Database processing started at clock %lld\n", current_clock );
			process_database ( );
			sleep ( 1 );
			current_clock = clock ( );
			printf ( "Main loop finished at clock %lld\n", current_clock );
		}
		
		/* Close database connection */
		close_database ( );
		
		/* Release memory */
		free_heap_data ( );	
	}
}