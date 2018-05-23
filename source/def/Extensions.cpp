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

#include <iostream>
#include <vector>
#include <string>

#include "Extensions.hpp"
#include "../file/sequence/FastaFile.hpp"
#include "../file/sequence/FastqFile.hpp"
#include "../file/sequence/GenBankFile.hpp"
#include "../file/sequence/GenomicCsvFile.hpp"
#include "../file/sequence/GenomicTsvFile.hpp"
#include "../file/sequence/SequenceFile.hpp"

namespace gene {

static std::vector<std::string> finder_file_formats;

std::vector<std::string> Extensions::kFinderDisplayInputFileFormats()
{
    return {FastaFile::displayExtension(),
            FastqFile::displayExtension(),
            GenomicCsvFile::displayExtension(),
            GenomicTsvFile::displayExtension()};
}

std::vector<std::string> Extensions::kFinderInputFileFormats()
{
    if (!finder_file_formats.empty())
        return finder_file_formats;

    auto extensions = FastaFile::extensions();
    finder_file_formats.insert(finder_file_formats.end(),
                               extensions.begin(), extensions.end());

    extensions = FastqFile::extensions();
    finder_file_formats.insert(finder_file_formats.end(),
                               extensions.begin(), extensions.end());

    extensions = GenomicTsvFile::extensions();
    finder_file_formats.insert(finder_file_formats.end(),
                               extensions.begin(), extensions.end());

    extensions = GenomicCsvFile::extensions();
    finder_file_formats.insert(finder_file_formats.end(),
                               extensions.begin(), extensions.end());

    extensions = GenBankFile::extensions();
    finder_file_formats.insert(finder_file_formats.end(),
                               extensions.begin(), extensions.end());

    finder_file_formats.push_back("gz");
    return finder_file_formats;
}

std::vector<std::string> Extensions::kFinderReferenceFileFormats()
{
    auto extensions = FastaFile::extensions();
    extensions.push_back("gz");

    auto tempExtensions = GenBankFile::extensions();
    extensions.insert(extensions.end(),
                      tempExtensions.begin(), tempExtensions.end());

    return extensions;
}

std::vector<std::string> Extensions::kInputQueriesFileFormat()
{
    return {"txt"};
}

}  // namespace gene
