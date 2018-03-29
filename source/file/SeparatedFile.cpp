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

#include <string>
#include <memory>
#include <vector>

#include "SeparatedFile.hpp"
#include "../flags/CommandLineFlags.hpp"
#include "../io/streams/StringInputStream.hpp"
#include "../io/streams/StringOutputStream.hpp"
#include "../utils/Tokenizer.hpp"
#include "../utils/StringUtils.hpp"

SeparatedFile::SeparatedFile(const std::string& path,
                             const std::unique_ptr<CommandLineFlags>& flags,
                             OpenMode mode,
                             char separator)
: IOFile(path,
         (separator == ',' ? FileType::Csv : FileType::Tsv),
         mode),
  separator_(separator)
{
    header_done_ = false;
    tokenizer_ = std::make_unique<Tokenizer>(separator);
    Prepare_(mode);
}

void SeparatedFile::Prepare_(OpenMode mode)
{
    if (header_done_)
        return;

    header_done_ = true;
    column_types = std::make_unique<ColumnTypesConfigurationCpp>();

    switch (mode) {
        case OpenMode::Read:
        {
            auto extension = utils::GetExtension(filePath());
            column_types->readConfigurationOfColumnTypesForFile(filePath());
            bool columned_extension = (extension == "csvc" ||
                                       extension == "tsvc" ||
                                       extension == "csvcr" ||
                                       extension == "tsvcr");
            if (!column_types->Columns().empty() || columned_extension)
                ReadHeader_();
            break;
        }
        case OpenMode::Write:
            // Add and write columns info
            column_types->saveConfigurationOfColumnTypesForFile(filePath());
            break;
    }
}

void SeparatedFile::ReadHeader_()
{
    std::string header_line = in_file_->ReadLine();

    tokenizer_->SetText(header_line);
    while (tokenizer_->ReadNext())
        header_.push_back(tokenizer_->GetNextToken());
}

void SeparatedFile::SetUpWithReadMode(bool read)
{
    if (header_done_)
        return;

    header_done_ = true;
    column_types = std::make_unique<ColumnTypesConfigurationCpp>();
    if (read)
        column_types->readConfigurationOfColumnTypesForFile(filePath());
}

void SeparatedFile::AddColumn(int columnId,
                              enum ColumnType type,
                              std::string description)
{
    column_types->AddColumn(columnId, type, description);
}

void SeparatedFile::SaveConfigurationOfColumnTypesForFile(std::string inputFileName)
{
    column_types->saveConfigurationOfColumnTypesForFile(inputFileName);
}

void SeparatedFile::AddColumnToHeader(std::string columnName)
{
    header_.push_back(columnName);
}

void SeparatedFile::SetHeader(const std::vector<std::string>& header)
{
    header_ = header;
}

void SeparatedFile::WriteHeaderIntoFile()
{
    out_file_->Write(header_[0]);
    for (int i = 1; i < header_.size(); i++) {
        out_file_->Write(separator_);
        out_file_->Write(header_[i]);
    }
    out_file_->WriteLine();
}

void SeparatedFile::WriteRow(const std::vector<std::string>& row)
{
    out_file_->Write(row[0]);
    for (int i = 1; i < row.size(); i++) {
        out_file_->Write(separator_);
        out_file_->Write(row[i]);
    }
    out_file_->WriteLine();
}

void SeparatedFile::Write(const std::string& str)
{
    out_file_->Write(str);
}

void SeparatedFile::Write(char ch)
{
    out_file_->Write(ch);
}

void SeparatedFile::WriteLine(const std::string& str)
{
    out_file_->WriteLine(str);
}

void SeparatedFile::WriteLine()
{
    out_file_->WriteLine();
}

void SeparatedFile::WriteQuoted(const std::string& str)
{
    out_file_->WriteQuoted(str);
}

char SeparatedFile::separator() const
{
    return separator_;
}

std::unique_ptr<Tokenizer>& SeparatedFile::tokenizer()
{
    return tokenizer_;
}

const std::vector<std::string>& SeparatedFile::header() const
{
    return header_;
}

bool SeparatedFile::HeaderPresent() const
{
    return !header_.empty();
}

std::string SeparatedFile::ReadLine()
{
    return in_file_->ReadLine();
}

std::vector<std::string> SeparatedFile::ReadNextRow()
{
    std::string last_read_line = in_file_->ReadLine();
    std::vector<std::string> next_row;

    if (last_read_line.empty())
        return next_row;

    tokenizer_->SetText(last_read_line);

    while (tokenizer_->ReadNext())
        next_row.push_back(tokenizer_->GetNextToken());

    return next_row;
}
