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

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "SamFile.hpp"
#include "SamRecord.hpp"
#include "SamHeader.hpp"

using std::string;

namespace gene {

SamFile::SamFile(const std::string& path,
                 const std::unique_ptr<CommandLineFlags>& flags,
                 OpenMode mode)
: AlignmentFile(path, FileType::Sam, flags, mode)
{
    switch (mode) {
        case OpenMode::Read:
            readHeader();
            break;
        case OpenMode::Write:
            writeHeader();
            break;
    }
}

void SamFile::writeHeader()
{
}

void SamFile::readHeader()
{
    char ch;

    std::vector<std::string> headerLines;
    while ((ch = in_file_->Peek()) == '@') {
        headerLines.push_back(in_file_->ReadLine());
    }

    if (!headerLines.empty())
        header = std::make_unique<SamHeader>(headerLines);
}

void SamFile::write(const SamRecord& record)
{
    out_file_->Write(record.chrom);
    out_file_->Write('\t');
    out_file_->Write(std::to_string(record.chromStart));
    out_file_->Write('\t');
    out_file_->Write(std::to_string(record.chromEnd));
    out_file_->Write('\t');
    out_file_->Write(record.name);
    out_file_->WriteLine();
}

SamRecord SamFile::read()
{
    return SamRecord(in_file_->ReadLine());
}

int64_t SamFile::length() const
{
    return IOFile::length();
}

int64_t SamFile::position() const
{
    return IOFile::position();
}

bool SamFile::isValidAlignmentFile() const
{
    return true;
}

string SamFile::strFileType() const
{
    return "sam";
}

string SamFile::defaultExtension()
{
    return "sam";
}

std::vector<std::string> SamFile::extensions()
{
    return {"sam"};
}

}  // namespace gene
