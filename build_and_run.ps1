# Automatic Build and Run Script for SD GUI

Write-Host "Building Stable Diffusion GUI..." -ForegroundColor Cyan
Write-Host ""

# Refresh PATH
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")

# Check CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "CMake not found!" -ForegroundColor Red
    Write-Host "Installing CMake..." -ForegroundColor Yellow
    winget install --id Kitware.CMake
    Write-Host "Please restart PowerShell and run this script again" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "CMake found: $(cmake --version | Select-Object -First 1)" -ForegroundColor Green
Write-Host ""

# Create build directory
if (Test-Path "build_gui") {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    Remove-Item "build_gui" -Recurse -Force
}

New-Item -ItemType Directory -Path "build_gui" | Out-Null
Set-Location "build_gui"

Write-Host "Configuring project with CMake..." -ForegroundColor Cyan
cmake .. -DCMAKE_BUILD_TYPE=Release

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    Set-Location ..
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "Building project (this may take several minutes)..." -ForegroundColor Cyan
cmake --build . --config Release -j

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
    Read-Host "Press Enter to exit"
    exit 1
}

Set-Location ..

Write-Host ""
Write-Host "Build Successful!" -ForegroundColor Green
Write-Host ""

# Check for models
if (-not (Test-Path "models\*.safetensors") -and -not (Test-Path "models\*.gguf")) {
    Write-Host "No models found in models\ directory" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Download a model with:" -ForegroundColor White
    Write-Host "  mkdir models -ErrorAction SilentlyContinue" -ForegroundColor Gray
    Write-Host "  Invoke-WebRequest -Uri 'https://huggingface.co/runwayml/stable-diffusion-v1-5/resolve/main/v1-5-pruned-emaonly.safetensors' -OutFile 'models\sd-v1-5.safetensors'" -ForegroundColor Gray
    Write-Host ""
}

Write-Host "Launching Stable Diffusion GUI..." -ForegroundColor Green
Write-Host ""

# Find and run the executable
$exePaths = @(
    "build_gui\bin\Release\sd_gui.exe",
    "build_gui\bin\sd_gui.exe",
    "build_gui\Release\sd_gui.exe"
)

$found = $false
foreach ($path in $exePaths) {
    if (Test-Path $path) {
        & $path
        $found = $true
        break
    }
}

if (-not $found) {
    Write-Host "Executable not found in expected locations!" -ForegroundColor Red
    Write-Host "Checked:" -ForegroundColor Yellow
    foreach ($path in $exePaths) {
        Write-Host "  $path" -ForegroundColor Gray
    }
}
