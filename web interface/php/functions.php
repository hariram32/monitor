<?php
/*
Functions
*/

/* Open database connection */
function &open_database_connection ( $server, $database, $username, $password )
{
	try 
	{
		$database_connection = new PDO ( "mysql:host=$server;dbname=$database", $username, $password );
		// set the PDO error mode to exception
		$database_connection->setAttribute ( PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );
		return $database_connection;
	}
	catch ( PDOException $e )
	{
		echo ( "Connection failed: " . $e->getMessage ( ) );
		exit ( );
	}
}

function get_percentage_on ( $db_connection )
{
	$sql_select_query = 
		"SELECT COUNT(id) AS count, status
		FROM hosts_status
		GROUP BY status";
		
	$on_count = 0;
	$total = 0;
	$result = 0;
	$index = 0;
		
	foreach ( $db_connection->query( $sql_select_query ) as $data_row )
	{
		$index++;
		$count = $data_row[ 'count' ];
		$status = $data_row[ 'status' ];
		$total += $count;
		if ( $status == 1 )
		{
			$on_count = $count;
		}		
	}

	if ( $total != 0 )
	{
		$result = $on_count * 100 / $total;
	}
	
	return number_format ( (float) $result, 1, '.', '' );
}

function get_percentage_colour ( $percentage )
{
	$colour = "#DDDDDDD";
	if ( $percentage > 95 )
	{
		$colour = "#00FF00";
	}
	else if ( $percentage > 85 )
	{
		$colour = "#FFBF00";
	}
	else
	{
		$colour = "#FF5555";
	}
	return $colour;
}

function get_status_count ( $db_connection )
{
	$sql_select_query = 
		"SELECT COUNT(id) AS count, status
		FROM hosts_status
		GROUP BY status";
		
	$index = 0;
	$results = array ( );
		
	foreach ( $db_connection->query( $sql_select_query ) as $data_row )
	{
		$status = $data_row [ 'status' ];
		$results [ $status ] = $data_row [ 'count' ];
	}

	/*echo ( "<pre>" );
	print_r ( $results );
	echo ( "</pre>" );*/
	
	return $results;
}

function get_item_percentage ( $item_count, $total )
{
	if ( $total != 0 )
	{
		$result = $item_count * 100 / $total;
	}
	
	return number_format ( (float) $result, 10, '.', '' );
}

?>