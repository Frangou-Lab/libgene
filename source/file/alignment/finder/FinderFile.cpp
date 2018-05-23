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

#include "FinderFile.hpp"
#include "FinderRecord.hpp"

#include "../../sequence/SequenceFile.hpp"
#include "../../../utils/StringUtils.hpp"
#include "../../../utils/CppUtils.hpp"
#include "../../../utils/Tokenizer.hpp"
#include "../../../def/Flags.hpp"
#include "../../../log/Logger.hpp"

namespace gene {

FinderFile::FinderFile(std::string path,
                       const std::unique_ptr<CommandLineFlags>& flags,
                       OpenMode mode,
                       char separator)
: SeparatedFile(path, flags, mode, separator)
{
    header_done_ = false;
    context_enabled_ = flags->SettingExists(Flags::kContextEnabled);
    paired_queries_search_mode_ = flags->SettingExists(Flags::kPairedQueryExtraction);
    coupled_queries_search_mode_ = flags->SettingExists(Flags::kCoupledQueries);
    mixed_strain_queries_search_mode_ = flags->SettingExists(Flags::kMixedStrainPairedPrimerSearch);
    has_annotation_ = flags->SettingExists(Flags::kGenBankInputFormat);
    Prepare_(mode);
}

FinderFile FinderFile::FileWithName(std::string name,
                                    const std::unique_ptr<CommandLineFlags>& flags,
                                    OpenMode mode)
{
    FileType type = (mode == OpenMode::Read) ?
                    flags->inputFormat() : flags->outputFormat();
    if (type == FileType::Unknown) // Determine from name
        type = utils::extension2type(utils::GetExtension(name));
    
    if (type == FileType::Fasta || type == FileType::Fastq) {
        PrintfLog("Output file format should be either tsv or csv");
    }
    if (type == FileType::Unknown) {
        std::string extension = utils::GetExtension(name);

        if (extension != "DS_Store") {
            PrintfLog("Unknown extension %s\n",
                       utils::GetExtension(name).c_str());
        }
    }
    return {name, flags, mode, (type == FileType::Tsv ? '\t' : ',')};
}

void FinderFile::Write(FinderRecord& record)
{
    std::string id = utils::CommaEscapedString(record.id);
    out_file_->WriteQuoted(id);

    if (!paired_queries_search_mode_) {
        out_file_->Write(separator_);

        if (record.target_sequence.size() < 10000)
            out_file_->WriteQuoted(record.target_sequence);
        else
            out_file_->WriteQuoted("<longer than 10^4 bp>");
    }

    if (has_annotation_) {
        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.annotation);
    }

    if (paired_queries_search_mode_ ||
        mixed_strain_queries_search_mode_ ||
        coupled_queries_search_mode_) {

        out_file_->Write(separator_);
        // Start of the left query
        out_file_->WriteQuoted(std::to_string(record.start - record.left_match_length));
    }

    out_file_->Write(separator_);
    out_file_->WriteQuoted(std::to_string(record.start));

    out_file_->Write(separator_);
    out_file_->WriteQuoted(std::to_string(record.end));

    if (paired_queries_search_mode_ || paired_queries_search_mode_ || coupled_queries_search_mode_) {
        out_file_->Write(separator_);
        // End of the right query
        out_file_->WriteQuoted(std::to_string(record.end + record.right_match_length));

        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.left_query_id);

        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.left_query);

        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.right_query_id);
        
        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.right_query);

        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.internal_seq);

        out_file_->Write(separator_);
        // Length of the internal sequence
        out_file_->WriteQuoted(std::to_string(record.internal_seq.size()));
    } else {
        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.query_id);

        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.query);
    }

    if (context_enabled_) {
        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.upstream_seq);
        out_file_->Write(separator_);
        out_file_->WriteQuoted(record.downstream_seq);
    }
    out_file_->WriteLine();
}

void FinderFile::Prepare_(OpenMode mode)
{
    if (header_done_)
        return;

    header_done_ = true;
    ColumnTypesConfigurationCpp column_types;

    switch (mode) {
        case OpenMode::Read:
        {
            column_types.readConfigurationOfColumnTypesForFile(filePath());
            if (!header_present_)
                return;

            std::string line = in_file_->ReadLine(); // need to own the returned variable since splitter receives a non-owning string_view
            tokenizer_->SetText(line);

            while (tokenizer_->ReadNext())
                header_.push_back(tokenizer_->GetNextToken());

            break;
        }
        case OpenMode::Write:
        {
            // Add and write columns info
            int index = 0;
            column_types.AddColumn(index++, ID, "ID");
            if (!paired_queries_search_mode_)
                column_types.AddColumn(index++, Desc, "Target sequence");

            if (has_annotation_)
                column_types.AddColumn(index++, Desc, "Annotation");

            if (paired_queries_search_mode_ ||
                mixed_strain_queries_search_mode_ ||
                coupled_queries_search_mode_) {

                column_types.AddColumn(index++, Data, "Left query start");
                column_types.AddColumn(index++, Data, "Amplicon start");
                column_types.AddColumn(index++, Data, "Amplicon end");
                column_types.AddColumn(index++, Data, "Right query end");
            } else {
                column_types.AddColumn(index++, Data, "Start position");
                column_types.AddColumn(index++, Data, "End position");
            }
            if (paired_queries_search_mode_ ||
                mixed_strain_queries_search_mode_ ||
                coupled_queries_search_mode_) {

                column_types.AddColumn(index++, Data, "Left query ID");
                column_types.AddColumn(index++, Data, "Left query");
                column_types.AddColumn(index++, Data, "Right query ID");
                column_types.AddColumn(index++, Data, "Right query");
                column_types.AddColumn(index++, Data, "Amplicon sequence");
                column_types.AddColumn(index++, Data, "Amplicon length");
            } else {
                column_types.AddColumn(index++, Data, "Query ID");
                column_types.AddColumn(index++, Data, "Query");
            }

            if (context_enabled_) {
                column_types.AddColumn(index++, Data, "Upstream of target");
                column_types.AddColumn(index++, Data, "Downstream of target");
            }
            column_types.saveConfigurationOfColumnTypesForFile(filePath());
            header_ = {"ID"};

            if (!paired_queries_search_mode_)
                header_.push_back("Target sequence");

            if (has_annotation_)
                header_.push_back("Annotation");

            if (paired_queries_search_mode_ ||
                mixed_strain_queries_search_mode_ ||
                coupled_queries_search_mode_) {

                header_.push_back("Left query start");
                header_.push_back("Amplicon start");
                header_.push_back("Amplicon end");
                header_.push_back("Right query end");
                header_.push_back("Left query ID");
                header_.push_back("Left query");
                header_.push_back("Right query ID");
                header_.push_back("Right query");
                header_.push_back("Amplicon sequence");
                header_.push_back("Amplicon length");
            } else {
                header_.push_back("Start position");
                header_.push_back("End position");
                header_.push_back("Query ID");
                header_.push_back("Query");
            }

            if (context_enabled_) {
                header_.push_back("Upstream of target");
                header_.push_back("Downstream of target");
            }

            out_file_->WriteQuoted(header_[0]);
            for (int i = 1; i < header_.size(); i++) {
                out_file_->Write(separator_);
                out_file_->WriteQuoted(header_[i]);
            }
            out_file_->WriteLine();
            break;
        }
    }
}

}  // namespace gene
