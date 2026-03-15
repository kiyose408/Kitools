# PC效率工具箱 - 自动化构建与打包脚本
#
# 功能：
#   1. 自动读取版本号
#   2. 构建Release版本
#   3. 部署Qt依赖
#   4. 复制文档文件
#   5. 创建安装包
#
# 使用方法：
#   .\build.ps1 [-Version "x.x.x"] [-SkipBuild] [-SkipNSIS]
#
# 参数：
#   -Version    : 指定版本号（默认从CMakeLists.txt读取）
#   -SkipBuild  : 跳过构建步骤
#   -SkipNSIS   : 跳过创建安装包步骤

param(
    [string]$Version = "",
    [switch]$SkipBuild,
    [switch]$SkipNSIS
)

# ==================== 配置区域 ====================

# 项目路径
$ProjectRoot = $PSScriptRoot
if (-not $ProjectRoot) {
    $ProjectRoot = "D:\Git\Gitee\kitools"
}

# Qt构建工具路径
$QtBinPath = "C:\Qt\6.9.3\mingw_64\bin"
$QtToolsPath = "C:\Qt\Tools\mingw1310_64\bin"
$WindeployQt = "C:\Qt\6.9.3\mingw_64\bin\windeployqt.exe"
$MakeQt = "C:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe"

# NSIS安装路径
$NSISPath = "C:\Program Files (x86)\NSIS"
$Makensis = "$NSISPath\makensis.exe"

# 构建目录
$BuildDir = "$ProjectRoot\build\Desktop_Qt_6_9_3_MinGW_64_bit-Release"
$ReleaseDir = "$ProjectRoot\release"

# ==================== 函数定义 ====================

function Get-VersionFromCMake {
    $cmakeFile = "$ProjectRoot\CMakeLists.txt"
    $content = Get-Content $cmakeFile -Raw
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
        Write-Log "Created directory: $Path"
    }
}

function Copy-DocsFiles {
    param([string]$DestPath)

    Write-Log "Copying documentation files..."

    # 复制docs目录
    $docsSrc = "$ProjectRoot\docs"
    $docsDest = "$DestPath\docs"
    if (Test-Path $docsSrc) {
        New-DirectoryIfNotExist $docsDest
        Copy-Item -Path "$docsSrc\*" -Destination $docsDest -Recurse -Force
        Write-Log "Copied docs folder" "SUCCESS"
    }

    # 复制README.md
    $readmeSrc = "$ProjectRoot\README.md"
    $readmeDest = "$DestPath\README.md"
    if (Test-Path $readmeSrc) {
        Copy-Item -Path $readmeSrc -Destination $readmeDest -Force
        Write-Log "Copied README.md" "SUCCESS"
    }

    # 创建CHANGELOG.md
    $changelogDest = "$DestPath\CHANGELOG.md"
    $changelogContent = @"
# 更新日志

## v$Version ($((Get-Date).ToString("yyyy-MM-dd")))

### 新增功能
- 桌面便签模块（独立窗口、Markdown渲染、提醒功能、颜色分类）

### 改进优化
- 更新项目文档至v$Version版本
- 优化模块化架构设计
"@
    Set-Content -Path $changelogDest -Value $changelogContent -Force
    Write-Log "Created CHANGELOG.md" "SUCCESS"
}

