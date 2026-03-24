param(
  [ValidateSet("cpu", "gpu", "all")]
  [string]$Mode = "all"
)

$ErrorActionPreference = "Stop"

function Require-Command([string]$Name) {
  if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
    throw "Comando obrigatório não encontrado: $Name"
  }
}

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Resolve-Path (Join-Path $ScriptDir "..")
$BuildDir = Join-Path $RootDir "build-win"
$OutDir = Join-Path $RootDir "dist\windows-x64"

Write-Host "===== BUILD BACKEND (WINDOWS RELEASE) ====="
Write-Host "Root  : $RootDir"
Write-Host "Build : $BuildDir"
Write-Host "Out   : $OutDir"
Write-Host "Mode  : $Mode"

Require-Command cmake

if ($Mode -eq "gpu") {
  Require-Command nvcc
}

if (Test-Path $BuildDir) {
  Remove-Item $BuildDir -Recurse -Force
}
if (Test-Path $OutDir) {
  Remove-Item $OutDir -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$buildGpu = $Mode -ne "cpu"
$cmakeArgs = @(
  "-S", $RootDir,
  "-B", $BuildDir,
  "-G", "Visual Studio 17 2022",
  "-A", "x64",
  "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake",
  "-DMCLIST_BUILD_GPU=$(if ($buildGpu) { 'ON' } else { 'OFF' })"
)

Write-Host "Configurando projeto com CMake..."
& cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) { throw "Falha na configuração do CMake." }

if ($Mode -eq "cpu") {
  Write-Host "Compilando backend CPU..."
  & cmake --build $BuildDir --config Release --target mc_sim_cpu
  if ($LASTEXITCODE -ne 0) { throw "Falha na compilação do backend CPU." }
} elseif ($Mode -eq "gpu") {
  Write-Host "Compilando backend GPU..."
  & cmake --build $BuildDir --config Release --target mc_sim
  if ($LASTEXITCODE -ne 0) { throw "Falha na compilação do backend GPU." }
} else {
  Write-Host "Compilando backend CPU + GPU..."
  & cmake --build $BuildDir --config Release --target mc_sim_cpu mc_sim
  if ($LASTEXITCODE -ne 0) { throw "Falha na compilação do backend." }
}

$releaseDir = Join-Path $BuildDir "Release"

$cpuBin = Join-Path $releaseDir "mc_sim_cpu.exe"
if (Test-Path $cpuBin) {
  Copy-Item $cpuBin $OutDir -Force
}

$gpuBin = Join-Path $releaseDir "mc_sim.exe"
if (Test-Path $gpuBin) {
  Copy-Item $gpuBin $OutDir -Force
}

if (-not (Test-Path (Join-Path $OutDir "mc_sim_cpu.exe"))) {
  throw "mc_sim_cpu.exe não foi gerado."
}

if (($Mode -eq "gpu" -or $Mode -eq "all") -and -not (Test-Path (Join-Path $OutDir "mc_sim.exe"))) {
  throw "mc_sim.exe não foi gerado."
}

Write-Host "===== DONE ====="
Get-ChildItem $OutDir | Format-Table Name, Length, LastWriteTime -AutoSize
