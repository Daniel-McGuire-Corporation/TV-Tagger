import os
import re

# Define the regex pattern to match '<Show Name> Season X Episode X - <Episode Title>'
pattern = re.compile(r'^(.*?) Season (\d{1,2}) Episode (\d{1,2}) - .*')

# Get all files in the current directory
files = [file for file in os.listdir() if os.path.isfile(file)]

for file in files:
    # Check if the file matches the pattern
    match = pattern.match(file)
    if match:
        # Extract components from the filename
        show_title = match.group(1).strip()
        season_number = match.group(2).zfill(2)
        episode_number = match.group(3).zfill(2)
        
        # Form the new filename
        new_filename = f"{show_title} S{season_number}E{episode_number}{os.path.splitext(file)[1]}"
        
        # Rename the file
        os.rename(file, new_filename)
        print(f"Renamed {file} to {new_filename}")
