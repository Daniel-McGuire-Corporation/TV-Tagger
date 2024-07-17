# Define the regex pattern to match 'Show Title Season X Episode X'
$pattern = '(?:(?:S(?:eason)?)?\s*(\d+)[sx]*)(?:(?:E(?:pisode)?)?\s*(\d+))'

# Get all files in the current directory
$files = Get-ChildItem -File

foreach ($file in $files) {
    # Check if the file matches the pattern
    if ($file.Name -match $pattern) {
        # Extract components from the filename
        $showTitle = $matches[0]
        $seasonNumber = $matches[1]
        $episodeNumber = $matches[2]
        
        # Form the new filename
        $newFilename = "{0} S{1}E{2}{3}" -f $showTitle, $seasonNumber.PadLeft(2, '0'), $episodeNumber.PadLeft(2, '0'), $file.Extension
        
        # Rename the file
        Rename-Item -Path $file.FullName -NewName $newFilename
        Write-Output "Renamed $($file.Name) to $newFilename"
    }
}
