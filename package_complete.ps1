<#
.SYNOPSIS
    PC效率工具箱 v1.4.0 - 完整打包脚本（增强版）
    
.DESCRIPTION
    功能：
    1. 自动查找Release版本可执行文件
    2. 使用windeployqt部署Qt依赖库（增强参数）
    3. 手动复制可能遗漏的关键DLL
    4. 复制文档和资源文件
    5. 创建ZIP压缩包
    6. 使用NSIS创建安装程序
    7. 验证所有依赖是否完整
    
.PARAMETER Version
    指定版本号（默认从CMakeLists.txt读取）
    
.PARAMETER BuildType
    构建类型：Release 或 Debug（默认 Release）
    
.PARAMETER SkipValidation
    跳过依赖验证步骤
    
.EXAMPLE
    .\package_complete.ps1 -Version "1.4.0"
#>

param(
    [string]$Version = "",
    [string]$BuildType = "Release",
    [switch]$SkipValidation
)

$ErrorActionPreference = "Stop"

$ProjectRoot = $PSScriptRoot
if (-not $ProjectRoot) {
    $ProjectRoot = "D:\Git\Gitee\kitools"
}

# Qt路径配置（根据实际安装位置）
$QtVersion = "6.9.3"
$QtCompiler = "mingw_64"
$QtPath = "C:\Qt\$QtVersion\$QtCompiler"
$QtBinPath = "$QtPath\bin"
$WindeployQt = "$QtBinPath\windeployqt.exe"

# MinGW路径
$MinGWPath = "$QtPath\bin"

# NSIS路径
$NSISPath = "C:\Program Files (x86)\NSIS\makensis.exe"

# 输出目录
$ReleaseDir = "$ProjectRoot\release"
$PackageDir = "$ReleaseDir\v${Version}\PCProductivityToolbox"

function Write-Log {
    param(
        [Parameter(Mandatory)]
        [string]$Message,
        [string]$Level = "INFO"
    )
    
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $color = switch ($Level) {
        "ERROR" { "Red" }
        "SUCCESS" { "Green" }
        "WARNING" { "Yellow" }
        default { "White" }
    }
    Write-Host "[$timestamp] [$Level] $Message" -ForegroundColor $color
}

function New-DirectorySafe {
    param([string]$Path)
    if (-not (Test-Path $Path)) {
        New-Item -ItemType Directory -Path $Path -Force | Out-Null
        Write-Log "Created directory: $Path" "SUCCESS"
    }
}

function Get-VersionFromCMake {
    $cmakeFile = "$ProjectRoot\CMakeLists.txt"
    if (Test-Path $cmakeFile) {
        $content = Get-Content $cmakeFile -Raw -Encoding UTF8
        if ($content -match 'project\(PCProductivityToolbox VERSION (\d+\.\d+\.\d+)') {
            return $matches[1]
        }
    }
    return "1.4.0"
}

function Find-Executable {
    Write-Log "Searching for Release executable..."
    
    $buildDir = "$ProjectRoot\build"
    if (-not (Test-Path $buildDir)) {
        Write-Log "Build directory not found!" "ERROR"
        return $null
    }
    
    $exeFiles = Get-ChildItem -Path $buildDir -Recurse -Filter "PCProductivityToolbox.exe" -ErrorAction SilentlyContinue | Where-Object {
        $_.FullName -notmatch "CMakeFiles" -and $_.FullName -match $BuildType
    }
    
    if ($exeFiles) {
        $exeFile = $exeFiles | Select-Object -First 1
        Write-Log "Found executable: $($exeFile.FullName)" "SUCCESS"
        return $exeFile.FullName
    }
    
    Write-Log "Executable not found!" "ERROR"
    return $null
}

