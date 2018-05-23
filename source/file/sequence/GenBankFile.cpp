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

#include <cassert>
#include <string_view>
#include <string>
#include <cctype>

#include "GenBankFile.hpp"

using std::string_view;

namespace gene {

GenBankFile::GenBankFile(const std::string& path,
                         const std::unique_ptr<CommandLineFlags>& flags,
                         OpenMode mode)
: SequenceFile(path, flags, FileType::GenBank, mode)
{
    
}

std::vector<std::string> GenBankFile::extensions()
{
    return {"gb", "gbk"};
}

std::string GenBankFile::defaultExtension()
{
    return "gb";
}

std::string GenBankFile::displayExtension()
{
    return defaultExtension();
}

SequenceRecord GenBankFile::Read()
{
    last_read_line_ = in_file_->ReadLine();
    if (last_read_line_.find("LOCUS") != 0) {
        last_read_line_.clear();
        return SequenceRecord();
    }

    int64_t id_start_position = last_read_line_.find_first_not_of(' ', sizeof("LOCUS") - 1);
    std::string desc;
    std::string name;
    if (id_start_position != std::string::npos) {
        int64_t id_end_position = last_read_line_.find_first_of(' ', id_start_position);
        if (id_end_position == std::string::npos)
            id_end_position = last_read_line_.size();

        name = last_read_line_.substr(id_start_position, id_end_position - id_start_position);
        if (id_end_position != last_read_line_.size()) {
            int64_t description_start = last_read_line_.find_first_not_of(' ', id_end_position + 1);
            desc = last_read_line_.substr(description_start);
        }
    } else {
        // No ID found. Consider this a format error
        return SequenceRecord();
    }

    std::string annotation_lines;
    // Skip the rest of the information that we don't need and look for the line that starts with
    // "ORIGIN".
    bool entered_feature_section = false;
    while (!(last_read_line_ = in_file_->ReadLine()).empty()) {
        if (last_read_line_.empty() || last_read_line_.find("ORIGIN") == 0) {
            // Found the beginning of the "ORIGIN" section (which is the beginning of the sequence
            // lines).
            break;
        } else if (entered_feature_section) {
            string_view skipped_5_spaces = string_view(last_read_line_).substr(5);
            annotation_lines.append(skipped_5_spaces.data(), skipped_5_spaces.size());
            annotation_lines += '\n';
        } else if (last_read_line_.find("FEATURES") == 0) {
            entered_feature_section = true;
        }
    }

    GenBankAnnotation annotation(annotation_lines);

    std::string data;
    while (!(last_read_line_ = in_file_->ReadLine()).empty()) {
        if (last_read_line_.empty() || last_read_line_.find("//") == 0) {
            // Found the beginning of the next sequence
            break;
        }

        int64_t sequence_block_starts_position = -1;
        std::string sequence_block;
        while ((sequence_block_starts_position = last_read_line_.find_first_of("atgcunATGCUN", sequence_block_starts_position + 1)) != std::string::npos) {
            int64_t next_space_position = last_read_line_.find(' ', sequence_block_starts_position + 1);

            sequence_block = last_read_line_.substr(sequence_block_starts_position, next_space_position - sequence_block_starts_position);

            for (char &c: sequence_block)
                 c = std::toupper(c);

            data += sequence_block;
            if (next_space_position == std::string::npos)
                break;

            sequence_block_starts_position = next_space_position;
        }
    }
    return SequenceRecord(std::move(name), std::move(desc), std::move(data), std::move(annotation));
}

std::vector<std::string> GenBankFile::ReadVec()
{
    std::vector<std::string> components;
    assert(false);  // Not implemented
    return components;
}

void GenBankFile::Write(const SequenceRecord& record)
{
    assert(false);
}

bool GenBankFile::isValidGeneFile() const
{
    return true;
}

}  // namespace gene
