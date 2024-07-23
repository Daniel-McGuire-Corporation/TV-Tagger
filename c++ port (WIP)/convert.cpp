// convert.cpp (New source file for the DLL)
#include <iostream>
#include <filesystem>
#include <vector> // Include the <vector> header
#include <string>

namespace fs = std::filesystem;

// Define the function to be exported from the DLL
__declspec(dllexport) void convert() {
    std::vector<std::string> mkv_files = get_files_with_extension(".mkv"); // Define or include get_files_with_extension function

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

// Implement get_files_with_extension function here
std::vector<std::string> get_files_with_extension(const std::string& extension) {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}