function Deploy-QtDependencies {
    param([string]$ExePath)
    
    Write-Log "Deploying Qt dependencies..."
    
    $env:PATH = "$QtBinPath;$MinGWPath;$env:PATH"
    
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo.FileName = $WindeployQt
    $process.StartInfo.Arguments = @"
"$ExePath" `
    --release `
    --no-translations `
    --no-system-d3d-compiler `
    --no-opengl-sw `
    --no-virtualkeyboard `
    --concurrent `
    --xml `
    --sql `
    --network `
    --svg `
    --printsupport
"@
    $process.StartInfo.UseShellExecute = $false
    $process.StartInfo.RedirectStandardOutput = $true
    $process.StartInfo.RedirectStandardError = $true
    $process.Start() | Out-Null
    
    $stdout = $process.StandardOutput.ReadToEnd()
    $stderr = $process.StandardError.ReadToEnd()
    $process.WaitForExit()
    
    if ($stdout) { Write-Log $stdout "INFO" }
    if ($stderr) { Write-Log $stderr "WARNING" }
    
    if ($process.ExitCode -eq 0) {
        Write-Log "windeployqt completed successfully" "SUCCESS"
    } else {
        Write-Log "windeployqt exited with code: $($process.ExitCode)" "WARNING"
    }
    
    Write-Log "Copying additional Qt libraries..."
    
    $additionalDlls = @(
        "Qt6Charts.dll",
        "Qt6DataVisualization.dll",
        "Qt6Sql.dll",
        "Qt6Xml.dll",
        "Qt6Concurrent.dll",
        "Qt6PrintSupport.dll",
        "Qt6OpenGL.dll",
        "Qt6OpenGLWidgets.dll"
    )
    
    $destDir = Split-Path $ExePath -Parent
    
    foreach ($dll in $additionalDlls) {
        $srcPath = "$QtBinPath\$dll"
        if (Test-Path $srcPath) {
            $destPath = "$destDir\$dll"
            if (-not (Test-Path $destPath)) {
                Copy-Item -Path $srcPath -Destination $destPath -Force
                Write-Log "Copied: $dll" "SUCCESS"
            } else {
                Write-Log "Already exists: $dll" "INFO"
            }
        }
    }
    
    return $true
}

function Copy-MinGWRuntime {
    param([string]$DestPath)
    
    Write-Log "Copying MinGW runtime libraries..."
    
    $mingwDlls = @(
        "libgcc_s_seh-1.dll",
        "libstdc++-6.dll",
        "libwinpthread-1.dll"
    )
    
    foreach ($dll in $mingwDlls) {
        $srcPath = "$MinGWPath\$dll"
        if (Test-Path $srcPath) {
            $destFile = "$DestPath\$dll"
            if (-not (Test-Path $destFile)) {
                Copy-Item -Path $srcPath -Destination $destFile -Force
                Write-Log "Copied MinGW: $dll" "SUCCESS"
            }
        }
    }
}

function Copy-Documentation {
    param([string]$DestPath)
    
    Write-Log "Copying documentation and resources..."
    
    $docsSrc = "$ProjectRoot\docs"
    $docsDest = "$DestPath\docs"
    if (Test-Path $docsSrc) {
        New-DirectorySafe $docsDest
        Copy-Item -Path "$docsSrc\*" -Destination $docsDest -Recurse -Force
        Write-Log "Copied docs folder" "SUCCESS"
    }
    
    $docFiles = @("README.md", "CHANGELOG.md", "LICENSE")
    foreach ($file in $docFiles) {
        $srcFile = "$ProjectRoot\$file"
        if (Test-Path $srcFile) {
            Copy-Item -Path $srcFile -Destination "$DestPath\$file" -Force
            Write-Log "Copied $file" "SUCCESS"
        }
    }
}

function Create-ZIPPackage {
    param([string]$SourceDir, [string]$OutputPath)
    
    Write-Log "Creating ZIP archive..."
    
    if (Test-Path $OutputPath) {
        Remove-Item $OutputPath -Force
    }
    
    Compress-Archive -Path $SourceDir -DestinationPath $OutputPath -CompressionLevel Optimal -Force
    
    if (Test-Path $OutputPath) {
        $sizeMB = [math]::Round((Get-Item $OutputPath).Length / 1MB, 2)
        Write-Log "ZIP created: $OutputPath ($sizeMB MB)" "SUCCESS"
        return $true
    } else {
        Write-Log "Failed to create ZIP" "ERROR"
        return $false
    }
}

function Create-NSISInstaller {
    param([string]$SourceDir, [string]$Version)
    
    Write-Log "Creating NSIS installer..."
    
    if (-not (Test-Path $NSISPath)) {
        Write-Log "NSIS not found at: $NSISPath" "WARNING"
        return $false
    }
    
    $nsisScript = "$ProjectRoot\installer.nsi"
    if (-not (Test-Path $nsisScript)) {
        Write-Log "NSIS script not found: $nsisScript" "ERROR"
        return $false
    }
    
    $nsisDest = "$SourceDir\installer.nsi"
    Copy-Item -Path $nsisScript -Destination $nsisDest -Force
    
    $content = Get-Content $nsisDest -Raw -Encoding UTF8
    $content = $content -replace '!define PRODUCT_VERSION "[\d.]+"', "!define PRODUCT_VERSION `"$Version`""
    Set-Content -Path $nsisDest -Value $content -Encoding UTF8 -NoNewline
    
    Push-Location $SourceDir
    try {
        $process = New-Object System.Diagnostics.Process
        $process.StartInfo.FileName = $NSISPath
        $process.StartInfo.Arguments = "/V4 installer.nsi"
        $process.StartInfo.UseShellExecute = $false
        $process.StartInfo.RedirectStandardOutput = $true
        $process.StartInfo.RedirectStandardError = $true
        $process.Start() | Out-Null
        
        $stdout = $process.StandardOutput.ReadToEnd()
        $stderr = $process.StandardError.ReadToEnd()
        $process.WaitForExit()
        
        if ($stdout) {
            $stdout -split "`n" | Where-Object { $_ -and $_.Trim() } | ForEach-Object {
                Write-Log $_ "INFO"
            }
        }
        
        if ($process.ExitCode -eq 0) {
            $installerFile = Get-ChildItem -Path $SourceDir -Filter "*Setup.exe" | Select-Object -First 1
            
            if ($installerFile) {
                $outputInstaller = "$ReleaseDir\PCProductivityToolbox_v${Version}_Setup.exe"
                Move-Item -Path $installerFile.FullName -Destination $outputInstaller -Force
                
                $sizeMB = [math]::Round((Get-Item $outputInstaller).Length / 1MB, 2)
                Write-Log "Installer created: $outputInstaller ($sizeMB MB)" "SUCCESS"
                return $true
            }
        } else {
            Write-Log "NSIS compilation failed with exit code: $($process.ExitCode)" "ERROR"
            return $false
        }
    }
    finally {
        Pop-Location
    }
}

