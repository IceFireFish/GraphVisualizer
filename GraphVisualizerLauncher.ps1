$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Viewer = Join-Path $Root "build\GraphVisualizerViewer.exe"
Add-Type -AssemblyName System.Windows.Forms
$Picker = New-Object System.Windows.Forms.OpenFileDialog
$Picker.Filter = "C++ source (*.cpp)|*.cpp|All files (*.*)|*.*"
$Picker.Title = "Select a C++ source file"
$Picker.Multiselect = $false

if (!(Test-Path $Viewer)) { throw "Viewer executable is missing." }

# Start the viewer first, as requested.
Start-Process -FilePath $Viewer
Start-Sleep -Seconds 2

if ($Picker.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) {
    Write-Host "No file selected. Existing examples\main.cpp was not changed."
    exit 0
}

$Target = Join-Path $Root "examples\main.cpp"
Copy-Item -LiteralPath $Picker.FileName -Destination $Target -Force
Write-Host "Copied selected source to examples\main.cpp"

$Build = Join-Path $Root "build_mygraph.bat"
$Run = Join-Path $Root "run_mygraph.bat"

$p = Start-Process -FilePath $Build -Wait -PassThru
if ($p.ExitCode -ne 0) { throw "Build failed. See compiler output." }

Start-Process -FilePath $Run -Wait
