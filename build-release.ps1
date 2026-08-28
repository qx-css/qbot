$ErrorActionPreference = "Stop"

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "Cannot find vswhere.exe. Please install Visual Studio 2022/2026 Build Tools."
}

$installPath = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
if (-not $installPath) {
    throw "Cannot find MSBuild. Please install Visual Studio Build Tools with C++ workload."
}

$msbuild = Join-Path $installPath "MSBuild\Current\Bin\MSBuild.exe"
if (-not (Test-Path $msbuild)) {
    throw "Cannot find MSBuild.exe at $msbuild."
}

& $msbuild QdBot.vcxproj /t:restore /p:RestorePackagesConfig=true /p:Configuration=Release /p:Platform=x64
if ($LASTEXITCODE -ne 0) {
    throw "NuGet restore failed with exit code $LASTEXITCODE."
}

& $msbuild QdBot.sln /p:Configuration=Release /p:Platform=x64
if ($LASTEXITCODE -ne 0) {
    throw "MSBuild failed with exit code $LASTEXITCODE."
}

$exe = Get-ChildItem -Recurse -Filter QdBot.exe | Where-Object {
    $_.FullName -match "\\x64\\Release\\"
} | Select-Object -First 1

if (-not $exe) {
    throw "Build finished but QdBot.exe was not found."
}

Write-Host "Built: $($exe.FullName)"
