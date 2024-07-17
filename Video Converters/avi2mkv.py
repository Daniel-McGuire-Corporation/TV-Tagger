import os
import subprocess

# Loop through each AVI file in the current directory
for filename in os.listdir():
    if filename.lower().endswith(".avi"):
        input_path = os.path.abspath(filename)
        output_path = os.path.splitext(input_path)[0] + ".mkv"
        subprocess.run(["ffmpeg", "-i", input_path, "-c:v", "copy", "-c:a", "copy", output_path])
