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

#ifndef LIBGENE_FILE_ALIGNMENT_ALIGNMENTFILE_HPP_
#define LIBGENE_FILE_ALIGNMENT_ALIGNMENTFILE_HPP_

#include <string>
#include <vector>
#include <memory>

#include "../../io/IOFile.hpp"

namespace gene {

class CommandLineFlags;
class AlignmentRecord;
class SamRecord;

class AlignmentFile : public IOFile {
    typedef std::unique_ptr<AlignmentFile> AlignmentFilePtr;

 protected:
    AlignmentFile(std::string path,
                  FileType type,
                  const std::unique_ptr<CommandLineFlags>& flags,
                  OpenMode mode);
    AlignmentFile(std::string path,
                  FileType type,
                  const std::unique_ptr<CommandLineFlags>& flags);

 public:
    static AlignmentFilePtr FileWithName(const std::string& path,
                                         OpenMode mode);

    static AlignmentFilePtr FileWithName(const std::string& path,
                                         const std::unique_ptr<CommandLineFlags>& flags,
                                         OpenMode mode);

    virtual std::string strFileType() const;
    virtual bool isValidAlignmentFile() const = 0;

    virtual SamRecord read() = 0;
    virtual void write(const SamRecord& record) = 0;

    static std::vector<std::string> supportedExtensions();
    static std::vector<std::string> defaultFileFormats();

    virtual int64_t position() const = 0;
    virtual int64_t length() const = 0;
};

}  // namespace libgene

#endif  // LIBGENE_FILE_ALIGNMENT_ALIGNMENTFILE_HPP_
