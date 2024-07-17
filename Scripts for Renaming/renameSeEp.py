import os
import re

# Define the regex pattern to match 'Show Title Season X Episode X'
pattern = r'(?:(?:S(?:eason)?)?\s*(\d+)[sx]*)(?:(?:E(?:pisode)?)?\s*(\d+))'

# Get all files in the current directory
files = [file for file in os.listdir() if os.path.isfile(file)]

for file in files:
    # Check if the file matches the pattern
    match = re.match(pattern, file)
    if match:
        # Extract components from the filename
        show_title = match.group(0)
        season_number = match.group(1)
        episode_number = match.group(2)
        
        # Form the new filename
        new_filename = f"{show_title} S{season_number.zfill(2)}E{episode_number.zfill(2)}{os.path.splitext(file)[1]}"
        
        # Rename the file
        os.rename(file, new_filename)
        print(f"Renamed {file} to {new_filename}")
