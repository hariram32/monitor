<?php
/* The relative path that needs to be taken to the webroot from the current location of this file */
$path_to_root = './';

/* Global variables used by all scripts */
include ( "{$path_to_root}php/globals.php" );

////////////////////////////////////////
/* Main entry point for daily_checks. */
////////////////////////////////////////

/* All functions are included here */
include ( "{$path_to_root}php/functions.php" );

/* Initialise global variables */
$g_config_data = array ( );
$g_hosts_details_query_results = array ( );
$g_hosts_details_query_results_counter = 0;
$g_hosts_status_query_results = array ( );
$g_hosts_status_query_results_counter = 0;

/* Grab configuration data from ini files */
$g_database_connection_data = parse_ini_file ( "{$db_config_location}", true );
$g_config_data = parse_ini_file ( "{$path_to_root}{$config_file_location}", true );

/* Initialise connection variables with configuration data */
$db_server = $g_database_connection_data[ 'connection' ][ 'server' ];
$db_database = $g_database_connection_data[ 'connection' ][ 'database' ];
$db_username = $g_database_connection_data[ 'connection' ][ 'username' ];
$db_password = $g_database_connection_data[ 'connection' ][ 'password' ];

$config_site_code = $g_config_data[ 'location' ][ 'site' ];
$config_default_user = $g_config_data[ 'location' ][ 'defaultuser' ];

/* Open connection to database */
$g_database_connection = open_database_connection ( $db_server, $db_database, $db_username, $db_password );

?>
<!DOCTYPE html>
<html lang="en">
	<head>
<?php
	include ( "{$path_to_root}php/header.php" );
?>
		<script>
			refresh_grid( );
			setInterval( refresh_grid, 10000 );
		</script>
	</head>
	<body>
		<!--Main content of page-->
		<div class="container">
			<div id="grid">
				<div id="loading">
					<p>Loading...</p>
				</div>
			</div>
		</div>
	</body>
</html>	
<?php
/* Close database connection */
$g_database_connection = null;

?>