<?php
/* The relative path that needs to be taken to the webroot from the current location of this file */
$path_to_root = '../';

/* Global variables used by all scripts */
include ( "{$path_to_root}php/globals.php" );


/* All functions are included here */
include ( "{$path_to_root}php/functions.php" );

/* Initialise global variables */

$g_hosts_details_query_results = array ( );
$g_hosts_details_query_results_counter = 0;
$g_hosts_status_query_results = array ( );
$g_hosts_status_query_results_counter = 0;
$g_percentage_on = 0;
$g_percentage_colour = "#DDDDDD";
$g_uninitialised_colour = "#FFFFFF";
$g_stale_dns_colour = "#AAAAAA";
$g_not_responding_colour = "#EE7070";
$g_just_off_colour = "#FFBF00";
$g_responding_colour = "#8CAA37";
$g_high_load_colour = "#8CAAFF";
$g_summary_results = NULL;
$g_total_hosts = 0;
$g_uninitialised_width = 0;
$g_up_width = 0;
$g_not_responding_width = 0;
$g_stale_dns_width = 0;
$g_just_lost_contact_width = 0;
$g_index = 0;

$g_site = 2;
$test_host = 0;

/* Grab configuration data from ini files */
$g_database_connection_data = parse_ini_file ( "{$db_config_location}", true );

/* Initialise connection variables with configuration data */
$db_server = $g_database_connection_data[ 'connection' ][ 'server' ];
$db_database = $g_database_connection_data[ 'connection' ][ 'database' ];
$db_username = $g_database_connection_data[ 'connection' ][ 'username' ];
$db_password = $g_database_connection_data[ 'connection' ][ 'password' ];

/* Open connection to database */
$g_database_connection = open_database_connection ( $db_server, $db_database, $db_username, $db_password );


/* SQL to return all the hosts in the database */
$sql_select_query = 
	"SELECT id, canonical_name, ip, display_name, grid_y, grid_x
	FROM hosts_details
	WHERE site = $g_site";

foreach ( $g_database_connection->query( $sql_select_query ) as $data_row )
{
	$id = $data_row[ 'id' ];
	$canonical_name = $data_row[ 'canonical_name' ];
	$ip = $data_row[ 'ip' ];
	$display_name = $data_row[ 'display_name' ];
	$grid_y = $data_row[ 'grid_y' ];
	$grid_x = $data_row[ 'grid_x' ];
	/* Store results in array for sorting later */
	$g_hosts_details_query_results[ $g_hosts_details_query_results_counter ][ 'id' ] = $id;
	$g_hosts_details_query_results[ $g_hosts_details_query_results_counter ][ 'canonical_name' ] = $canonical_name;
	$g_hosts_details_query_results[ $g_hosts_details_query_results_counter ][ 'ip' ] = $ip;
	$g_hosts_details_query_results[ $g_hosts_details_query_results_counter ][ 'display_name' ] = $display_name;
	$g_hosts_details_query_results[ $g_hosts_details_query_results_counter ][ 'grid_y' ] = $grid_y;
	$g_hosts_details_query_results[ $g_hosts_details_query_results_counter ][ 'grid_x' ] = $grid_x;
	
	if ( $id == 1364 )
	{
		$test_host = $canonical_name;
	}
	
	/* Increment the results counter */
	$g_hosts_details_query_results_counter++;
}

/* SQL to return the status of all the hosts */
$sql_select_query = 
	"SELECT id, status, cpu_usage, memory_usage, disk_usage, cpu_timestamp, logged_on_user
	FROM hosts_status
	WHERE id IN
			(SELECT id
				FROM hosts_details
				WHERE site = $g_site
				)";
	
