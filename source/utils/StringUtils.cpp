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

#include <cassert>
#include <algorithm>
#include <locale>
#include <cctype>

#include "StringUtils.hpp"

namespace gene::utils {

std::string InsertSuffixBeforePathExtension(std::string path, std::string suffix)
{
    return StringByDeletingPathExtension(path) + suffix + '.' + GetExtension(path);
}

bool HasExtension(const std::string& path, const std::string ext)
{
    const int64_t size = path.size();
    const int64_t extSize = ext.size();
    std::string extension = path.substr(std::max(size - extSize - 1, 0ll), extSize + 1);
    std::for_each(extension.begin(), extension.end(), [](char& c){
        return std::tolower(c, std::locale());
    });
    return (size >= extSize + 1 && extension == "." + ext);
}

std::string GetExtension(const std::string& str)
{
    auto dot = str.rfind('.');

    if (str.empty() || dot == std::string::npos || dot + 1 == str.size())
        return "";
    return str.substr(dot + 1);
}

std::string GetExtensionCompressed(const std::string& path)
{
    auto extension = GetExtension(path);
    if (extension == "gz")
        extension = GetExtension(StringByDeletingPathExtension(path));
    return extension;
}

std::string GetLastPathComponent(const std::string& str)
{
    char path_delimiter = '/';
    if constexpr (kCompiledForWindows) {
        path_delimiter = '\\';
    }
    auto slash_position = str.rfind(path_delimiter);
    if (str.empty() || slash_position == std::string::npos)
        return str;

    return str.substr(slash_position + 1);
}

void ReplaceOccurrencesOfString(std::string& targetStr, std::string what, std::string withWhat)
{
    std::string::size_type start = 0;
    while ((start = targetStr.find(what, start)) != std::string::npos) {
        auto occurence = targetStr.begin() + start;
        targetStr.replace(occurence, occurence + what.size(), withWhat);
        start += withWhat.size();
    }
}

bool StringContainsCharacter(const std::string& src, char ch)
{
    return src.find(ch) != std::string::npos;
}

std::string UppercaseString(const std::string& str)
{
    std::string uppercase = str;
    std::for_each(uppercase.begin(), uppercase.end(), [](char& c){
        c = std::toupper(c);
    });
    return uppercase;
}

std::string LowercaseString(const std::string& str)
{
    std::string lowercase = str;
    std::for_each(lowercase.begin(), lowercase.end(), [](char& c){
        return (char)std::tolower(c);
    });
    return lowercase;
}

std::string StringByDeletingPathExtension(const std::string& str)
{
    auto dotPosition = str.rfind('.');
    if (dotPosition == std::string::npos)
        return str;

    return str.substr(0, dotPosition);
}

std::string StringByDeletingPathExtensionCompressed(const std::string& path)
{
    auto extension = GetExtension(path);
    auto result = StringByDeletingPathExtension(path);

    if (extension == "gz")
        result = StringByDeletingPathExtension(result);

    return result;
}

std::string StringByDeletingLastPathComponent(const std::string& str)
{
    char path_delimiter = '/';
    if constexpr (kCompiledForWindows) {
        path_delimiter = '\\';
    }
    auto slash = str.rfind(path_delimiter);
    if (slash == std::string::npos)
        return str;

    return str.substr(0, slash);
}

std::string CommaEscapedString(const std::string& stringToEscape)
{
    std::string result = stringToEscape;

    if (result.find('"') == std::string::npos)
        return result;

    auto iter = result.begin();
    while (iter != result.end()) {
        if (*iter == '"') {
            iter = result.insert(iter, '\"');
            ++iter;
        }
        ++iter;
    }
    return result;
}

std::string PaddedToLengthString(int number, int length)
{
    assert(length < 10);
    char buffer[10];
    snprintf(buffer, length + 1, ("%0" + std::to_string(length) + "d").c_str(), number);
    return std::string(buffer);
}

}  // namespace gene::utils
