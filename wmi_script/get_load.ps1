param (
    [string]$computername = "."
)
#$memory_usage = Get-Counter -ComputerName $computername '\Memory\Available MBytes'
$processor_usage = Get-Counter -ComputerName $computername '\Processor(_Total)\% Processor Time' `
	| Select-Object -ExpandProperty countersamples `
    | Select-Object -Property cookedvalue

<#
Get-Counter -ComputerName $computername '\Process(*)\% Processor Time' `
    | Select-Object -ExpandProperty countersamples `
    | Select-Object -Property instancename, cookedvalue `
    | Sort-Object -Property cookedvalue -Descending | Select-Object -First 20 `
    | ft InstanceName,@{L='CPU';E={($_.Cookedvalue/100).toString('P')}} -AutoSize
#>

return $processor_usage.CookedValue