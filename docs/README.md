# TV Tagger [<sub>Leave a name suggestion here!</sub>](https://github.com/Daniel-McGuire-Corporation/TV-Tagger/discussions/2)

> [!IMPORTANT] 
> Requires ffmpeg in PATH.
> 
> Doesn't work with multiple seasons (yet.)

> [!WARNING]
> This is in alpha, please keep a backup of what you are converting in case something goes wrong! (I am not responsible for corrupted files or deleted files!)

## How to use:

1. Gather Files. (Video files should be named like this: 'SHOW NAME S0E0.(mkv/mp4)')
 - Put the Python Script, template.json, and all episodes (MKV or MP4) in the same folder. (Only one type of video file at a time!)

2. Setup.
 - Update the 'Episode Outline' template.json as needed (If you need help, take a look at the examples.)

3. Run the script.
```
python tag.py -json Path.To.Your.Outline.json
```
 - (defaults to ```template.json``` if no args are used.)
