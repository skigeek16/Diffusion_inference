# Build script that links with stable-diffusion.cpp library

Write-Host "=== Building Real Model Wrapper ===" -ForegroundColor Green

# Check if stable-diffusion.cpp is cloned
if (-not (Test-Path "stable-diffusion.cpp")) {
    Write-Host "Error: stable-diffusion.cpp not found!" -ForegroundColor Red
    Write-Host "Please run:" -ForegroundColor Yellow
    Write-Host "  git clone https://github.com/leejet/stable-diffusion.cpp" -ForegroundColor White
    Write-Host "  cd stable-diffusion.cpp" -ForegroundColor White
    Write-Host "  git submodule update --init --recursive" -ForegroundColor White
    Write-Host "  # Then build it following its instructions" -ForegroundColor White
    exit 1
}

# Check if stable-diffusion.cpp is built
if (-not (Test-Path "stable-diffusion.cpp/build")) {
    Write-Host "Error: stable-diffusion.cpp not built!" -ForegroundColor Red
    Write-Host "Please build stable-diffusion.cpp first:" -ForegroundColor Yellow
    Write-Host "  cd stable-diffusion.cpp" -ForegroundColor White
    Write-Host "  mkdir build && cd build" -ForegroundColor White
    Write-Host "  cmake .." -ForegroundColor White
    Write-Host "  cmake --build . --config Release" -ForegroundColor White
    exit 1
}

$SD_CPP_ROOT = "stable-diffusion.cpp"
$SD_INCLUDE = "$SD_CPP_ROOT"
$SD_LIB_DIR = "$SD_CPP_ROOT/build"

Write-Host "`nInclude path: $SD_INCLUDE" -ForegroundColor Cyan
Write-Host "Library path: $SD_LIB_DIR" -ForegroundColor Cyan

# Create build directory
if (-not (Test-Path "build_real")) {
    New-Item -ItemType Directory -Path "build_real" | Out-Null
}

Write-Host "`nCompiling wrapper..." -ForegroundColor Cyan

# Check for g++
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    Write-Host "Using g++..." -ForegroundColor Green
    
    # Compile wrapper
    g++ -c -std=c++17 -O2 -I"$SD_INCLUDE" sd_wrapper.cpp -o build_real/sd_wrapper.o
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to compile wrapper!" -ForegroundColor Red
        exit 1
    }
    
    # Compile example
    g++ -std=c++17 -O2 -I"$SD_INCLUDE" `
        real_model_example.cpp build_real/sd_wrapper.o `
        -L"$SD_LIB_DIR" -lstable-diffusion `
        -o build_real/real_sd_example.exe
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to compile example!" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "g++ not found. Please install MinGW or MSYS2." -ForegroundColor Red
    exit 1
}

Write-Host "`n=== Build Successful! ===" -ForegroundColor Green
Write-Host "Executable: build_real\real_sd_example.exe" -ForegroundColor Yellow

Write-Host "`nTo run with a model:" -ForegroundColor Cyan
Write-Host "  .\build_real\real_sd_example.exe models\sd-v1-5.safetensors" -ForegroundColor White
