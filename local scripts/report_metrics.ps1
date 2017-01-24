function query-sql {
Param (
  [Parameter(
  Mandatory = $true,
  ParameterSetName = '',
  ValueFromPipeline = $true)]
  [string]$Query
) 

	$MySQLAdminUserName = 'monitor'
	$MySQLAdminPassword = 'Mo123456'
	$MySQLDatabase = 'monitor'
	$MySQLHost = 'ecws002'
	$ConnectionString = "server=" + $MySQLHost + ";port=3306;uid=" + $MySQLAdminUserName + ";pwd=" + $MySQLAdminPassword + ";database="+$MySQLDatabase

	Try {
	  [void][System.Reflection.Assembly]::LoadWithPartialName("MySql.Data")
	  $Connection = New-Object MySql.Data.MySqlClient.MySqlConnection
	  $Connection.ConnectionString = $ConnectionString
	  $Connection.Open()

	  $Command = New-Object MySql.Data.MySqlClient.MySqlCommand($Query, $Connection)
	  $DataAdapter = New-Object MySql.Data.MySqlClient.MySqlDataAdapter($Command)
	  $DataSet = New-Object System.Data.DataSet
	  $RecordCount = $dataAdapter.Fill($dataSet, "data")
	  $DataSet.Tables[0]
	 }

	Catch {
	  Write-Host "ERROR : Unable to run query : $query `n$Error[0]"
	}

	Finally {
	  $Connection.Close()
	}
	
}

function get-load {

	$processor_usage = Get-Counter '\Processor(_Total)\% Processor Time' `
		| Select-Object -ExpandProperty countersamples `
		| Select-Object -Property cookedvalue

	return $processor_usage.CookedValue
	
}

function get-availablememory {

	$physical_memory = (systeminfo | Select-String 'Total Physical Memory:').ToString().Split(':')[1].Trim().Split(' ')[0]
	$memory_usage = Get-Counter '\Memory\Available MBytes'

}

$host_name =  $env:computername
write-host $host_name
$select_query = "SELECT id, canonical_name FROM hosts_details WHERE canonical_name=`'$host_name`'"
$select_result = query-sql( $select_query )
$id = $select_result.id
write-host $id
if ( -not [string]::IsNullOrEmpty( $id ) ) {
	$load = 0
	Try {
		$load = get-load
	}
	Catch {
		Write-Host "ERROR : Bad data"
	}
	if ( -not [string]::IsNullOrEmpty( $load ) ) {
		$timestamp = get-date -f 'yyyy-MM-dd HH:mm:ss'
		write-host "$host_name - $load - $timestamp"
		$update_query = "UPDATE hosts_status SET cpu_usage=$load, cpu_timestamp=`'$timestamp`' WHERE id=$id"
		query-sql( $update_query )
	}
}