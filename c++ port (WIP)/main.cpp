// main.cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "thirdparty/json.hpp"
#include <cstdlib>
#include "thirdparty/argparse.hpp"
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

void update_metadata(const std::string& json_file) {
    std::ifstream ifs(json_file);
    nlohmann::json data;
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

            std::string show_title = data["Show"]["Title"].get<std::string>();
            std::string season_title = data["Show"]["Seasons"]["Season " + season]["Title"].get<std::string>();
            std::string episode_title = data["Show"]["Seasons"]["Season " + season]["Episodes"]["E" + std::string(2 - episode.length(), '0') + episode]["Title"].get<std::string>();
            std::string episode_comment = data["Show"]["Seasons"]["Season " + season]["Episodes"]["E" + std::string(2 - episode.length(), '0') + episode]["Comments"].get<std::string>();

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

    HMODULE convertDLL = LoadLibrary(L"convert.dll");
    HMODULE splitDLL = LoadLibrary(L"split.dll");

    if (convertDLL == NULL || splitDLL == NULL) {
        std::cerr << "Failed to load DLLs." << std::endl;
        return 1;
    }

    typedef void (*ConvertFunction)();
    typedef std::vector<std::tuple<std::string, std::string, std::string>> (*SplitFunction)();

    ConvertFunction convert = (ConvertFunction)GetProcAddress(convertDLL, "convert");
    SplitFunction split = (SplitFunction)GetProcAddress(splitDLL, "split");

    if (convert == NULL || split == NULL) {
        std::cerr << "Failed to load functions from DLLs." << std::endl;
        return 1;
    }

    convert();
    auto episodes_to_move = split();

    update_metadata(json_file);

    std::cout << "Script execution completed." << std::endl;

    return 0;
}