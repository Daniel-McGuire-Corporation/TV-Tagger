import os
import json
import shutil
import argparse

def split_special_episodes():
    mp4_files = [filename for filename in os.listdir('.') if filename.lower().endswith('.mp4')]

    episodes_to_move = []  # List to store episodes after splitting

    for mp4_file in mp4_files:
        # Check if the filename follows the pattern 'SHOW S1E4-E5.mp4'
        if ' S' in mp4_file and '-E' in mp4_file:
            try:
                base_name, range_part = mp4_file.rsplit(' ', 1)[0], mp4_file.rsplit(' ', 1)[1].split('-')
                episode_start = int(range_part[0][1:])  # Remove 'E' and convert to int
                episode_end = int(range_part[1][1:])    # Remove 'E' and convert to int

                if episode_end <= episode_start:
                    print(f"Invalid episode range in {mp4_file}. Skipping.")
                    continue

                # Calculate the midpoint to split into two episodes
                midpoint = (episode_start + episode_end) // 2

                # Generate new file names
                episode1_name = f"{base_name} S1E{episode_start}.mp4"
                episode2_name = f"{base_name} S1E{midpoint + 1}.mp4"  # Increment midpoint for second episode

                # Perform the split using ffmpeg
                os.system(f'ffmpeg -i "{mp4_file}" -ss 0 -t {midpoint - episode_start + 1} -c copy "{episode1_name}"')
                os.system(f'ffmpeg -i "{mp4_file}" -ss {midpoint - episode_start + 1} -c copy "{episode2_name}"')

                print(f"Split {mp4_file} into {episode1_name} and {episode2_name}")

                # Add episodes to list to move after updating metadata
                episodes_to_move.append((episode1_name, episode2_name, mp4_file))

            except Exception as e:
                print(f"Error processing {mp4_file}: {e}")

    return episodes_to_move

def convert():
    # Get a list of all MKV files in the current folder
    mkv_files = [filename for filename in os.listdir('.') if filename.lower().endswith('.mkv')]

    if not mkv_files:
        print("No MKV files found. Skipping the conversion step.")
        return
    
    for mkv_file in mkv_files:
        mp4_file = os.path.splitext(mkv_file)[0] + ".mp4"
        # Convert MKV to MP4 using ffmpeg
        os.system(f'ffmpeg -i "{mkv_file}" -codec copy "{mp4_file}"')
        print(f"Converted {mkv_file} to {mp4_file}.")
        # Optionally, move the original MKV file to an 'original' folder
        shutil.move(mkv_file, os.path.join("original", mkv_file))
        print(f"Moved original {mkv_file} to 'original' folder.")

def update_metadata(json_file):
    # Read data from the JSON file
    with open(json_file, 'r') as f:
        data = json.load(f)
        
    # Get a list of all MP4 files in the current folder
    mp4_files = [filename for filename in os.listdir('.') if filename.lower().endswith('.mp4')]
    
    if not mp4_files:
        print("No MP4 files found.")
        return
    
    for mp4_file in mp4_files:
        try:
            _, season_episode = os.path.splitext(mp4_file)[0].split(" S")
            season, episode = season_episode.split("E")
            episode = episode.lstrip("0")  # Remove leading zeros
            
            # Construct season folder name
            season_folder = f"Season {season.zfill(2)}"

            # Construct new metadata tags
            show_title = data["Show"]["Title"]
            season_title = data["Show"]["Seasons"][f"Season {season}"]["Title"]
            episode_data = data["Show"]["Seasons"][f"Season {season}"]["Episodes"][f"E{episode.zfill(2)}"]
            episode_title = episode_data["Title"]
            episode_comment = episode_data["Comments"]
            
            new_title = f"{show_title} | {season_title} S{season}E{episode} - {episode_title}"
            
        except KeyError as e:
            print(f"KeyError: {e}. Skipping {mp4_file}")
            continue
        
        # Create the season folder if it does not exist
        if not os.path.exists(season_folder):
            os.makedirs(season_folder)

        # Modify the MP4 file's metadata using ffmpeg
        modified_mp4 = f"{mp4_file}_modified.mp4"
        os.system(f'ffmpeg -i "{mp4_file}" -metadata title="{new_title}" -metadata comment="{episode_comment}" -c copy "{modified_mp4}"')
        
        # Move the modified file to the season folder instead of the original file
        shutil.move(modified_mp4, os.path.join(season_folder, mp4_file))
        
        # Remove the '_modified' label from the file name
        os.rename(os.path.join(season_folder, mp4_file), os.path.join(season_folder, mp4_file.replace('_modified', '')))
        
        print(f"Processed {mp4_file}: Title='{new_title}', Comments='{episode_comment}'. Moved to {season_folder}")

        # Delete the file after copying
        os.remove(mp4_file)

    # Move split episodes after updating metadata
    episodes_to_move = split_special_episodes()  # Get episodes to move
    for episode1_name, episode2_name, original_file in episodes_to_move:
        shutil.move(episode1_name, os.path.join(season_folder, episode1_name))
        shutil.move(episode2_name, os.path.join(season_folder, episode2_name))
        print(f"Moved {episode1_name} and {episode2_name} to {season_folder}")


def main():
    parser = argparse.ArgumentParser(description="Convert MKV files to MP4, split special episodes, and update metadata from JSON.")
    parser.add_argument("-json", metavar="JSON_FILE", type=str, default="template.json", help="Path to JSON file for metadata (default: template.json)")
    parser.add_argument("-help", action="store_true", help="Show this help message and exit")
    args = parser.parse_args()

    if args.help:
        parser.print_help()
        return
    
    # Step 1: Convert MKV to MP4
    convert()
    
    # Step 2: Split special episodes
    split_special_episodes()
    
    # Step 3: Update metadata from JSON
    update_metadata(args.json)
    
    print("Script execution completed.")

if __name__ == "__main__":
    main()