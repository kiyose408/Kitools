<#
.SYNOPSIS
    PC效率工具箱 - 简化打包脚本
#>

param(
    [string]$Version = "1.4.0",
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Continue"
$ProjectRoot = $PSScriptRoot
if (-not $ProjectRoot) { $ProjectRoot = "D:\Git\Gitee\kitools" }

# Configuration
$QtVersion = "6.9.3"
$QtCompiler = "mingw_64"
$QtBinPath = "C:\Qt\$QtVersion\$QtCompiler\bin"
$ReleaseDir = "$ProjectRoot\release"
$PackageDir = "$ReleaseDir\v${Version}\PCProductivityToolbox"

function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "HH:mm:ss"
    $color = switch ($Level) {
        "ERROR" { "Red" }
        "SUCCESS" { "Green" }
        "WARNING" { "Yellow" }
        default { "White" }
    }
    Write-Host "[$timestamp] $Message" -ForegroundColor $color
}

# Find executable
Write-Log "Searching for executable..."
$buildDir = "$ProjectRoot\build"
$exeFile = Get-ChildItem -Path $buildDir -Recurse -Filter "PCProductivityToolbox.exe" | 
    Where-Object { $_.FullName -notmatch "CMakeFiles" -and $_.FullName -match $BuildType } |
    Select-Object -First 1

if (-not $exeFile) {
    Write-Log "Executable not found!" "ERROR"
    exit 1
}
Write-Log "Found: $($exeFile.FullName)" "SUCCESS"

# Create package directory
if (Test-Path $PackageDir) {
    Remove-Item $PackageDir -Recurse -Force
}
New-Item -ItemType Directory -Path $PackageDir -Force | Out-Null
Write-Log "Created package directory" "SUCCESS"

# Copy executable
Copy-Item $exeFile.FullName $PackageDir -Force
Write-Log "Copied executable" "SUCCESS"

# Run windeployqt
Write-Log "Running windeployqt..."
$env:PATH = "$QtBinPath;$env:PATH"
$windeployqt = "$QtBinPath\windeployqt.exe"
$targetExe = "$PackageDir\PCProductivityToolbox.exe"

try {
    & $windeployqt $targetExe --release --no-translations --no-system-d3d-compiler --no-opengl-sw 2>&1 | Out-Null
} catch {
    # Ignore warnings
}
Write-Log "windeployqt completed" "SUCCESS"

# Copy additional Qt DLLs
Write-Log "Copying additional Qt libraries..."
$qtDlls = @(
    "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "Qt6Network.dll", "Qt6Svg.dll",
    "Qt6Charts.dll", "Qt6Sql.dll", "Qt6Xml.dll", "Qt6Concurrent.dll", "Qt6PrintSupport.dll"
)

foreach ($dll in $qtDlls) {
    $src = "$QtBinPath\$dll"
    $dst = "$PackageDir\$dll"
    if ((Test-Path $src) -and -not (Test-Path $dst)) {
        Copy-Item $src $dst -Force
        Write-Log "  + $dll" "SUCCESS"
    }
}

# Copy MinGW runtime
Write-Log "Copying MinGW runtime..."
$mingwDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
foreach ($dll in $mingwDlls) {
    $src = "$QtBinPath\$dll"
    $dst = "$PackageDir\$dll"
    if ((Test-Path $src) -and -not (Test-Path $dst)) {
        Copy-Item $src $dst -Force
        Write-Log "  + $dll" "SUCCESS"
    }
}

# Copy documentation
Write-Log "Copying documentation..."
$docs = @("README.md", "CHANGELOG.md", "LICENSE")
foreach ($doc in $docs) {
    $src = "$ProjectRoot\$doc"
    if (Test-Path $src) {
        Copy-Item $src "$PackageDir\$doc" -Force
    }
}

if (Test-Path "$ProjectRoot\docs") {
    Copy-Item "$ProjectRoot\docs" "$PackageDir\docs" -Recurse -Force
}

# Verify dependencies
Write-Log "Verifying dependencies..."
$requiredDlls = @(
    "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll",
    "libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll"
)

$missing = @()
foreach ($dll in $requiredDlls) {
    if (-not (Test-Path "$PackageDir\$dll")) {
        $missing += $dll
    }
}

if ($missing.Count -gt 0) {
    Write-Log "Missing DLLs: $($missing -join ', ')" "ERROR"
} else {
    Write-Log "All required DLLs present" "SUCCESS"
}

# Check platforms plugin
if (Test-Path "$PackageDir\platforms\qwindows.dll") {
    Write-Log "Platform plugin present" "SUCCESS"
} else {
    Write-Log "Missing qwindows.dll!" "ERROR"
}

# Create ZIP
Write-Log "Creating ZIP package..."
$zipPath = "$ReleaseDir\PCProductivityToolbox_v${Version}.zip"
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
Compress-Archive -Path $PackageDir -DestinationPath $zipPath -CompressionLevel Optimal
$zipSize = [math]::Round((Get-Item $zipPath).Length / 1MB, 2)
Write-Log "ZIP created: $zipPath ($zipSize MB)" "SUCCESS"

# Create NSIS installer
Write-Log "Creating NSIS installer..."
$nsisPath = "C:\Program Files (x86)\NSIS\makensis.exe"
$nsisScript = "$ProjectRoot\installer.nsi"

if ((Test-Path $nsisPath) -and (Test-Path $nsisScript)) {
    # Copy NSIS script to package dir
    $nsisDest = "$PackageDir\installer.nsi"
    Copy-Item $nsisScript $nsisDest -Force
    
    # Update version in script
    $content = Get-Content $nsisDest -Raw
    $content = $content -replace '!define PRODUCT_VERSION "[\d.]+"', "!define PRODUCT_VERSION `"$Version`""
    Set-Content $nsisDest $content -NoNewline
    
    # Run NSIS
    Push-Location $PackageDir
    try {
        & $nsisPath /V2 installer.nsi 2>&1 | Out-Null
        
        $installerFile = Get-ChildItem "$PackageDir\*Setup.exe" | Select-Object -First 1
        if ($installerFile) {
            $outputInstaller = "$ReleaseDir\PCProductivityToolbox_v${Version}_Setup.exe"
            Move-Item $installerFile.FullName $outputInstaller -Force
            $instSize = [math]::Round((Get-Item $outputInstaller).Length / 1MB, 2)
            Write-Log "Installer created: $outputInstaller ($instSize MB)" "SUCCESS"
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Log "NSIS not found, skipping installer" "WARNING"
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Packaging Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Package directory: $PackageDir" -ForegroundColor Cyan
Write-Host "ZIP file: $zipPath" -ForegroundColor Cyan
Write-Host ""
Write-Host "IMPORTANT: Test the executable before distribution!" -ForegroundColor Yellow
Write-Host ""
