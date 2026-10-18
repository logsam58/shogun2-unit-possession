param([string]$Output,[long]$StartUnix,[switch]$Inventory)
$ErrorActionPreference = 'Continue'
if ($Inventory) {
  $data = [ordered]@{ utc=[DateTime]::UtcNow.ToString('o'); cpu=@(Get-CimInstance Win32_Processor | Select-Object Name,NumberOfCores,NumberOfLogicalProcessors,MaxClockSpeed); os=@(Get-CimInstance Win32_OperatingSystem | Select-Object Caption,Version,BuildNumber,TotalVisibleMemorySize); gpu=@(Get-CimInstance Win32_VideoController | Select-Object Name,DriverVersion,AdapterRAM); audio=@(Get-CimInstance Win32_SoundDevice | Select-Object Name,Manufacturer,Status,PNPDeviceID); notes='WMI AdapterRAM may wrap above 4 GiB; use NVIDIA device memory.total. CPU temperature unavailable without a supported sensor provider.' }
} else {
  $since=[DateTimeOffset]::FromUnixTimeSeconds($StartUnix).LocalDateTime.AddMinutes(-2)
  $app=@(Get-WinEvent -FilterHashtable @{LogName='Application';StartTime=$since;Id=1000,1001,1002} -ErrorAction SilentlyContinue | Where-Object {$_.Message -match 'shogun|Empire.Retail'} | Select-Object TimeCreated,Id,ProviderName,Message)
  $system=@(Get-WinEvent -FilterHashtable @{LogName='System';StartTime=$since} -ErrorAction SilentlyContinue | Where-Object {$_.ProviderName -match 'Display|nvlddmkm|WHEA|Resource-Exhaustion|Disk|storahci|stornvme|Audio|Audiosrv|AudioEndpointBuilder|Kernel-PnP|UserPnp' -and ($_.Level -le 3)} | Select-Object TimeCreated,Id,ProviderName,Message)
  $resource=@(Get-WinEvent -FilterHashtable @{LogName='Microsoft-Windows-Resource-Exhaustion-Detector/Operational';StartTime=$since} -ErrorAction SilentlyContinue | Select-Object TimeCreated,Id,ProviderName,Message)
  $data=[ordered]@{utc=[DateTime]::UtcNow.ToString('o');application=$app;system=$system;resource_exhaustion=$resource;query_errors=@($Error | ForEach-Object {$_.ToString()})}
}
$data | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $Output -Encoding UTF8
