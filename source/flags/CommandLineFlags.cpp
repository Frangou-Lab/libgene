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

#include <vector>

#include "CommandLineFlags.hpp"
#include "../utils/CppUtils.hpp"
#include "../def/Flags.hpp"

static std::vector<std::string> flagsWithParameters = {
    Flags::kInputFormat,
    Flags::kOutputFormat,
    Flags::kDefaultQuality,
    "f",
    "r",
    "sk",
    "sm",
    Flags::kReorderOutputColumns,
    Flags::kReorderInputColumns,
    Flags::kContextEnabled,
    Flags::kMismatchesEnabled,
    Flags::kMaxAmpliconSize,
    Flags::kMinAmpliconSize,
    Flags::kSolexaFastqCutoffLength};

CommandLineFlags::CommandLineFlags(const char **argv, int argc, int *pStart)
: CommandLineFlags()
{
    while (*pStart < argc) {
        if (argv[*pStart][0] != '-')
            break;

        std::string flag(argv[*pStart] + 1);
        
        ++*pStart;
        if (flag.empty())
            continue;
        
        // Check if have parameters
        bool isParameterizedFlag = utils::Contains(flagsWithParameters, flag);
        std::string param;
        if (isParameterizedFlag && *pStart < argc) {
            param = argv[*pStart];
            ++*pStart;
        }
        dict_[flag] = param;
        verbose = SettingExists(Flags::kVerbose);
    }
}

enum FileType CommandLineFlags::inputFormat() const
{
    if (dict_.find(Flags::kInputFormat) != dict_.end())
        return utils::str2type(dict_.at(Flags::kInputFormat));
    else
        return FileType::Unknown;
}

enum FileType CommandLineFlags::outputFormat() const
{
    if (dict_.find(Flags::kOutputFormat) != dict_.end())
        return utils::str2type(dict_.at(Flags::kOutputFormat));
    else
        return FileType::Unknown;
}

char CommandLineFlags::quality() const
{
    auto quality_string = GetSetting(Flags::kFastqQuality);
    if (quality_string == nullptr)
        return 'I';

    return quality_string->front();
}

bool CommandLineFlags::SettingExists(std::string name) const
{
    return dict_.find(name) != dict_.end();
}

int CommandLineFlags::GetIntSetting(std::string name) const
{
    auto val = GetSetting(name);
    if (val == nullptr)
        return 0;
    
    int valInt;
    try {
        valInt = std::stoi(*val);
    } catch (...) {
        fprintf(stderr, "Caught exception: std::stoi invalid argument exception. Argument: \"%s\"",
                val.get()->c_str());
        return 0;
    }
    return valInt;
}

std::unique_ptr<std::string> CommandLineFlags::GetSetting(std::string name) const
{
    if (!SettingExists(name))
        return nullptr;

    return std::make_unique<std::string>(dict_.at(name));
}

void CommandLineFlags::SetSetting(std::string key, std::string value)
{
    dict_[key] = value;
}

void CommandLineFlags::SetSetting(std::string key, int value)
{
    SetSetting(key, std::to_string(value));
}