foreach ( $g_database_connection->query( $sql_select_query ) as $data_row )
{
	$id = $data_row[ 'id' ];
	$status = $data_row[ 'status' ];
	$cpu_usage = $data_row[ 'cpu_usage' ] . '%';
	$memory_usage = $data_row[ 'memory_usage' ] . '%';
	$disk_usage = $data_row[ 'disk_usage' ];
	$cpu_timestamp = $data_row[ 'cpu_timestamp' ];
	$logged_on_user = $data_row[ 'logged_on_user' ];
	if ( $cpu_timestamp == 0 )
	{
		$cpu_usage = "unknown";
		$memory_usage = "unknown";
		$disk_usage = "unknown";
		$cpu_timestamp = "never";
		$logged_on_user = "unknown";
	}

	/* Store results in array for sorting later */
	$g_hosts_status_query_results[ $g_hosts_status_query_results_counter ][ 'id' ] = $id;
	$g_hosts_status_query_results[ $g_hosts_status_query_results_counter ][ 'status' ] = $status;
	$g_hosts_status_query_results[ $g_hosts_status_query_results_counter ][ 'cpu_usage' ] = $cpu_usage;
	$g_hosts_status_query_results[ $g_hosts_status_query_results_counter ][ 'memory_usage' ] = $memory_usage;
	$g_hosts_status_query_results[ $g_hosts_status_query_results_counter ][ 'disk_usage' ] = $disk_usage;
	$g_hosts_status_query_results[ $g_hosts_status_query_results_counter ][ 'cpu_timestamp' ] = $cpu_timestamp;
	$g_hosts_status_query_results[ $g_hosts_status_query_results_counter ][ 'logged_on_user' ] = $logged_on_user;

	/* Increment the results counter */
	$g_hosts_status_query_results_counter++;
}

$g_summary_results = get_status_count ( $g_database_connection );
foreach ( $g_summary_results as $count )
{
	$g_total_hosts += $count;
}

for ( $g_index = 0; $g_index < 6; $g_index++ )
{
	if ( array_key_exists ( $g_index, $g_summary_results ) )
	{
		$count = $g_summary_results [ $g_index ];		
	}
	else
	{
		$count = NULL;
	}
	//echo ( "Count: $count" );
	if ( $count != NULL )
	{
		$item_percentage = get_item_percentage ( $count, $g_total_hosts );
		switch ( $g_index )
		{
			case 0:
			{
				$g_uninitialised_width = $item_percentage;
				break;
			}
			case 1:
			{
				$g_up_width = $item_percentage;
				break;
			}
			case 2:
			{
			}
			case 3:
			{
				$g_not_responding_width += $item_percentage;
				break;
			}
			case 4:
			{
				$g_stale_dns_width = $item_percentage;
				break;
			}
			case 5:
			{
				$g_just_lost_contact_width = $item_percentage;
				break;
			}
			default:
			{
			}
		}
	}
}

