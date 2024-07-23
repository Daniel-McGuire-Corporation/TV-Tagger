// main.cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <json/json.h>
#include <cstdlib>
#include <argparse/argparse.hpp>
#include <shlwapi.h>

namespace fs = std::filesystem;

std::vector<std::string> get_files_with_extension(const std::string& extension) {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.path().extension() == extension) {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;
}

std::vector<std::tuple<std::string, std::string, std::string>> split_special_episodes() {
    std::vector<std::string> mp4_files = get_files_with_extension(".mp4");
    std::vector<std::tuple<std::string, std::string, std::string>> episodes_to_move;

    for (const auto& mp4_file : mp4_files) {
        if (mp4_file.find(" S") != std::string::npos && mp4_file.find("-E") != std::string::npos) {
            try {
                size_t pos = mp4_file.rfind(' ');
                std::string base_name = mp4_file.substr(0, pos);
                std::string range_part = mp4_file.substr(pos + 1);
                size_t dash_pos = range_part.find('-');
                int episode_start = std::stoi(range_part.substr(1, dash_pos - 1));
                int episode_end = std::stoi(range_part.substr(dash_pos + 2));

                if (episode_end <= episode_start) {
                    std::cout << "Invalid episode range in " << mp4_file << ". Skipping." << std::endl;
                    continue;
                }

                int midpoint = (episode_start + episode_end) / 2;
                std::string episode1_name = base_name + " S1E" + std::to_string(episode_start) + ".mp4";
                std::string episode2_name = base_name + " S1E" + std::to_string(midpoint + 1) + ".mp4";

                std::string command1 = "ffmpeg -i \"" + mp4_file + "\" -ss 0 -t " + std::to_string(midpoint - episode_start + 1) + " -c copy \"" + episode1_name + "\"";
                std::string command2 = "ffmpeg -i \"" + mp4_file + "\" -ss " + std::to_string(midpoint - episode_start + 1) + " -c copy \"" + episode2_name + "\"";
                std::system(command1.c_str());
                std::system(command2.c_str());

                std::cout << "Split " << mp4_file << " into " << episode1_name << " and " << episode2_name << std::endl;
                episodes_to_move.emplace_back(episode1_name, episode2_name, mp4_file);
            } catch (const std::exception& e) {
                std::cout << "Error processing " << mp4_file << ": " << e.what() << std::endl;
            }
        }
    }
    return episodes_to_move;
}

void convert() {
    std::vector<std::string> mkv_files = get_files_with_extension(".mkv");

    if (mkv_files.empty()) {
        std::cout << "No MKV files found. Skipping the conversion step." << std::endl;
        return;
    }

    for (const auto& mkv_file : mkv_files) {
        std::string mp4_file = mkv_file.substr(0, mkv_file.find_last_of('.')) + ".mp4";
        std::string command = "ffmpeg -i \"" + mkv_file + "\" -codec copy \"" + mp4_file + "\"";
        std::system(command.c_str());
        std::cout << "Converted " << mkv_file << " to " << mp4_file << "." << std::endl;

        fs::create_directory("original");
        fs::rename(mkv_file, "original/" + mkv_file);
        std::cout << "Moved original " << mkv_file << " to 'original' folder." << std::endl;
    }
}

void update_metadata(const std::string& json_file) {
    std::ifstream ifs(json_file);
    Json::Value data;
    ifs >> data;

    std::vector<std::string> mp4_files = get_files_with_extension(".mp4");

    if (mp4_files.empty()) {
        std::cout << "No MP4 files found." << std::endl;
        return;
    }

    for (const auto& mp4_file : mp4_files) {
        try {
            size_t pos = mp4_file.find(" S");
            std::string season_episode = mp4_file.substr(pos + 2, mp4_file.find('.') - pos - 2);
            size_t e_pos = season_episode.find('E');
            std::string season = season_episode.substr(0, e_pos);
            std::string episode = season_episode.substr(e_pos + 1);

            std::string season_folder = "Season " + std::string(2 - season.length(), '0') + season;

            std::string show_title = data["Show"]["Title"].asString();
            std::string season_title = data["Show"]["Seasons"]["Season " + season]["Title"].asString();
            std::string episode_title = data["Show"]["Seasons"]["Season " + season]["Episodes"]["E" + std::string(2 - episode.length(), '0') + episode]["Title"].asString();
            std::string episode_comment = data["Show"]["Seasons"]["Season " + season]["Episodes"]["E" + std::string(2 - episode.length(), '0') + episode]["Comments"].asString();

            std::string new_title = show_title + " | " + season_title + " S" + season + "E" + episode + " - " + episode_title;

            fs::create_directory(season_folder);

            std::string modified_mp4 = mp4_file + "_modified.mp4";
            std::string command = "ffmpeg -i \"" + mp4_file + "\" -metadata title=\"" + new_title + "\" -metadata comment=\"" + episode_comment + "\" -c copy \"" + modified_mp4 + "\"";
            std::system(command.c_str());

            fs::rename(modified_mp4, season_folder + "/" + mp4_file);
            fs::rename(season_folder + "/" + mp4_file, season_folder + "/" + mp4_file.substr(0, mp4_file.find("_modified")) + ".mp4");

            std::cout << "Processed " << mp4_file << ": Title='" << new_title << "', Comments='" << episode_comment << "'. Moved to " << season_folder << std::endl;

            fs::remove(mp4_file);
        } catch (const std::exception& e) {
            std::cout << "Error processing " << mp4_file << ": " << e.what() << std::endl;
        }
    }

    auto episodes_to_move = split_special_episodes();
    for (const auto& [episode1_name, episode2_name, original_file] : episodes_to_move) {
        fs::rename(episode1_name, "Season " + std::string(2 - episode1_name.length(), '0') + episode1_name);
        fs::rename(episode2_name, "Season " + std::string(2 - episode2_name.length(), '0') + episode2_name);
        std::cout << "Moved " << episode1_name << " and " << episode2_name << " to " << "Season " + std::string(2 - episode1_name.length(), '0') + episode1_name << std::endl;
    }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("media_processor");

    program.add_argument("-json")
        .default_value(std::string("template.json"))
        .help("Path to JSON file for metadata (default: template.json)");

    program.add_argument("-help")
        .default_value(false)
        .implicit_value(true)
        .help("Show this help message and exit");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    if (program.get<bool>("-help")) {
        std::cout << program;
        return 0;
    }

    std::string json_file = program.get<std::string>("-json");

    convert();
    split_special_episodes();
    update_metadata(json_file);

    std::cout << "Script execution completed." << std::endl;

    return 0;
}


 
