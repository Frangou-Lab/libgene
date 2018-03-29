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

#include "GenomicTsvFile.hpp"

#include <string>
#include <vector>
#include <memory>

GenomicTsvFile::GenomicTsvFile(const std::string& path,
                               const std::unique_ptr<CommandLineFlags>& flags,
                               OpenMode mode)
: GenomicSeparatedFile(path, flags, FileType::Tsv, mode, '\t')
{
    
}

std::vector<std::string> GenomicTsvFile::extensions()
{
    return {"tsv", "tsvc", "tsvr", "tsvcr"};
}

std::string GenomicTsvFile::defaultExtension()
{
    return "tsvc";
}

std::string GenomicTsvFile::displayExtension()
{
    return "tsv";
}
