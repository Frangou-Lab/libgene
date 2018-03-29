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

#include "BedFile.hpp"
#include "../sam/SamRecord.hpp"

BedFile::BedFile(const std::string& path,
                 const std::unique_ptr<CommandLineFlags>& flags,
                 OpenMode mode)
: AlignmentFile(path, FileType::Bed, flags, mode)
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

void BedFile::readHeader()
{

}

void BedFile::writeHeader()
{
    out_file_->Write("chrom");
    out_file_->Write(',');
    out_file_->Write("chromStart");
    out_file_->Write(',');
    out_file_->Write("chromEnd");
    out_file_->Write(',');
    out_file_->Write("name");
    out_file_->WriteLine();
}

void BedFile::write(const SamRecord& record)
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

SamRecord BedFile::read()
{
    std::string line = in_file_->ReadLine();
    return SamRecord(line);
}

int64_t BedFile::length() const
{
    return IOFile::length();
}

int64_t BedFile::position() const
{
    return IOFile::position();
}

bool BedFile::isValidAlignmentFile() const
{
    return true;
}

std::string BedFile::strFileType() const
{
    return "bed";
}

std::string BedFile::defaultExtension()
{
    return "bed";
}

std::vector<std::string> BedFile::extensions()
{
    return {"bed"};
}
