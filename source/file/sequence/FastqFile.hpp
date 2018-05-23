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

#ifndef FastqFile_hpp
#define FastqFile_hpp

#include <string>
#include <vector>
#include <memory>

#include "../../file/sequence/SequenceFile.hpp"
#include "../../flags/CommandLineFlags.hpp"

namespace gene {

class FastqFile final : public SequenceFile {
 private:
    char def_quality_;
    bool duplicate_;
    bool override_existing_quality_;

 public:
    FastqFile(const std::string& path,
              const std::unique_ptr<CommandLineFlags>& flags,
              OpenMode mode);
    virtual ~FastqFile() = default;

    bool isValidGeneFile() const override;
    SequenceRecord Read() override;
    std::vector<std::string> ReadVec() override;
    void Write(const SequenceRecord& record) override;

    static std::string defaultExtension();
    static std::string displayExtension();
    static std::vector<std::string> extensions();
};

}  // namespace gene

#endif /* FastqFile_hpp */
