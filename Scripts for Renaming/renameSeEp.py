import os
import re

# Define the regex pattern to match 'Show Title SXXEYY'
pattern = re.compile(r'(.*?)[ ._]*[Ss](\d{1,2})[ ._]*[Ee](\d{1,2})')

# Get all files in the current directory
files = [file for file in os.listdir() if os.path.isfile(file)]

for file in files:
    # Check if the file matches the pattern
    match = pattern.match(file)
    if match:
        # Extract components from the filename
        show_title = match.group(1).strip()
        season_number = match.group(2)
        episode_number = match.group(3)
        
        # Form the new filename
        new_filename = f"{show_title} S{season_number.zfill(2)}E{episode_number.zfill(2)}{os.path.splitext(file)[1]}"
        
        # Rename the file
        os.rename(file, new_filename)
        print(f"Renamed {file} to {new_filename}")
