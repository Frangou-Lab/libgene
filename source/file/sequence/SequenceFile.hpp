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

#ifndef LIBGENE_FILE_SEQUENCE_SEQUENCEFILE_HPP_
#define LIBGENE_FILE_SEQUENCE_SEQUENCEFILE_HPP_

#include <string>
#include <vector>
#include <memory>

#include "SequenceRecord.hpp"
#include "../../io/IOFile.hpp"
#include "../../flags/CommandLineFlags.hpp"

enum class FileKind {
    SingleEnd,
    PairedEnd_1,
    PairedEnd_2
};

class SamRecord;

class SequenceFile : public IOFile {
 private:
    bool AttemptToDetermineFileKind_();

 protected:
    FileKind file_kind_{FileKind::SingleEnd};
    bool verbose_{false};

    SequenceFile(const std::string& path,
                 const std::unique_ptr<CommandLineFlags>& flags,
                 FileType type);

 public:
    typedef std::unique_ptr<SequenceFile> SequenceFilePtr;

    SequenceFile(const std::string& path,
                 const std::unique_ptr<CommandLineFlags>& flags,
                 FileType type,
                 OpenMode mode);

    static SequenceFilePtr FileWithName(const std::string& name, OpenMode mode);
    static SequenceFilePtr FileWithName(const std::string& name,
                                        const std::unique_ptr<CommandLineFlags>& flags,
                                        OpenMode mode);

    virtual std::string strFileType() const;
    virtual bool isValidGeneFile() const = 0;
    FileKind fileKind() const;

    virtual SequenceRecord Read() = 0;
    virtual std::vector<std::string> ReadVec() = 0;
    virtual void Write(const SequenceRecord& record) = 0;

    static std::vector<std::string> supportedExtensions();
    static std::vector<std::string> defaultFileFormats();
};

#endif  // LIBGENE_FILE_SEQUENCE_SEQUENCEFILE_HPP_
