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
#include <string>
#include <cmath>
#include <utility>
#include <memory>
#include <algorithm>

#include "TxtFile.hpp"
#include "../log/Logger.hpp"

namespace gene {

TxtFile::TxtFile(const std::string& path,
                 const std::unique_ptr<CommandLineFlags>& flags,
                 OpenMode mode)
: SequenceFile(path, flags, FileType::PlainTxt, mode)
{
}

std::vector<std::string> TxtFile::extensions()
{
    return {"txt"};
}

std::string TxtFile::defaultExtension()
{
    return "txt";
}

std::string TxtFile::displayExtension()
{
    return defaultExtension();
}

SequenceRecord TxtFile::Read()
{
    if ((last_read_string_ = in_file_->ReadLine()).empty())
        return SequenceRecord();

    std::string seq = last_read_string_;
    return SequenceRecord("", "", std::move(seq));
}

std::vector<std::string> TxtFile::ReadVec()
{
    std::vector<std::string> components;
    last_read_string_ = in_file_->ReadLine();
    components.push_back(last_read_string_);
    return components;
}

void TxtFile::Write(const SequenceRecord& record)
{
    out_file_->WriteLine(record.seq);
}

bool TxtFile::isValidGeneFile() const
{
    return false;
}

}  // namespace gene
