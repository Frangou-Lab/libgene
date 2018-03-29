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

#ifndef CommandLineFlags_hpp
#define CommandLineFlags_hpp

#include <map>
#include <string>
#include <memory>

#include "../io/IOFile.hpp"

class CommandLineFlags {
 protected:
    std::map<std::string, std::string> dict_;
    char qual_;
    
 public:
    CommandLineFlags() : verbose(true) {};
    CommandLineFlags(const CommandLineFlags &) = default;
    CommandLineFlags(const char **argv, int argc, int *pStart);

    static std::unique_ptr<CommandLineFlags> flagsWithArguments(const char **argv, int argc, int *pStart);
    enum FileType inputFormat() const;
    enum FileType outputFormat() const;
    char quality() const;

    bool SettingExists(std::string name) const;
    int GetIntSetting(std::string name) const; // Returns 0 if no setting exists or it is invalid

    // Return nullptr if setting doesn't exist. In the future is should be
    // replaced with std::optional.
    std::unique_ptr<std::string> GetSetting(std::string name) const;

    void SetSetting(std::string key, std::string value = "");
    void SetSetting(std::string key, int value);

    bool verbose;
};

#endif /* CommandLineFlags_hpp */
