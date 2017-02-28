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
	if ( !mysql_real_connect ( g_conn, server, user, password, database, 0, NULL, CLIENT_MULTI_STATEMENTS ) ) 
	{
		fprintf ( stderr, "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}
	/* send SQL query */
	/*if ( mysql_query ( g_conn, "show tables" ) ) 
	{
		fprintf ( stderr, "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}
	res = mysql_use_result ( g_conn );
	// output table name
	printf ( "MySQL Tables in mysql database:\n" );
	while ( ( row = mysql_fetch_row ( res ) ) != NULL )
	printf ( "%s \n", row [ 0 ] );
	// close connection
	mysql_free_result ( res );*/
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
		char site_code [ MAX_CSV_ENTRY_LENGTH ];
		strcpy ( site_code, g_host_data [ i ][ 4 ] );
		int site_id = 0;
		char select_query [ MAX_SQL_QUERY_LENGTH ];
		sprintf
		(
			select_query,
			"SELECT id FROM sites \
				WHERE code='%s'",
				site_code
		);
		
		if ( mysql_query ( g_conn, select_query ) ) 
		{
			printf ( "%s\n", mysql_error ( g_conn ) );
			exit ( 1 );
		}
		
		MYSQL_RES *result = mysql_store_result ( g_conn );
		  
		if ( result == NULL ) 
		{
			printf ( "%s\n", mysql_error ( g_conn ) );
			exit ( 1 );
		}

		//int num_fields = mysql_num_fields ( result );

		MYSQL_ROW row;
		  
		while ( ( row = mysql_fetch_row ( result ) ) ) 
		{ 
			site_id = strtol ( row [ 0 ], NULL, 10 );
		}
		  
		mysql_free_result ( result );
		  
		sprintf 
		( 
			insert_query,
			"INSERT INTO hosts_details (id, canonical_name, ip, display_name, grid_y, grid_x, site) \
				VALUES ('%d', '%s', '%s', '%s', '%d', '%d', '%d')",
			i, canonical_name, ip, display_name, grid_y, grid_x, site_id
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
	/*
	int i;
	char query_values [ MAX_SQL_VALUES_LENGTH ];
	for ( i = 0; i < g_host_count; i++ )
	{
		char statement_line [ 50 ];
		host_status status = g_hosts [ i ] . status;
		sprintf
		(
			statement_line,
			"( '%d', '%d', '0', '0', '0', '0', '0', '0', '0', '0' ),",
			i, status
		);
		strcat ( query_values, statement_line );
	}
	query_values [ strlen ( query_values ) - 1 ] = 0;
	char insert_query [ MAX_LONG_SQL_QUERY_LENGTH ];
	sprintf
	(
		insert_query,
		"INSERT INTO hosts_status ( \
				id,\
				status,\
				status_timestamp,\
				memory_usage,\
				memory_timestamp,\
				cpu_usage,\
				cpu_timestamp,\
				disk_usage,\
				disk_timestamp,\
				logged_on_user )\
			VALUES\
			%s\
		ON DUPLICATE KEY UPDATE\
			status_timestamp = VALUES( status_timestamp ),\
			memory_usage = VALUES( memory_usage ),\
			memory_timestamp = VALUES( memory_timestamp ),\
			cpu_usage = VALUES( cpu_usage ),\
			cpu_timestamp = VALUES( cpu_timestamp ),\
			disk_usage = VALUES( disk_usage ),\
			disk_timestamp = VALUES( disk_timestamp ),\
			logged_on_user = VALUES( logged_on_user )\
		",
		query_values
	);
	printf ( "Statement: %s\n", insert_query );
	if ( mysql_query ( g_conn, insert_query ) ) 
	{
		printf ( "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}*/
	int i = 0, consecutive_updates = 0;
	for ( i = 0; i < g_host_count; i++ )
	{
		host_status previous_status = g_hosts [ i ] . previous_status;
		host_status status = g_hosts [ i ] . status;
		if ( status != previous_status || g_consecutive_database_updates == MAX_CONSECUTIVE_DIFFERENTIAL_DATABASE_UPDATES )
		{
			consecutive_updates++;
			if ( consecutive_updates % 100 == 0 )
			{
				int j = 0;
				for ( j = 0; j < 10; j++ )
				{		
					receive ( );
				}
				process_timers ( );
			}
			printf( "Updating database status for host %d, previous status: %d, current status: %d\n", i, previous_status, status );
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
		else
		{
			consecutive_updates = 0;
		}
	}
	if ( ++g_consecutive_database_updates > MAX_CONSECUTIVE_DIFFERENTIAL_DATABASE_UPDATES )
	{
			g_consecutive_database_updates = 0;
	}
	/*
	char final_update_query [ MAX_MULTI_STATEMENT_SQL_QUERY_LENGTH ];
	for ( i = 0; i < g_host_count; i++ )
	{
		host_status status = g_hosts [ i ] . status;
		char update_query [ MAX_SQL_QUERY_LENGTH ];
		sprintf
		(
			update_query,
			"UPDATE hosts_status SET status='%d' WHERE id=%d;"
			, status, i
		);
		strcat ( final_update_query, update_query );
	}
	final_update_query [ strlen( final_update_query ) - 1 ] = 0;
	//printf ( "Query: %s\n", final_update_query );
	int query_status = mysql_query ( g_conn, final_update_query );
	if ( query_status ) 
	{
		printf ( "%s\n", mysql_error ( g_conn ) );
		exit ( 1 );
	}*/
	//MYSQL_RES *result = mysql_use_result ( g_conn );
	//mysql_free_result ( result );

	/* process each statement result 
	do {
	  // did current statement return data?
	  MYSQL_RES *result = mysql_store_result ( g_conn );
	  if ( result )
	  {
		// yes; process rows and free the result set
		//process_result_set(mysql, result);
		printf ( "Result freed.\n" );
		mysql_free_result ( result );
	  }
	  else          // no result set or error
	  {
		if ( mysql_field_count ( g_conn ) == 0 )
		{
		  //printf( "%lld rows affected\n", mysql_affected_rows ( g_conn ) );
		}
		else  // some error occurred
		{
		  printf ( "Could not retrieve result set\n" );
		  break;
		}
	  }
	  // more results? -1 = no, >0 = error, 0 = yes (keep looping)
	  if ( ( query_status = mysql_next_result ( g_conn ) ) > 0 )
		printf ( "Could not execute statement\n" );
	} while ( query_status == 0 );	*/
}

int close_database ( )
{
	mysql_close ( g_conn );
}