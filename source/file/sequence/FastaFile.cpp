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
#include <algorithm>

#include "FastaFile.hpp"
#include "../../log/Logger.hpp"

namespace gene {

FastaFile::FastaFile(const std::string& path,
                     const std::unique_ptr<CommandLineFlags>& flags,
                     OpenMode mode)
: SequenceFile(path, flags, FileType::Fasta, mode)
{
    split_ = flags->SettingExists("splitfasta");
    
    if (flags->verbose && split_)
        PrintfLog("Limiting sequence length to 80 characters\n");
}

std::vector<std::string> FastaFile::extensions()
{
    return {"fas", "fasta", "fna", "ffn", "faa", "frn"};
}

std::string FastaFile::defaultExtension()
{
    return "fasta";
}

std::string FastaFile::displayExtension()
{
    return defaultExtension();
}

SequenceRecord FastaFile::Read()
{
    std::string line;
    SequenceRecord result_rec;

    while (!(line = in_file_->ReadLine()).empty()) {
        if (line[0] == '>')
            break;
    }

    if (line.empty())
        return result_rec;
    
    int64_t space = line.find(' ');
    if (space == std::string::npos) {
        // No description
        result_rec.name = line.substr(1);
    } else {
        result_rec.name = line.substr(1, space - 1);
        result_rec.desc = line.substr(space + 1);
    }

    int next_char;

    while ((next_char = in_file_->Peek()) != '>' && next_char != EOF) {
        // Read until the next charater is '>' (Sequence data in single record
        // can span several lines).
        result_rec.seq += in_file_->ReadLine();
    }
    return result_rec;
}

std::vector<std::string> FastaFile::ReadVec()
{
    std::vector<std::string> components;
    std::string line;

    while (!(line = in_file_->ReadLine()).empty()) {
        if (line[0] == '>')
            break;
    }
    if (line.empty())
        return components;

    int64_t space = line.find(' ');

    if (space == std::string::npos) {
        components.push_back(line.substr(1));
        components.push_back("");  // No description
    } else {
        components.push_back(line.substr(1, space - 1));
        components.push_back(line.substr(space + 1));
    }
    line.clear();
    int next_char;

    while ((next_char = in_file_->Peek()) != '>' && next_char != EOF) {
        // Read until the next charater is '>' (Sequence data in single record
        // can span several lines).
        line += in_file_->ReadLine();
    }
    components.push_back(std::move(line));
    return components;
}

void FastaFile::Write(const SequenceRecord& record)
{
    out_file_->Write('>');
    if (record.desc.empty())
        out_file_->WriteLine(record.name);
    else {
        out_file_->Write(record.name);
        out_file_->Write(' ');
        out_file_->WriteLine(record.desc);
    }
    
    if (split_) {
        // Data, split by 80 chars max
        for (int i = 0; i < record.seq.size(); i += 80) {
            out_file_->WriteLine(record.seq.substr(i, std::min(static_cast<int64_t>(record.seq.size()) - i, 80LL)));
        }
    } else
        out_file_->WriteLine(record.seq);
}

bool FastaFile::isValidGeneFile() const
{
    return true;
}

}  // namespace gene
