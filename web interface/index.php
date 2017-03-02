<?php
/* The relative path that needs to be taken to the webroot from the current location of this file */
$path_to_root = './';

/* Global variables used by all scripts */
include ( "{$path_to_root}php/globals.php" );

////////////////////////////////////////
/* Main entry point for Ping Monitor  */
////////////////////////////////////////

$form_value="";

/* Initialise global variables */
$g_row_count = 0;
$g_config_data = array ( );
$g_site_details = null;

/* Grab configuration data from ini files */
$g_database_connection_data = parse_ini_file ( "{$db_config_location}", true );
$g_config_data = parse_ini_file ( "{$path_to_root}{$config_file_location}", true );

/* Initialise connection variables with configuration data */
$db_server = $g_database_connection_data[ 'connection' ][ 'server' ];
$db_database = $g_database_connection_data[ 'connection' ][ 'database' ];
$db_username = $g_database_connection_data[ 'connection' ][ 'username' ];
$db_password = $g_database_connection_data[ 'connection' ][ 'password' ];

$g_url_site= parse_url('SERVER', PHP_URL_QUERY);
//$g_site_details = generate_site_array ( $g_database_connection );

?>
<!DOCTYPE html>
<html lang="en">
	<head>
<?php
	include ( "{$path_to_root}php/header.php" );
?>
	</head>
	<body>
<?php
	if (isset($_POST['site'])) 
	{
		$form_value=$_POST['site'];
		echo
		(
			"<script type=\"text/javascript\">
			function refresh_grid( )
			{
				//window.alert(\"Refreshing\");
				var xhttp = new XMLHttpRequest( );
				xhttp.onreadystatechange = function( )
				{
					if ( xhttp.readyState == 4 && xhttp.status == 200 ) 
					{
						//window.alert(\"Ready state: \" + xhttp.readyState);
						document.getElementById( \"grid\" ).innerHTML = xhttp.responseText;
						$(function () { $(\"[data-toggle='popover']\").popover(); });
					}
					else
					{
						//window.alert(\"Ready state: \" + xhttp.readyState);
					}
				};
				xhttp.open(\"GET\", \"php/grid.php?site=$form_value\", true);
				xhttp.send();
			}
			</script>
			<script>
				refresh_grid( );
				setInterval( refresh_grid, 10000 );
			</script>
			<!--Main content of page-->
			<div class=\"container\">
				<div id=\"grid\">
					<div id=\"loading\">
						<p>Loading...</p>
					</div>
				</div>
			</div>"
		);
	}
	else 
	{ 

		echo 
		( 
			"
			<form action=\"index.php\" method=\"POST\">
				<input type=\"radio\" name=\"site\" value=\"EC\">Emmanuel College
				<input type=\"radio\" name=\"site\" value=\"KA\">Kings Academy
				<input type=\"radio\" name=\"site\" value=\"BA\">Bede Academy
				<input type=\"radio\" name=\"site\" value=\"TA\">Trinity Academy
				<input type=\"radio\" name=\"site\" value=\"SS\">Support Services
				<input type=\"submit\" name=\"submit\" value=\"Go\"/>
			</form>
			"
		);

	}
?>

	</body>
</html>	