$g_percentage_on = get_percentage_on ( $g_database_connection );
$g_percentage_colour = get_percentage_colour ( $g_percentage_on, $g_responding_colour, $g_just_off_colour, $g_not_responding_colour );

	echo ( 	"<div id=\"grid_row\" class=\"row\">
			<div class=\"col-sm-12\">" );
	
		echo ( "<table id=\"grid_table\" border=\"1\" class=\"fixed centred\">" );
		
		for ( $i = 0; $i < 30; $i++ )
		{
			echo ( "<tr>" );
			for ( $j = 0; $j < 32; $j++ )
			{
				$colour = "lightgrey";
				$display_text = "";
				$index = 0;
				for ( $k = 0; $k < $g_hosts_details_query_results_counter; $k++ )
				{
					if ( $g_hosts_details_query_results[ $k ][ 'grid_y' ] == $i + 1 && $g_hosts_details_query_results[ $k ][ 'grid_x' ] == $j + 1 )
					{
						$index = $k;
						$id = $g_hosts_status_query_results[ $k ][ 'id' ];
						$status = $g_hosts_status_query_results[ $k ][ 'status' ];
						$cpu_usage = $g_hosts_status_query_results[ $k ][ 'cpu_usage' ];
						$memory_usage = $g_hosts_status_query_results[ $k ][ 'memory_usage' ];
						$disk_usage = $g_hosts_status_query_results[ $k ][ 'disk_usage' ];
						$cpu_timestamp = $g_hosts_status_query_results[ $k ][ 'cpu_timestamp' ];
						$logged_on_user = $g_hosts_status_query_results[ $k ][ 'logged_on_user' ];
						$display_text = $g_hosts_details_query_results[ $k ][ 'display_name' ];
						$canonical_name = $g_hosts_details_query_results[ $k ][ 'canonical_name' ];
						switch ( $status )
						{
							case 0:
							{
								$colour = $g_uninitialised_colour;
								break;
							}
							case 1:
							{
								$red_value = sprintf ( "%02X", 140 );
								$green_value = sprintf ( "%02X", 170 );
								$blue_value = sprintf ( "%02X", $cpu_usage * 2 + 55 );
								$colour = "#". $red_value . $green_value . $blue_value;
								break;
							}
							case 2:
							{
							}
							case 3:
							{
								$colour = $g_not_responding_colour;
								break;
							}
							case 4:
							{
								$colour = $g_stale_dns_colour;
								break;
							}
							case 5:
							{
								$colour = $g_just_off_colour;
								break;
							}
							default:
							{
							}
						}
					}
				}
				echo ( "<td bgcolor=$colour class=\"fixed_height_25\">" );
				echo 
				( "
					<div 
						data-toggle=\"popover\"
						data-placement=\"auto\" 
						data-trigger=\"hover\" 
						data-html=\"true\" 
						title=\"$canonical_name\" 
						data-content=\"<p>User: $logged_on_user</p><p>CPU Usage: $cpu_usage</p><p>Free Memory: $memory_usage</p><p>Disk Usage: $disk_usage</p><p><small>Last Reported: $cpu_timestamp</small></p>\"
					>
				" );
				//$display_text = $test_host;
				echo ( $display_text );
				echo ( "</div>" );
				echo ( "</td>" );
			}
			echo ( "</tr>" );
		}
		
		echo ( "</table>" );
		
	echo 
	( "
			</div>
		</div>
		<div class=\"row statistics_row\">
			<div class=\"col-sm-8\">
				<div id=\"progress_bar\">
					<div class=\"progress_bar_inner_columns\" style=\"background-color:$g_uninitialised_colour; width:$g_uninitialised_width%;\">
						<p></p>
					</div>
					<div class=\"progress_bar_inner_columns\" style=\"background-color:$g_responding_colour; width:$g_up_width%;\">
						<p></p>
					</div>
					<div class=\"progress_bar_inner_columns\" style=\"background-color:$g_just_off_colour; width:$g_just_lost_contact_width%;\">
						<p></p>
					</div>
					<div class=\"progress_bar_inner_columns\" style=\"background-color:$g_not_responding_colour; width:$g_not_responding_width%;\">
						<p></p>
					</div>
					<div class=\"progress_bar_inner_columns\" style=\"background-color:$g_stale_dns_colour; width:$g_stale_dns_width%;\">
						<p></p>
					</div>
					<div class=\"clear\"></div>
				</div>
			</div>
			<div class=\"col-sm-4\">
				<div id=\"percentage\" style=\"background-color:$g_percentage_colour;\">
					<p>$g_percentage_on%</p>
				</div>
			</div>
		</div>
	" );
	
	echo 
	( "
		<div class=\"row key_row\">
			<div class=\"col-sm-2\">
				<div>
					<p>Key:</p>
				</div>
			</div>
			<div class=\"col-sm-2\">
				<div style=\"background-color:$g_responding_colour;\">
					<p>On</p>
				</div>
			</div>
			<div class=\"col-sm-2\">
				<div style=\"background-color:$g_high_load_colour;\">
					<p>High load</p>
				</div>
			</div>
			<div class=\"col-sm-2\">
				<div style=\"background-color:$g_just_off_colour;\">
					<p>Just off</p>
				</div>
			</div>
			<div class=\"col-sm-2\">
				<div style=\"background-color:$g_not_responding_colour;\">
					<p>Not responding</p>
				</div>
			</div>
			<div class=\"col-sm-2\">
				<div style=\"background-color:$g_stale_dns_colour;\">
					<p>Stale DNS</p>
				</div>
			</div>
		</div>
	" );
	
/* Close database connection */
$g_database_connection = null;

?>