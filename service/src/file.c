#include <header.h>

int read_csv ( char *file_path, char hosts_data [ MAX_HOSTS ][ MAX_CSV_COLUMNS ][ MAX_CSV_ENTRY_LENGTH ] )
{
    FILE* stream = fopen ( file_path, "r" );
	if ( stream == NULL )
	{
		printf ( "Error opening hosts data file.\n" );
		return 0;
	}
	printf ( "Reading hosts data...\n" );
	int i, j;
    char line [ MAX_CSV_LINE_LENGTH ];
    for ( i = 0; fgets ( line, MAX_CSV_LINE_LENGTH, stream ) && i < MAX_HOSTS; i++ )
    {
		j = 0;
		/* Make a copy of the scanned line */
		char* tmp = strdup ( line );
		
		/* Replace carriage return and newline or \0 with \0 ( strcspn calculates length of string up to specified character,
		in this case \n - also terminates at \0 which is needed for the final line of the csv assuming no new line character ) */
		tmp [ strcspn ( tmp, "\r\n" ) ] = 0;
 		//printf ( "Line %d: %s  ", i + 1, tmp );
		char *token = strtok ( tmp, "," );
		strcpy ( hosts_data [ i ] [ j ], token );
		printf ( "Token %d: %s ", j, token );
		for ( j = 1; token = strtok ( NULL, "," ), token != NULL && j < MAX_CSV_COLUMNS; j++ )
		{
			printf ( "Token %d: %s ", j, token );
			strcpy ( hosts_data [ i ] [ j ], token );
		}
		printf ( "\n" );
		//printf ( "Hosts data [%d][0]: %s\n", i, hosts_data [ i ] [ 0 ] );
		free ( tmp );
    }
	fclose ( stream );
	return i;
}