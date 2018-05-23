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

#include "SequenceFile.hpp"

#include "FastaFile.hpp"
#include "FastqFile.hpp"
#include "GenomicCsvFile.hpp"
#include "GenomicTsvFile.hpp"
#include "GenBankFile.hpp"
#include "../TxtFile.hpp"
#include "../../utils/StringUtils.hpp"
#include "../../utils/CppUtils.hpp"
#include "../../def/Flags.hpp"

namespace gene {

SequenceFile::SequenceFile(const std::string& path,
                           const std::unique_ptr<CommandLineFlags>& flags,
                           FileType type,
                           OpenMode mode)
: IOFile(path, type, mode)
{
    verbose_ = flags->verbose;
    if (mode == OpenMode::Read)
        AttemptToDetermineFileKind_();
}

SequenceFile::SequenceFile(const std::string& path,
                           const std::unique_ptr<CommandLineFlags>& flags,
                           FileType type)
: IOFile(path, type)
{
    verbose_ = flags->verbose;
}

std::string SequenceFile::strFileType() const
{
    return utils::type2str(type_);
}

FileKind SequenceFile::fileKind() const
{
    return file_kind_;
}

std::unique_ptr<SequenceFile> SequenceFile::FileWithName(const std::string& name, OpenMode mode)
{
    auto flags = std::make_unique<CommandLineFlags>();
    return FileWithName(name, flags, mode);
}

std::unique_ptr<SequenceFile> SequenceFile::FileWithName(const std::string& name,
                                                         const std::unique_ptr<CommandLineFlags>& flags,
                                                         OpenMode mode)
{
    FileType type = (mode == OpenMode::Read) ? flags->inputFormat() : flags->outputFormat();
    if (type == FileType::Unknown) {  // Determine from name
        type = utils::extension2type(utils::GetExtension(name));
        if (type == FileType::Unknown) {
            auto extension = utils::GetExtension(utils::StringByDeletingPathExtension(name));
            type = utils::extension2type(extension); // For example: 'fastq.gz'
        }
    }

    switch (type) {
        case FileType::Fasta:
            return std::make_unique<FastaFile>(name, flags, mode);
        case FileType::Fastq:
            return std::make_unique<FastqFile>(name, flags, mode);
        case FileType::Csv:
            return std::make_unique<GenomicCsvFile>(name, flags, mode);
        case FileType::Tsv:
            return std::make_unique<GenomicTsvFile>(name, flags, mode);
        case FileType::GenBank:
            return std::make_unique<GenBankFile>(name, flags, mode);
        case FileType::PlainTxt:
            return std::make_unique<TxtFile>(name, flags, mode);
        default:
            return nullptr;
    }
}

//
// Sometimes FASTQ (& FASTA) files come in pairs which contain sequences read
// from different directions of the same sequence. This provides additional
// information about positional realtions of corresponding reads. It is
// important to keep these sequences separated after demultiplexing.
//
bool SequenceFile::AttemptToDetermineFileKind_()
{
    std::string name = fileName();
    int extension_start_index = static_cast<int>(name.rfind('.'));
    
    if (extension_start_index == std::string::npos)
        return false;
    
    if (name.substr(extension_start_index) == ".gz")
        extension_start_index = static_cast<int>(name.substr(0, extension_start_index - 1).find_last_of('.'));
    
    if (extension_start_index < 3 ||
        extension_start_index == std::string::npos) {
        // File name is too short to contain 'endinness' information
        return false;
    }
    
    std::string two_letters_before_extension = name.substr(extension_start_index - 2, 2);
    if (two_letters_before_extension == "_1" ||
        name.find("_R1") != std::string::npos) {
        file_kind_ = FileKind::PairedEnd_1;
    } else if (two_letters_before_extension == "_2" ||
             name.find("_R2") != std::string::npos) {
        file_kind_ = FileKind::PairedEnd_2;
    } else {
        file_kind_ = FileKind::SingleEnd;
    }
    return (file_kind_ != FileKind::SingleEnd);
}

std::vector<std::string> SequenceFile::supportedExtensions()
{
    auto extensions = FastaFile::extensions();

    std::vector<std::string> tempExtensions = FastqFile::extensions();
    extensions.insert(extensions.end(), tempExtensions.begin(), tempExtensions.end());

    tempExtensions = GenomicTsvFile::extensions();
    extensions.insert(extensions.end(), tempExtensions.begin(), tempExtensions.end());
    
    tempExtensions = GenomicCsvFile::extensions();
    extensions.insert(extensions.end(), tempExtensions.begin(), tempExtensions.end());
    
    tempExtensions = GenBankFile::extensions();
    extensions.insert(extensions.end(), tempExtensions.begin(), tempExtensions.end());
    
    return extensions;
}

std::vector<std::string> SequenceFile::defaultFileFormats()
{
    return {FastaFile::defaultExtension(),
            FastqFile::defaultExtension(),
            GenomicCsvFile::defaultExtension(),
            GenomicTsvFile::defaultExtension(),
            GenBankFile::defaultExtension()};
}

}  // namespace gene
