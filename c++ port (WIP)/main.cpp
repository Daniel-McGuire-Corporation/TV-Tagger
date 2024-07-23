// main.cpp
#include "file_util.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem> // For filesystem operations
#include "thirdparty/json.hpp"
#include "thirdparty/argparse.hpp"

namespace fs = std::filesystem;

// Convert function integrated directly
void convert() {
    std::vector<std::string> mkv_files = get_files_with_extension(".mkv");

    if (mkv_files.empty()) {
        std::cout << "No MKV files found. Skipping the conversion step." << std::endl;
        return;
    }

    for (const auto& mkv_file : mkv_files) {
        std::string mp4_file = mkv_file.substr(0, mkv_file.find_last_of('.')) + ".mp4";
        std::string command = "ffmpeg -i \"" + mkv_file + "\" -codec copy \"" + mp4_file + "\"";
        if (std::system(command.c_str()) != 0) {
            std::cerr << "Failed to convert " << mkv_file << " to " << mp4_file << std::endl;
            continue;
        }
        std::cout << "Converted " << mkv_file << " to " << mp4_file << "." << std::endl;

        fs::create_directory("original");
        fs::rename(mkv_file, "original/" + mkv_file);
        std::cout << "Moved original " << mkv_file << " to 'original' folder." << std::endl;
    }
}

// Split function integrated directly
std::vector<std::tuple<std::string, std::string, std::string>> split() {
    std::vector<std::tuple<std::string, std::string, std::string>> result;

    // Your logic to split and collect tuples
    // Example logic
    // result.push_back({"example1", "example2", "example3"});
    
    std::cout << "Split function called." << std::endl;
    return result;
}

void update_metadata(const std::string& json_file) {
    std::ifstream ifs(json_file);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open JSON file: " << json_file << std::endl;
        return;
    }

    nlohmann::json data;
    try {
        ifs >> data;
    } catch (const std::exception& e) {
        std::cerr << "Error reading JSON file: " << e.what() << std::endl;
        return;
    }

    std::vector<std::string> mp4_files = get_files_with_extension(".mp4");

    if (mp4_files.empty()) {
        std::cout << "No MP4 files found." << std::endl;
        return;
    }

    for (const auto& mp4_file : mp4_files) {
        try {
            size_t pos = mp4_file.find(" S");
            if (pos == std::string::npos) {
                std::cerr << "Invalid filename format: " << mp4_file << std::endl;
                continue;
            }

            std::string season_episode = mp4_file.substr(pos + 2, mp4_file.find('.') - pos - 2);
            size_t e_pos = season_episode.find('E');
            if (e_pos == std::string::npos) {
                std::cerr << "Invalid season_episode format: " << season_episode << std::endl;
                continue;
            }

            std::string season = season_episode.substr(0, e_pos);
            std::string episode = season_episode.substr(e_pos + 1);

            std::string season_folder = "Season " + std::string(2 - season.length(), '0') + season;

            std::string show_title = data["Show"]["Title"].get<std::string>();
            std::string season_title = data["Show"]["Seasons"]["Season " + season]["Title"].get<std::string>();
            std::string episode_title = data["Show"]["Seasons"]["Season " + season]["Episodes"]["E" + std::string(2 - episode.length(), '0') + episode]["Title"].get<std::string>();
            std::string episode_comment = data["Show"]["Seasons"]["Season " + season]["Episodes"]["E" + std::string(2 - episode.length(), '0') + episode]["Comments"].get<std::string>();

            std::string new_title = show_title + " | " + season_title + " S" + season + "E" + episode + " - " + episode_title;

            fs::create_directory(season_folder);

            std::string modified_mp4 = mp4_file + "_modified.mp4";
            std::string command = "ffmpeg -i \"" + mp4_file + "\" -metadata title=\"" + new_title + "\" -metadata comment=\"" + episode_comment + "\" -c copy \"" + modified_mp4 + "\"";
            if (std::system(command.c_str()) != 0) {
                std::cerr << "Failed to execute ffmpeg command: " << command << std::endl;
                continue;
            }

            fs::rename(modified_mp4, season_folder + "/" + mp4_file);
            fs::rename(season_folder + "/" + mp4_file, season_folder + "/" + mp4_file.substr(0, mp4_file.find("_modified")) + ".mp4");

            std::cout << "Processed " << mp4_file << ": Title='" << new_title << "', Comments='" << episode_comment << "'. Moved to " << season_folder << std::endl;

            fs::remove(mp4_file);
        } catch (const std::exception& e) {
            std::cerr << "Error updating metadata for " << mp4_file << ": " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    struct MyArgs : public argparse::Args {
        std::string &json = arg("json", "Path to JSON file for metadata (default: template.json)");
        bool &help = flag("help", "Show this help message and exit");
    };

    auto args = argparse::parse<MyArgs>(argc, argv);

    if (args.help) {
        args.print(); // prints all variables
        return 0;
    }

    std::string json_file = args.json;

    // Call the functions directly
    convert();
    auto episodes_to_move = split();

    update_metadata(json_file);

    std::cout << "Script execution completed." << std::endl;

    return 0;
}
