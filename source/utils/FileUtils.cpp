/*
 * Copyright 2018 Frangou Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <dirent.h>

#include "FileUtils.hpp"

#define BEGIN_NAMESPACE_LIBGENE_UTILS_ namespace utils {
#define END_NAMESPACE_LIBGENE_UTILS_ }

BEGIN_NAMESPACE_LIBGENE_UTILS_

bool IsDirectory(const std::string& path)
{
    if (DIR *dir = opendir(path.c_str()); dir != nullptr) {
        closedir(dir);
        return true;
    }
    return false;
}

std::vector<std::string> GetDirectoryContents(const std::string& path)
{
    std::vector<std::string> contents;

    if (DIR *dir = opendir(path.c_str()); dir != nullptr) {
        /* print all the files and directories within directory */
        struct dirent *dir_entry;

        while ((dir_entry = readdir(dir)) != nullptr) {
            std::string name = dir_entry->d_name;
            if (name.front() != '.' /* hidden file */ &&
                name != "." &&
                name != ".." &&
                name != ".DS_Store") {
                contents.push_back(path + '/' + name);
            }
        }
        closedir(dir);
    } else
        throw std::runtime_error("Could not open directory");

    return contents;
}

int64_t GetFileSize(const std::string& path)
{
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    if (!in)
        throw std::runtime_error("Unable to open the file");

    return in.tellg();
}

bool CheckFstreamsEqual(std::ifstream& f1, std::ifstream& f2)
{
    std::string line, line_ref;
    bool check_next = f1 && f2;

    while (check_next) {
        check_next &= static_cast<bool>(std::getline(f1, line));
        check_next &= static_cast<bool>(std::getline(f2, line_ref));

        if (line != line_ref)
            return false;
    }
    return true;
}

bool CheckFstreamsEqualUnordered(std::ifstream& f1, std::ifstream& f2)
{
    std::string line, line_ref;
    std::unordered_map<std::string, int> line_count;

    while (std::getline(f1, line))
        line_count[line]++;

    while (std::getline(f2, line_ref))
        line_count[line_ref]--;

    for (auto it = line_count.cbegin(); it != line_count.cend(); ++it) {
        if (it->second != 0)
            return false;
    }
    return true;
}

END_NAMESPACE_LIBGENE_UTILS_
