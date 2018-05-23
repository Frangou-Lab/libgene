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

#include "GenomicSeparatedFile.hpp"

#include <string>
#include <memory>
#include <cmath>

#include "../../utils/Tokenizer.hpp"
#include "../../utils/CppUtils.hpp"
#include "../../utils/StringUtils.hpp"
#include "../../def/Flags.hpp"
#include "../../log/Logger.hpp"

namespace gene {

GenomicSeparatedFile::GenomicSeparatedFile(const std::string& path,
                                           const std::unique_ptr<CommandLineFlags>& flags,
                                           FileType type,
                                           OpenMode mode,
                                           char separator)
 : SequenceFile(path, flags, type)
{
    file_ = std::make_unique<SeparatedFile>(path, flags, mode, separator);
    if (mode == OpenMode::Write) {
        if (flags->SettingExists(Flags::kReorderOutputColumns))
            columns_reordering_ = utils::UppercaseString(*flags->GetSetting(Flags::kReorderOutputColumns));
        else if (flags->SettingExists(Flags::kReorderInputColumns))
            columns_reordering_ = utils::UppercaseString(*flags->GetSetting(Flags::kReorderInputColumns));
    }
    skip_quality_column_ = flags->SettingExists(Flags::kOmitQuality);
    no_column_defs_ = flags->SettingExists("nocolumndefs");
    Prepare_(mode);
}

void GenomicSeparatedFile::Prepare_(OpenMode mode)
{
    file_->SetUpWithReadMode(mode == OpenMode::Read);

    switch (mode) {
        case OpenMode::Read:
            ParseColumnsInfo_();
            break;
        case OpenMode::Write:
        {
            // Form header
            if (!columns_reordering_.empty()) {
                if (verbose_)
                    PrintfLog("Parsing columns order of output file from flag...\n");

                name_col_ = desc_col_ = seq_col_ = quality_col_ = -1;

                int64_t ui = columns_reordering_.find('N');
                if (ui != std::string::npos)
                    name_col_ = static_cast<int>(ui);

                ui = columns_reordering_.find('D');
                if (ui != std::string::npos)
                    desc_col_ = static_cast<int>(ui);

                ui = columns_reordering_.find('S');
                if (ui != std::string::npos)
                    seq_col_ = static_cast<int>(ui);

                ui = columns_reordering_.find('Q');
                if (ui != std::string::npos)
                    quality_col_ = static_cast<int>(ui);
            } else {
                name_col_ = 0;
                desc_col_ = 1;
                seq_col_ = 2;
                quality_col_ = 3;
            }

            if (name_col_ != std::string::npos)
                file_->AddColumn(name_col_, ID, "Name");
            if (desc_col_ != std::string::npos)
                file_->AddColumn(desc_col_, Desc, "Description");
            if (seq_col_ != std::string::npos)
                file_->AddColumn(seq_col_, Data, "Sequence");
            if (quality_col_ != std::string::npos)
                file_->AddColumn(quality_col_, Data, "Quality");

            int cNum = std::max({name_col_, desc_col_, seq_col_, quality_col_}) + 1;
            if (cNum <= 0)
                return;

            for (int i = 0; i < cNum; i++) {
                if (i == name_col_)
                    file_->AddColumnToHeader("Name");
                else if (i == desc_col_)
                    file_->AddColumnToHeader("Description");
                else if (i == seq_col_)
                    file_->AddColumnToHeader("Sequence");
                else if (i == quality_col_)
                    file_->AddColumnToHeader("Quality");
                else
                    file_->AddColumnToHeader("Unknown");
            }

            if (!file_->HeaderPresent())
                return;

            file_->SaveConfigurationOfColumnTypesForFile(filePath());
            file_->WriteHeaderIntoFile();
            break;
        }
    }
}

std::vector<std::string> GenomicSeparatedFile::getHeader() const
{
    return file_->header();
}

void GenomicSeparatedFile::ParseColumnsInfo_()
{
    name_col_ = 0;
    desc_col_ = 1;
    seq_col_ = 2;
    quality_col_ = 3;
    
    // Now columns
    if (no_column_defs_) {
        if (verbose_)
            PrintfLog("Using defalut columns order NDSQ\n");
    } else if (!columns_reordering_.empty()) {

        if (verbose_)
            PrintfLog("Parsing columns order of input file from flag...\n");
        
        name_col_ = desc_col_ = seq_col_ = quality_col_ = -1;
        
        int64_t ui = columns_reordering_.find('N');
        if (ui != std::string::npos)
            name_col_ = static_cast<int>(ui);
        
        ui = columns_reordering_.find('D');
        if (ui != std::string::npos)
            desc_col_ = static_cast<int>(ui);
        
        ui = columns_reordering_.find('S');
        if (ui != std::string::npos)
            seq_col_ = static_cast<int>(ui);
        
        ui = columns_reordering_.find('Q');
        if (ui != std::string::npos)
            quality_col_ = static_cast<int>(ui);
    } else if (!file_->column_types->Columns().empty()) {
        // From column types
        if (verbose_)
            PrintfLog("Parsing columns order from columns definition...\n");
        
        name_col_ = desc_col_ = seq_col_ = quality_col_ = -1;
        if (file_->column_types->IDColumn())
            name_col_ = file_->column_types->IDColumn()->column_id;
        
        auto descs = file_->column_types->ColumnsWithType(Desc);
        if (descs.size() > 0)
            desc_col_ = descs[0]->column_id;
        
        auto data = file_->column_types->ColumnsWithType(Data);
        if (data.size() > 0)
            seq_col_ = data[0]->column_id;
        
        if (data.size() > 1)
            quality_col_ = data[1]->column_id;
        
        if (((name_col_ == -1) &&
             (desc_col_ == -1) &&
             (seq_col_ == -1) &&
             (quality_col_ == -1))
            || (std::fabs(name_col_ + desc_col_ + seq_col_ + quality_col_) > 10 /* Max sensible sum that columns indexes can be: 0 + 1 + 2 + 3 + 4*/))
        {
            // An attempt to fix corrupted .ctp file which happens to contain
            // broken column indexes
            name_col_ = 0;
            desc_col_ = 1;
            seq_col_ = 2;
            quality_col_ = 3;
        }
    }
    
    if (skip_quality_column_) {
        quality_col_ = -1;
        if (verbose_)
            PrintfLog("Skipping quality column\n");
    }
    
    if (verbose_) {
        if (name_col_ >= 0)
            PrintfLog("Name column: %lld\n", name_col_);
        if (desc_col_ >= 0)
            PrintfLog("Description column: %lld\n", desc_col_);
        if (seq_col_ >= 0)
            PrintfLog("Sequence column: %lld\n", seq_col_);
        if (quality_col_ >= 0)
            PrintfLog("Quality column: %lld\n", quality_col_);
    }
}

bool GenomicSeparatedFile::isValidGeneFile() const
{
    return name_col_ != std::string::npos && seq_col_ != std::string::npos;
}

SequenceRecord GenomicSeparatedFile::Read()
{
    std::string str = file_->ReadLine();
    SequenceRecord record;

    if (str.empty())
       return record;

    const auto& tokenizer = file_->tokenizer();
    tokenizer->SetText(str);

    const bool default_order = (name_col_ == 0 &&
                                desc_col_ == 1 &&
                                seq_col_ == 2 &&
                                quality_col_ == 3);

    if (default_order) {
        tokenizer->ReadNext();
        record.name = tokenizer->GetNextToken();
        
        tokenizer->ReadNext();
        record.desc = tokenizer->GetNextToken();
        
        tokenizer->ReadNext();
        record.seq = tokenizer->GetNextToken();
        
        tokenizer->ReadNext();
        record.quality = tokenizer->GetNextToken();
    } else {
        int i = 0;
        while (tokenizer->ReadNext()) {
            if (i == name_col_)
                record.name = tokenizer->GetNextToken();
            else if (i == desc_col_)
                record.desc = tokenizer->GetNextToken();
            else if (i == seq_col_)
                record.seq = tokenizer->GetNextToken();
            else if (i == quality_col_)
                record.quality = tokenizer->GetNextToken();
            ++i;
        }
    }
    return record;
}

std::vector<std::string> GenomicSeparatedFile::ReadVec()
{
    std::vector<std::string> components;

    auto line = file_->ReadLine();
    const auto& tokenizer = file_->tokenizer();
    tokenizer->SetText(line);

    while (tokenizer->ReadNext())
        components.emplace_back(tokenizer->GetNextToken());

    return components;
}

void GenomicSeparatedFile::Write(const SequenceRecord& record)
{
    const bool default_order = (name_col_ == 0) &&
                               (desc_col_ == 1) &&
                               (seq_col_ == 2) &&
                               (quality_col_ == 3);
    if (default_order) {
        char separator = file_->separator();
        file_->WriteQuoted(record.name);
        file_->Write(separator);
        file_->WriteQuoted(record.desc);
        file_->Write(separator);
        file_->WriteQuoted(record.seq);
        file_->Write(separator);
        auto qual = utils::CommaEscapedString(record.quality);
        file_->WriteQuoted(qual);
    } else {
        for (int i = 0; i < file_->header().size(); i++) {
            if (i != 0)
                file_->Write(file_->separator());
            if (i == name_col_)
                file_->WriteQuoted(record.name);
            else if (i == desc_col_)
                file_->WriteQuoted(record.desc);
            else if (i == seq_col_)
                file_->WriteQuoted(record.seq);
            else if (i == quality_col_)
                file_->WriteQuoted(record.quality);
        }
    }
    file_->WriteLine();
}

int64_t GenomicSeparatedFile::position() const
{
    return file_->position();
}

int64_t GenomicSeparatedFile::length() const
{
    return file_->length();
}

std::string GenomicSeparatedFile::strFileType() const
{
    return utils::type2str(file_->fileType());
}

std::string GenomicSeparatedFile::filePath() const
{
    return file_->filePath();
}

std::string GenomicSeparatedFile::fileName() const
{
    return file_->fileName();
}

}  // namespace gene
