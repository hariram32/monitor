.\query_webserver_database "SELECT id, canonical_name FROM hosts_details" | `
foreach `
{
	$id = $_.id
	$canonical_name = $_.canonical_name
	$select_query = "SELECT id, status FROM hosts_status WHERE id=$id"
	$select_result = .\query_webserver_database $select_query
	$status = $select_result.status
	#write-host $status
	$load = 0
	if ( $status -eq 1 )
	{
		Try `
		{
			$load = .\get_load.ps1 $canonical_name
		}
		Catch `
		{
			Write-Host "ERROR : Bad data"
		}
		if ( -not [string]::IsNullOrEmpty( $load ) )
		{
			write-host "$canonical_name - $load"
			$update_query = "UPDATE hosts_status SET cpu_usage=$load WHERE id=$id"
			.\query_webserver_database $update_query
		}
	#$processor_load = .\get_load.ps1 $_.canonical_name
	#write-host $processor_load
	#write-host $_.canonical_name
	}
}