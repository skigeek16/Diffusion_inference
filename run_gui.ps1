# Launch Stable Diffusion GUI

Write-Host "================================" -ForegroundColor Cyan
Write-Host " Stable Diffusion GUI Launcher" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# Check if executable exists
$exePath = "build_gui\bin\Release\sd_gui.exe"
if (Test-Path $exePath) {
    Write-Host "Starting GUI..." -ForegroundColor Green
    & $exePath
} else {
    Write-Host "ERROR: Executable not found!" -ForegroundColor Red
    Write-Host "Expected location: $exePath" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Please build the project first:" -ForegroundColor White
    Write-Host "  cd build_gui" -ForegroundColor Gray
    Write-Host "  cmake --build . --config Release" -ForegroundColor Gray
}
