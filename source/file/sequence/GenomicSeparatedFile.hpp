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

#ifndef LIBGENE_FILE_SEQUENCE_GENOMICSEPARATEDFILE_HPP
#define LIBGENE_FILE_SEQUENCE_GENOMICSEPARATEDFILE_HPP

#include <vector>
#include <string>
#include <memory>

#include "../../plist/ColumnTypesConfigurationCpp.hpp"
#include "../../flags/CommandLineFlags.hpp"
#include "../SeparatedFile.hpp"
#include "SequenceFile.hpp"

class GenomicSeparatedFile : public SequenceFile {
 protected:
    std::unique_ptr<SeparatedFile> file_;
    int name_col_;
    int desc_col_;
    int seq_col_;
    int quality_col_;
    std::vector<std::string> header_;
    std::string columns_reordering_;
    bool skip_quality_column_{false};
    bool no_column_defs_{true};

 private:
    void ParseColumnsInfo_();
    void Prepare_(OpenMode mode);

 public:
    GenomicSeparatedFile(const std::string& path,
                         const std::unique_ptr<CommandLineFlags>& flags,
                         FileType type,
                         OpenMode mode,
                         char separator);

    bool isValidGeneFile() const override;
    SequenceRecord Read() override;
    std::vector<std::string> ReadVec() override;
    void Write(const SequenceRecord& record) override;

    int64_t position() const override;
    int64_t length() const override;
    std::string strFileType() const override;
    std::string fileName() const override;
    std::string filePath() const override;

    std::vector<std::string> getHeader() const;
};

#endif  // LIBGENE_FILE_SEQUENCE_GENOMICSEPARATEDFILE_HPP
