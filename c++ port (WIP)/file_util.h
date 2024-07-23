#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <vector>
#include <string>
#include <filesystem>

std::vector<std::string> get_files_with_extension(const std::string& extension);

#endif // FILE_UTIL_H
