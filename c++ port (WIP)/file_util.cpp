#include "file_util.h"

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
