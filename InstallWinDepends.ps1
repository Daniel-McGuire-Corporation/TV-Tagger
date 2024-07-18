# Function to check if a command exists
function CommandExists {
    param (
        [string]$command
    )
    $null -ne (Get-Command $command -ErrorAction SilentlyContinue)
}

Write-Host "Dependency installer for TV-Tagger" -ForegroundColor Blue

# Initialize installation flags
$install_ffmpeg = $false
$install_python = $false

# Check if ffmpeg is installed
Write-Host "Checking for ffmpeg" -ForegroundColor Blue
if (CommandExists "ffmpeg") {
    Write-Host "ffmpeg is already installed." -ForegroundColor Green
} else {
    Write-Host "ffmpeg is not installed." -ForegroundColor Red
    $install_ffmpeg = $true
}

# Check if python is installed
Write-Host "Checking for python" -ForegroundColor Blue
if (CommandExists "python") {
    Write-Host "Python is already installed." -ForegroundColor Green
} else {
    Write-Host "Python is not installed." -ForegroundColor Red
    $install_python = $true
}

# Check if any installations are needed
if ($install_ffmpeg -or $install_python) {
    Write-Host "Checking for package manager" -ForegroundColor Yellow
    # Check if winget is available
    if (CommandExists "winget") {
        Write-Host "Found winget" -ForegroundColor Yellow
        if ($install_ffmpeg) {
            winget install --id Ffmpeg.Ffmpeg -e --silent
            Write-Host "ffmpeg installed via winget." -ForegroundColor Green
        }
        if ($install_python) {
            winget install --id Python.Python.3 -e --silent
            Write-Host "Python installed via winget." -ForegroundColor Green
        }
    } else {
        # Open Microsoft Store
        Start-Process ms-windows-store://pdp/?ProductId=9NJ46SX7X90P

        # Display message box
        Add-Type -AssemblyName PresentationFramework
        [System.Windows.MessageBox]::Show("Python and/or ffmpeg were not found. Please update App Installer via the Microsoft Store to install them.", "Dependency Installation", "OK", "Information")
    }
} else {
    Write-Host "Both ffmpeg and Python are installed." -ForegroundColor Green
}
