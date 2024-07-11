<sub>I don't even know how there are two example folders :skull:</sub>
# TV Tagger

 Requires FFMPEG in PATH

## How to use:

1. Gather Files. (Video files should be named like this: 'SHOW NAME S0E0.(mkv/mp4)')
 - Put the Python Script, template.json, and all episodes (MKV or MP4) in the same folder. (Only one type of video file at a time!)

2. Setup.
 - Update the 'Episode Outline' template.json as needed (If you need help, take a look at the examples.)

3. Run the script.
 - ```python tag.py -json Path.To.Your.Outline.json``` (defaults to 'template.json')
