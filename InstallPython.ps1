# Check if winget is available
if (Get-Command winget -ErrorAction SilentlyContinue) {
    # Install Python using winget
    winget install Python.Python
    Write-Output "Python installed via winget."
} else {
    # Open Microsoft Store
    Start-Process ms-windows-store://pdp/?ProductId=9NJ46SX7X90P

    # Display message box
    Add-Type -AssemblyName PresentationFramework
    [System.Windows.MessageBox]::Show("Python was not found. Please update App Installer via the Microsoft Store to install Python.", "Python Installation", "OK", "Information")
}
