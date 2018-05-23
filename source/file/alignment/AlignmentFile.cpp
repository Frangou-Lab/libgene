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
#include <vector>

#include "AlignmentFile.hpp"
#include "sam/SamFile.hpp"
#include "bam/BamFile.hpp"
#include "bed/BedFile.hpp"
#include "../../utils/StringUtils.hpp"
#include "../../utils/CppUtils.hpp"

namespace gene {

AlignmentFile::AlignmentFile(std::string path,
                             FileType type,
                             const std::unique_ptr<CommandLineFlags>& flags,
                             OpenMode mode)
: IOFile(path, type, mode)
{
}

AlignmentFile::AlignmentFile(std::string path,
                             FileType type,
                             const std::unique_ptr<CommandLineFlags>& flags)
: IOFile(path, type)
{
}

std::string AlignmentFile::strFileType() const
{
    return utils::type2str(type_);
}

auto AlignmentFile::FileWithName(const std::string& path, OpenMode mode)
-> AlignmentFile::AlignmentFilePtr
{
    auto flags = std::make_unique<CommandLineFlags>();
    return FileWithName(path, flags, mode);
}

auto AlignmentFile::FileWithName(const std::string& path,
                                 const std::unique_ptr<CommandLineFlags>& flags,
                                 OpenMode mode)
-> AlignmentFile::AlignmentFilePtr
{
    try {
        auto extension = utils::GetExtension(path);
        if (extension == "sam" || extension == "SAM") {
            return std::make_unique<SamFile>(path, flags, mode);
        } else if (extension == "bam" || extension == "BAM") {
            return std::make_unique<BamFile>(path, flags, mode);
        } else if (extension == "bed" || extension == "BED") {
            return std::make_unique<BedFile>(path, flags, mode);
        }
        return nullptr;
    } catch (std::runtime_error& error) {
        return nullptr;
    }
}

std::vector<std::string> AlignmentFile::supportedExtensions()
{
    auto extensions = SamFile::extensions();
    auto bamExtensions = BamFile::extensions();
    extensions.insert(extensions.end(), bamExtensions.begin(), bamExtensions.end());
    return extensions;
}

std::vector<std::string> AlignmentFile::defaultFileFormats()
{
    return {SamFile::defaultExtension(), BamFile::defaultExtension()};
}

}  // namespace gene
