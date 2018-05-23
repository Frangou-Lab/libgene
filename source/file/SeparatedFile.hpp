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

#ifndef LIBGENE_FILE_SEPARATEDFILE_HPP_
#define LIBGENE_FILE_SEPARATEDFILE_HPP_

#include <vector>
#include <string>
#include <memory>

#include "../flags/CommandLineFlags.hpp"
#include "../io/IOFile.hpp"
#include "../utils/Tokenizer.hpp"
#include "../plist/ColumnTypesConfigurationCpp.hpp"

namespace gene {

class SeparatedFile : public IOFile {
 protected:
    virtual void Prepare_(OpenMode mode);
    void ReadHeader_();

    bool header_present_;
    bool header_done_;
    std::vector<std::string> header_;
    std::unique_ptr<Tokenizer> tokenizer_;
    char separator_;

 public:
    SeparatedFile(const std::string& path,
                  const std::unique_ptr<CommandLineFlags>& flags,
                  OpenMode mode,
                  char separator);

    bool HeaderPresent() const;
    const std::vector<std::string>& header() const;
    std::string LastReadString() const;
    std::unique_ptr<Tokenizer>& tokenizer();
    char separator() const;

    std::vector<std::string> ReadNextRow();
    // Needs refactoring
    void SetUpWithReadMode(bool read);
    void AddColumn(int columnId, enum ColumnType type, std::string description);
    void SaveConfigurationOfColumnTypesForFile(std::string inputFileName);
    void AddColumnToHeader(std::string columnName);

    void SetHeader(const std::vector<std::string>& header);
    void WriteHeaderIntoFile();

    void Write(const std::string& str);
    void Write(char ch);
    void WriteRow(const std::vector<std::string>& row);
    void WriteLine(const std::string& str);
    void WriteLine();
    void WriteQuoted(const std::string& str);

    std::string ReadLine();
    std::unique_ptr<ColumnTypesConfigurationCpp> column_types;
};

}  // namespace gene

#endif  // LIBGENE_FILE_SEPARATEDFILE_HPP_
