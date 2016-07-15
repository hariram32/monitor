#include <header.h>

void connect_to_mysql ( )
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *server = "localhost";
	char *user = "monitor";
	char *password = "Mo123456"; /* set me first */
	char *database = "monitor";
	g_conn = mysql_init ( NULL );
	/* Connect to database */
	if ( !mysql_real_connect ( g_conn, server, user, password, database, 0, NULL, 0 ) ) 
	{
		fprintf ( stderr, "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}
	/* send SQL query */
	if ( mysql_query ( g_conn, "show tables" ) ) 
	{
		fprintf ( stderr, "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}
	res = mysql_use_result ( g_conn );
	/* output table name */
	printf ( "MySQL Tables in mysql database:\n" );
	while ( ( row = mysql_fetch_row ( res ) ) != NULL )
	printf ( "%s \n", row [ 0 ] );
	/* close connection */
	mysql_free_result ( res );
}

int initialise_database ( )
{
	connect_to_mysql ( );
	int i;
	char truncate_query [ MAX_SQL_QUERY_LENGTH ];
	sprintf 
	( 
		truncate_query,
		"TRUNCATE table hosts_status"
	);
	if ( mysql_query ( g_conn, truncate_query ) ) 
	{
		printf ( "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}
	sprintf 
	( 
		truncate_query,
		"TRUNCATE table hosts_details"
	);
	if ( mysql_query ( g_conn, truncate_query ) ) 
	{
		printf ( "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}

	for ( i = 0; i < g_host_count; i++ )
	{
		char insert_query [ MAX_SQL_QUERY_LENGTH ];
		sprintf 
		( 
			insert_query,
			"INSERT INTO hosts_status (id, status) \
				VALUES ('%d', '%d')",
			i, 0
		);
		if ( mysql_query ( g_conn, insert_query ) ) 
		{
			printf ( "%s\n", mysql_error ( g_conn ) );
			exit ( 1 );
		}
		
		char *canonical_name = g_host_data [ i ] [ 0 ];
		char *ip = g_hosts [ i ] . ip;
		char *display_name = g_host_data [ i ] [ 1 ];
		int grid_y = strtol ( g_host_data [ i ] [ 2 ], NULL, 10 );
		int grid_x = strtol ( g_host_data [ i ] [ 3 ], NULL, 10 );
		sprintf 
		( 
			insert_query,
			"INSERT INTO hosts_details (id, canonical_name, ip, display_name, grid_y, grid_x) \
				VALUES ('%d', '%s', '%s', '%s', '%d', '%d')",
			i, canonical_name, ip, display_name, grid_y, grid_x
		);
		if ( mysql_query ( g_conn, insert_query ) ) 
		{
			printf ( "%s\n", mysql_error ( g_conn ) );
			exit ( 1 );
		}
	}
}

int process_database ( )
{
	int i;
	for ( i = 0; i < g_host_count; i++ )
	{
		host_status status = g_hosts [ i ] . status;
		char update_query [ MAX_SQL_QUERY_LENGTH ];
		sprintf
		(
			update_query,
			"UPDATE hosts_status SET status='%d' WHERE id=%d"
			, status, i
		);
		if ( mysql_query ( g_conn, update_query ) ) 
		{
			printf ( "%s\n", mysql_error ( g_conn ) );
			exit ( 1 );
		}
	}
}

int close_database ( )
{
	mysql_close ( g_conn );
}