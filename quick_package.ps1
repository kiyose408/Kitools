param(
    [string]$Version = ""
)

$ProjectRoot = $PSScriptRoot
if (-not $ProjectRoot) {
    $ProjectRoot = "D:\Git\Gitee\kitools"
}

$BuildDir = "$ProjectRoot\build\Desktop_Qt_6_9_3_MinGW_64_bit-Release"
$ReleaseDir = "$ProjectRoot\release"
$WindeployQt = "C:\Qt\6.9.3\mingw_64\bin\windeployqt.exe"
$NSISPath = "C:\Program Files (x86)\NSIS\makensis.exe"

function Get-VersionFromCMake {
    $cmakeFile = "$ProjectRoot\CMakeLists.txt"
    $content = Get-Content $cmakeFile -Raw -Encoding UTF8
    if ($content -match 'project\(PCProductivityToolbox VERSION (\d+\.\d+\.\d+)') {
        return $matches[1]
    }
    return "1.0.0"
}

function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $color = switch ($Level) {
        "ERROR" { "Red" }
        "SUCCESS" { "Green" }
        "WARNING" { "Yellow" }
        default { "White" }
    }
    Write-Host "[$timestamp] [$Level] $Message" -ForegroundColor $color
}

function New-DirectoryIfNotExist {
    param([string]$Path)
    if (-not (Test-Path $Path)) {
        New-Item -ItemType Directory -Path $Path -Force | Out-Null
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  PCProductivityToolbox - Quick Package" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

if (-not $Version) {
    $Version = Get-VersionFromCMake
}
Write-Log "Version: $Version"

$SourceExe = "$BuildDir\bin\PCProductivityToolbox.exe"
if (-not (Test-Path $SourceExe)) {
    Write-Log "Build file not found: $SourceExe" "ERROR"
    exit 1
}

$VersionDir = "$ReleaseDir\v$Version\PCProductivityToolbox"
New-DirectoryIfNotExist $VersionDir

Write-Log "Copying executable..."
Copy-Item -Path $SourceExe -Destination $VersionDir -Force

Write-Log "Deploying Qt dependencies..."
$FullExePath = "$VersionDir\PCProductivityToolbox.exe"
& $WindeployQt $FullExePath --release

if ($LASTEXITCODE -ne 0) {
    Write-Log "Qt deployment failed" "ERROR"
    exit 1
}
Write-Log "Qt dependencies deployed" "SUCCESS"

Write-Log "Copying documentation..."
$docsDest = "$VersionDir\docs"
New-DirectoryIfNotExist $docsDest

if (Test-Path "$ProjectRoot\docs") {
    Copy-Item -Path "$ProjectRoot\docs\*" -Destination $docsDest -Recurse -Force
}

if (Test-Path "$ProjectRoot\README.md") {
    Copy-Item -Path "$ProjectRoot\README.md" -Destination "$VersionDir\README.md" -Force
}

$changelogContent = @"
# Changelog

## v$Version ($((Get-Date).ToString("yyyy-MM-dd")))

### New Features
- Desktop Notes module (independent windows, Markdown rendering, reminder, color classification)

### Improvements
- Updated documentation to v$Version
- Optimized modular architecture

---

## v1.0.0 (2026-02-19)

### Initial Release

#### Desktop Timer
- Count up/count down mode
- Always on top, draggable
- Time arrival notification

#### Desktop Todo
- Task add/delete/complete
- Dark mode toggle
- Data persistence
"@
Set-Content -Path "$VersionDir\CHANGELOG.md" -Value $changelogContent -Encoding UTF8 -Force
Write-Log "Documentation copied" "SUCCESS"

Write-Log "Creating ZIP package..."
$zipPath = "$ReleaseDir\PCProductivityToolbox_v${Version}.zip"
if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}
Compress-Archive -Path $VersionDir -DestinationPath $zipPath -Force
$zipSize = (Get-Item $zipPath).Length / 1MB
Write-Log "ZIP created: $zipSize MB" "SUCCESS"

if (Test-Path $NSISPath) {
    Write-Log "Creating installer..."
    $nsisScript = "$ProjectRoot\installer.nsi"
    if (Test-Path $nsisScript) {
        (Get-Content $nsisScript -Encoding UTF8) -replace '!define PRODUCT_VERSION "[\d.]+"', "!define PRODUCT_VERSION `"$Version`"" | Set-Content $nsisScript -Encoding UTF8

        $tempBuildDir = "$ReleaseDir\temp_build"
        New-DirectoryIfNotExist $tempBuildDir
        Copy-Item -Path "$VersionDir\*" -Destination $tempBuildDir -Recurse -Force

        $tempNsi = "$tempBuildDir\installer.nsi"
        Copy-Item -Path $nsisScript -Destination $tempNsi -Force

        Push-Location $tempBuildDir
        & $NSISPath /V3 "installer.nsi"
        Pop-Location

        $nsisOutput = "$ReleaseDir\PCProductivityToolbox_v${Version}_Setup.exe"
        $generatedInstaller = Get-ChildItem -Path $tempBuildDir -Filter "*.exe" | Where-Object { $_.Name -like "*Setup*" } | Select-Object -First 1

        if ($generatedInstaller) {
            Move-Item -Path $generatedInstaller.FullName -Destination $nsisOutput -Force
            $installerSize = (Get-Item $nsisOutput).Length / 1MB
            Write-Log "Installer created: $installerSize MB" "SUCCESS"
        }

        Remove-Item $tempBuildDir -Recurse -Force
    }
}
else {
    Write-Log "NSIS not found, skipping installer" "WARNING"
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Package Complete!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Release Dir: $VersionDir" -ForegroundColor Green
Write-Host "ZIP:         $zipPath ($([math]::Round($zipSize, 2)) MB)" -ForegroundColor Green

$installerFile = "$ReleaseDir\PCProductivityToolbox_v${Version}_Setup.exe"
if (Test-Path $installerFile) {
    $installerSize = (Get-Item $installerFile).Length / 1MB
    Write-Host "Installer:   $installerFile ($([math]::Round($installerSize, 2)) MB)" -ForegroundColor Green
}
Write-Host ""