function Test-Dependencies {
    param([string]$ExePath)
    
    if ($SkipValidation) {
        Write-Log "Skipping dependency validation" "WARNING"
        return $true
    }
    
    Write-Log "Validating dependencies..."
    
    $exeDir = Split-Path $ExePath -Parent
    $dllFiles = Get-ChildItem -Path $exeDir -Filter "*.dll" -ErrorAction SilentlyContinue
    
    Write-Log "Found $($dllFiles.Count) DLL files in package" "INFO"
    
    $requiredDlls = @(
        "Qt6Core.dll",
        "Qt6Gui.dll",
        "Qt6Widgets.dll",
        "Qt6Network.dll",
        "Qt6Svg.dll",
        "libgcc_s_seh-1.dll",
        "libstdc++-6.dll",
        "libwinpthread-1.dll"
    )
    
    $missingDlls = @()
    foreach ($dll in $requiredDlls) {
        $dllPath = "$exeDir\$dll"
        if (-not (Test-Path $dllPath)) {
            $missingDlls += $dll
        }
    }
    
    if ($missingDlls.Count -gt 0) {
        Write-Log "Missing required DLLs: $($missingDlls -join ', ')" "ERROR"
        return $false
    }
    
    $platformsDir = "$exeDir\platforms"
    if (-not (Test-Path "$platformsDir\qwindows.dll")) {
        Write-Log "Missing qwindows.dll in platforms directory" "ERROR"
        return $false
    }
    
    Write-Log "All required dependencies are present" "SUCCESS"
    return $true
}

# ==================== 主流程 ====================
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  PC Productivity Toolbox v1.4.0" -ForegroundColor Cyan
Write-Host "  Complete Packaging Tool" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

if (-not $Version) {
    $Version = Get-VersionFromCMake
}
Write-Log "Target version: $Version"
Write-Log "Build type: $BuildType"

$exePath = Find-Executable
if (-not $exePath) {
    Write-Log "Please build the project first!" "ERROR"
    exit 1
}

New-DirectorySafe $PackageDir

Write-Log "Copying executable to package directory..."
Copy-Item -Path $exePath -Destination $PackageDir -Force
$targetExe = "$PackageDir\PCProductivityToolbox.exe"
Write-Log "Executable copied" "SUCCESS"

Deploy-QtDependencies -ExePath $targetExe
Copy-MinGWRuntime -DestPath $PackageDir
Copy-Documentation -DestPath $PackageDir

if (-not (Test-Dependencies -ExePath $targetExe)) {
    Write-Log "Dependency validation failed!" "ERROR"
    Write-Log "Please check the package manually" "WARNING"
}

$zipPath = "$ReleaseDir\PCProductivityToolbox_v${Version}.zip"
Create-ZIPPackage -SourceDir $PackageDir -OutputPath $zipPath

Create-NSISInstaller -SourceDir $PackageDir -Version $Version

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Packaging Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Version:   $Version" -ForegroundColor White
Write-Host "BuildType: $BuildType" -ForegroundColor White
Write-Host ""
Write-Host "Output Directory:" -ForegroundColor Yellow
Write-Host "  $PackageDir" -ForegroundColor Green
Write-Host ""
Write-Host "Generated Files:" -ForegroundColor Yellow

if (Test-Path $zipPath) {
    $zipSize = [math]::Round((Get-Item $zipPath).Length / 1MB, 2)
    Write-Host "  [ZIP]  $zipPath ($zipSize MB)" -ForegroundColor Green
}

$installerPath = "$ReleaseDir\PCProductivityToolbox_v${Version}_Setup.exe"
if (Test-Path $installerPath) {
    $installerSize = [math]::Round((Get-Item $installerPath).Length / 1MB, 2)
    Write-Host "  [EXE]  $installerPath ($installerSize MB)" -ForegroundColor Green
}

Write-Host ""
Write-Host "Note: Test the executable before distribution!" -ForegroundColor Yellow
Write-Host ""
