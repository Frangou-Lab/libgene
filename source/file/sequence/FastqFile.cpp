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

#include "FastqFile.hpp"

#include "../../flags/CommandLineFlags.hpp"
#include "../../io/streams/StringInputStream.hpp"
#include "../../io/streams/StringOutputStream.hpp"
#include "../../file/sequence/SequenceRecord.hpp"
#include "../../file/alignment/AlignmentRecord.hpp"
#include "../../file/alignment/sam/SamRecord.hpp"
#include "../../io/IOFile.hpp"
#include "../../def/Flags.hpp"
#include "../../log/Logger.hpp"

std::string FastqFile::defaultExtension()
{
    return "fastq";
}

std::string FastqFile::displayExtension()
{
    return defaultExtension();
}

std::vector<std::string> FastqFile::extensions()
{
    return {"fq", "fastq"};
}

FastqFile::FastqFile(const std::string& path,
                     const std::unique_ptr<CommandLineFlags>& flags,
                     OpenMode mode)
 : SequenceFile(path, flags, FileType::Fastq, mode)
{
    def_quality_ = flags->quality();
    override_existing_quality_ = flags->SettingExists(Flags::kOverrideExistingQuality);
    duplicate_ = flags->SettingExists("duplicatefastqids");
        
    if (flags->verbose && mode != OpenMode::Read && duplicate_)
        PrintfLog("Duplicating id & description\n");
}

SequenceRecord FastqFile::Read()
{
    std::string last_read_line;
    SequenceRecord record;

    while (!(last_read_line = in_file_->ReadLine()).empty()) {
        if (last_read_line[0] != '@')
            last_read_line.clear();
        else
            break;
    }
    if (last_read_line.empty())
        return record;
    
    int64_t space = last_read_line.find(' ');
    if (space == std::string::npos) {
        // No description
        record.name = last_read_line.substr(1);
    } else {
        record.name = last_read_line.substr(1, space - 1);
        record.desc = last_read_line.substr(space + 1);
    }
    record.seq = in_file_->ReadLine();

    // Skip until '+' string
    while (!(last_read_line = in_file_->ReadLine()).empty()) {
        if (last_read_line[0] != '+')
            last_read_line.clear();
        else
            break;
    }
    
    record.quality = in_file_->ReadLine();
    return record;
}

std::vector<std::string> FastqFile::ReadVec()
{
    std::string last_read_line;
    std::vector<std::string> components;

    if (last_read_line.empty()) {
        while (!(last_read_line = in_file_->ReadLine()).empty()) {
            if (last_read_line[0] != '@')
                last_read_line.clear();
            else
                break;
        }
    }
    if (last_read_line.empty())
        return components;

    int64_t space = last_read_line.find(' ');

    if (space == std::string::npos) {
        components.push_back(last_read_line.substr(1));
        components.push_back("");  // No description
    } else {
        components.push_back(last_read_line.substr(1, space - 1));
        components.push_back(last_read_line.substr(space + 1));
    }

    std::string data = in_file_->ReadLine();

    // Skip until '+' string
    while (! (last_read_line = in_file_->ReadLine()).empty()) {
        if (last_read_line[0] != '+')
            last_read_line.clear();
        else
            break;
    }
    components.push_back(std::move(data));

    std::string quality = in_file_->ReadLine();
    components.push_back(std::move(quality));
    return components;
}


void FastqFile::Write(const SequenceRecord& record)
{
    out_file_->Write('@');
    if (record.desc.empty())
        out_file_->WriteLine(record.name);
    else {
        out_file_->Write(record.name);
        out_file_->Write(' ');
        out_file_->WriteLine(record.desc);
    }
    out_file_->WriteLine(record.seq);
    
    out_file_->Write('+');
    if (duplicate_) {
        out_file_->Write(record.name);
        if (!record.desc.empty()) {
            out_file_->Write(' ');
            out_file_->Write(record.desc);
        }
    }
    out_file_->WriteLine();
    
    if (! (record.quality.empty() || override_existing_quality_))
        out_file_->WriteLine(record.quality);
    else {
        // Write long string from default quality
        std::string default_quality(record.seq.size(), def_quality_);
        out_file_->WriteLine(default_quality);
    } 
}

bool FastqFile::isValidGeneFile() const
{
    return true;
}
