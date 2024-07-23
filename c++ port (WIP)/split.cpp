// split.cpp 
#include <iostream>
#include <filesystem>
#include <vector>
#include <tuple>
#include <string>

namespace fs = std::filesystem;

// Placeholder for get_files_with_extension function
std::vector<std::string> get_files_with_extension(const std::string& extension) {
    // Implement logic to retrieve files with a specific extension
    // This function should return a vector of file paths with the given extension
}

// Export the split function for DLL use
extern "C" __declspec(dllexport) std::vector<std::tuple<std::string, std::string, std::string>> split() {
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