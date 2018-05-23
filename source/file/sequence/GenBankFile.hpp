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

#ifndef GenBankFile_hpp
#define GenBankFile_hpp

#include <string>
#include <vector>
#include <memory>

#include "SequenceFile.hpp"
#include "../../flags/CommandLineFlags.hpp"

namespace gene {

class GenBankFile final : public SequenceFile {
 public:
    GenBankFile(const std::string& path,
                const std::unique_ptr<CommandLineFlags>& flags,
                OpenMode mode);
    virtual ~GenBankFile() = default;

    bool isValidGeneFile() const override;
    SequenceRecord Read() override;
    std::vector<std::string> ReadVec() override;
    void Write(const SequenceRecord& record) override;

    static std::vector<std::string> extensions();
    static std::string defaultExtension();
    static std::string displayExtension();

 private:
    std::string last_read_line_;
};

}  // namespace gene

#endif /* GenBankFile_hpp */