function Build-Project {
    Write-Log "Starting project build..."

    # 设置环境变量
    $env:PATH = "$QtBinPath;$QtToolsPath;$env:PATH"

    # 检查构建目录
    if (-not (Test-Path $BuildDir)) {
        New-DirectoryIfNotExist $BuildDir
    }

    # CMake配置
    Write-Log "Running CMake configuration..."
    Push-Location $BuildDir
    try {
        cmake -G "MinGW Makefiles" `
            -DCMAKE_C_COMPILER="$QtToolsPath\bin\gcc.exe" `
            -DCMAKE_PREFIX_PATH="C:\Qt\6.9.3\mingw_64" `
            $ProjectRoot

        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed"
        }
        Write-Log "CMake configuration completed" "SUCCESS"
    }
    finally {
        Pop-Location
    }

    # MinGW编译
    Write-Log "Running MinGW build..."
    Push-Location $BuildDir
    try {
        & "$QtToolsPath\bin\mingw32-make.exe" -j$(Get-CimInstance Win32_ComputerSystem).NumberOfProcessors

        if ($LASTEXITCODE -ne 0) {
            throw "Build failed"
        }
        Write-Log "Build completed" "SUCCESS"
    }
    finally {
        Pop-Location
    }
}

function Deploy-QtDependencies {
    param([string]$ExePath)

    Write-Log "Deploying Qt dependencies..."

    # 设置环境变量
    $env:PATH = "$QtBinPath;$QtToolsPath;$env:PATH"

    # 运行windeployqt
    & $WindeployQt $ExePath --release

    if ($LASTEXITCODE -ne 0) {
        Write-Log "windeployqt failed" "ERROR"
        return $false
    }

    Write-Log "Qt dependencies deployed" "SUCCESS"
    return $true
}

function New-Installer {
    param(
        [string]$Version,
        [string]$InstallerPath
    )

    Write-Log "Creating NSIS installer..."

    # NSIS脚本路径
    $nsisScript = "$ProjectRoot\installer.nsi"

    if (-not (Test-Path $nsisScript)) {
        Write-Log "NSIS script not found: $nsisScript" "ERROR"
        return $false
    }

    # 运行makensis
    & $Makensis /V3 $nsisScript

    if ($LASTEXITCODE -ne 0) {
        Write-Log "NSIS failed" "ERROR"
        return $false
    }

    Write-Log "Installer created: $InstallerPath" "SUCCESS"
    return $true
}

# ==================== 主流程 ====================

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  PC效率工具箱 - 自动化构建与打包脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 获取版本号
if (-not $Version) {
    $Version = Get-VersionFromCMake
}
Write-Log "Target version: $Version"

# 创建发布目录
$VersionDir = "$ReleaseDir\$Version\PCProductivityToolbox"
New-DirectoryIfNotExist $VersionDir

# 构建步骤
if (-not $SkipBuild) {
    Build-Project

    # 复制可执行文件
    $exePath = "$BuildDir\bin\PCProductivityToolbox.exe"
    if (-not (Test-Path $exePath)) {
        $exePath = "$BuildDir\PCProductivityToolbox.exe"
    }

    if (-not (Test-Path $exePath)) {
        Write-Log "Executable not found: $exePath" "ERROR"
        exit 1
    }

    Copy-Item -Path $exePath -Destination $VersionDir -Force
    Write-Log "Copied executable to release folder" "SUCCESS"

    # 部署Qt依赖
    $fullExePath = "$VersionDir\PCProductivityToolbox.exe"
    Deploy-QtDependencies -ExePath $fullExePath
}
else {
    Write-Log "Skipping build step" "WARNING"
}

# 复制文档文件
Copy-DocsFiles -DestPath $VersionDir

# 创建安装包
if (-not $SkipNSIS) {
    if (-not (Test-Path $Makensis)) {
        Write-Log "NSIS not found at: $Makensis" "ERROR"
        Write-Log "Skipping installer creation" "WARNING"
    }
    else {
        $installerName = "PCProductivityToolbox_v${Version}_Setup.exe"
        $installerOutput = "$ReleaseDir\$installerName"

        # 更新NSIS脚本版本号
        $nsisScript = "$ProjectRoot\installer.nsi"
        if (Test-Path $nsisScript) {
            (Get-Content $nsisScript) -replace '!define VERSION "[\d.]+"', "!define VERSION `"$Version`"" | Set-Content $nsisScript
        }

        New-Installer -Version $Version -InstallerPath $installerOutput
    }
}
else {
    Write-Log "Skipping NSIS step" "WARNING"
}

# 输出最终信息
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  构建完成!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "发布目录: $VersionDir" -ForegroundColor Green
Write-Host "版本号:   $Version" -ForegroundColor Green

$zipFile = "$ReleaseDir\PCProductivityToolbox_v${Version}.zip"
if (Test-Path $zipFile) {
    $zipSize = (Get-Item $zipFile).Length / 1MB
    Write-Host "ZIP包:    $zipFile ($([math]::Round($zipSize, 2)) MB)" -ForegroundColor Green
}

$installerFile = "$ReleaseDir\PCProductivityToolbox_v${Version}_Setup.exe"
if (Test-Path $installerFile) {
    $installerSize = (Get-Item $installerFile).Length / 1MB
    Write-Host "安装包:   $installerFile ($([math]::Round($installerSize, 2)) MB)" -ForegroundColor Green
}

Write-Host ""
