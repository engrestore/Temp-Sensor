# flash.ps1
# Flashes the compiled AHT10Project binary to the TM4C123G LaunchPad via
# OpenOCD + the ICDI's JTAG interface, then reminds you to press RESET
# since OpenOCD's own automatic reset step doesn't work on this board's
# ICDI implementation (SRST error every time -- harmless, just means the
# chip is left halted after programming instead of auto-restarting).
#
# Usage: just run this script after rebuilding in Keil (make sure the
# .bin path below matches your project's output).
 
$openocd = "C:\Espressif\tools\openocd-esp32\v0.12.0-esp32-20260424\openocd-esp32\bin\openocd.exe"
$binFile = "C:/Workspace_Keil/AHT10Project/New.bin"
 
& $openocd `
    -f interface/ti-icdi.cfg `
    -c "transport select hla_jtag" `
    -f target/ti/stellaris.cfg `
    -c "program $binFile verify reset exit 0x00000000"
 
Write-Host ""
Write-Host "================================================================" -ForegroundColor Yellow
Write-Host " Flash complete. Press the physical RESET button on the board" -ForegroundColor Yellow
Write-Host " now -- OpenOCD's own reset step doesn't work on this ICDI, so" -ForegroundColor Yellow
Write-Host " the chip is currently sitting halted, not running your code." -ForegroundColor Yellow
Write-Host "================================================================" -ForegroundColor Yellow