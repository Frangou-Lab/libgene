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

#ifndef LIBGENE_FILE_ALIGNMENT_BAM_BAMFILE_HPP_
#define LIBGENE_FILE_ALIGNMENT_BAM_BAMFILE_HPP_

#include <vector>
#include <iostream>
#include <memory>
#include <string>

#include "../AlignmentFile.hpp"
#include "../sam/SamHeader.hpp"
#include "../../../io/BgzfFile.hpp"

class SamRecord;

class BamFile : public AlignmentFile, BgzfFile {
 private:
    class ReferenceInfo {
     public:
        std::string name;
        int32_t l_ref;

        void PrintDebug() const {
            std::cout << "Ref info:\n";
            std::cout << "name: " << name << '\n';
            std::cout << "l_ref: " << l_ref << '\n';
        }
    };

    void ReadSamHeader();
    std::vector<ReferenceInfo> refs;

 public:
    BamFile(const std::string& path,
            const std::unique_ptr<CommandLineFlags>& flags,
            OpenMode Mode);

    std::string strFileType() const override;
    bool isValidAlignmentFile() const override;

    SamRecord read() override;
    void write(const SamRecord& record) override;

    static std::string defaultExtension();
    static std::vector<std::string> extensions();

    std::unique_ptr<SamHeader> header;

    int64_t position() const override;
    int64_t length() const override;
};

#endif  // LIBGENE_FILE_ALIGNMENT_BAM_BAMFILE_HPP_
