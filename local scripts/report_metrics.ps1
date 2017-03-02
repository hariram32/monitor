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

function get-freememory {

	$memory_available = Get-Counter '\Memory\Available MBytes' `
		| Select-Object -ExpandProperty countersamples `
		| Select-Object -Property cookedvalue
	return $memory_available.CookedValue
	
}

function get-diskqueue {

	$disk_queue = Get-Counter '\PhysicalDisk(_Total)\Current Disk Queue Length' `
		| Select-Object -ExpandProperty countersamples `
		| Select-Object -Property cookedvalue
	return $disk_queue.CookedValue
	
}

function get-disktime {

	$disk_time = Get-Counter '\PhysicalDisk(_Total)\% Disk Time' `
		| Select-Object -ExpandProperty countersamples `
		| Select-Object -Property cookedvalue
	return $disk_time.CookedValue
	
}

$host_name =  $env:computername
write-host $host_name
$CS = Gwmi Win32_ComputerSystem
write-host $CS.UserName
$logged_on_user = [regex]::escape( $CS.UserName )
$select_query = "SELECT id, canonical_name FROM hosts_details WHERE canonical_name=`'$host_name`'"
$select_result = query-sql( $select_query )
$id = $select_result.id
write-host $id
if ( -not [string]::IsNullOrEmpty( $id ) ) {

	$load = 0
	$memory_available = 0
	#$disk_time = 0
	$disk_queue = 0
	Try {
		$load = get-load
	}
	Catch {
		Write-Host "ERROR : Bad CPU data"
	}
	Try {
		$memory_available = get-freememory
	}
	Catch {
		Write-Host "ERROR : Bad memory data"
	}
	<#Try {
		$disk_time = get-disktime
	}
	Catch {
		Write-Host "ERROR : Bad disk data"
	}#>
	Try {
		$disk_queue = get-diskqueue
	}
	Catch {
		Write-Host "ERROR : Bad disk data"
	}
	if ( -not [string]::IsNullOrEmpty( $load ) ) {
		$timestamp = get-date -f 'yyyy-MM-dd HH:mm:ss'
		write-host "Load: $host_name - $load - $timestamp"
		$update_query = "UPDATE hosts_status SET cpu_usage=$load, cpu_timestamp=`'$timestamp`' WHERE id=$id"
		query-sql( $update_query )
	}
	if ( -not [string]::IsNullOrEmpty( $memory_available ) ) {
		$physical_memory = (systeminfo | Select-String 'Total Physical Memory:').ToString().Split(':')[1].Trim().Split(' ')[0]
		$physical_memory = $physical_memory.replace(",","")
		$free_percentage = ($memory_available*100/$physical_memory)
		write-host "Memory: Physical - $physical_memory, Available - $memory_available, %Free - $free_percentage"
		$timestamp = get-date -f 'yyyy-MM-dd HH:mm:ss'
		write-host "Memory: $host_name - $free_percentage - $timestamp"
		$update_query = "UPDATE hosts_status SET memory_usage=$free_percentage, memory_timestamp=`'$timestamp`' WHERE id=$id"
		query-sql( $update_query )
	}
	if ( -not [string]::IsNullOrEmpty( $disk_queue ) ) {
		$timestamp = get-date -f 'yyyy-MM-dd HH:mm:ss'
		write-host "Disk: $host_name - $disk_queue - $timestamp"
		$update_query = "UPDATE hosts_status SET disk_usage=$disk_queue, disk_timestamp=`'$timestamp`' WHERE id=$id"
		query-sql( $update_query )
	}
	<#if ( -not [string]::IsNullOrEmpty( $disk_time ) ) {
		$timestamp = get-date -f 'yyyy-MM-dd HH:mm:ss'
		write-host "Disk: $host_name - $disk_time - $timestamp"
		$update_query = "UPDATE hosts_status SET disk_usage=$disk_time, disk_timestamp=`'$timestamp`' WHERE id=$id"
		query-sql( $update_query )
	}#>
	if ( [string]::IsNullOrEmpty( $logged_on_user ) ) {
		$logged_on_user = "none"
	}
	$update_query = "UPDATE hosts_status SET logged_on_user=`'$logged_on_user`' WHERE id=$id"
	query-sql( $update_query )

}