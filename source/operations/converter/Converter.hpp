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

#ifndef LIBGENE_OPERATIONS_CONVERTER_HPP_
#define LIBGENE_OPERATIONS_CONVERTER_HPP_

#include <vector>
#include <memory>

#include "../Operation.hpp"
#include "../../file/sequence/SequenceFile.hpp"
#include "../../file/alignment/AlignmentFile.hpp"
#include "../../flags/CommandLineFlags.hpp"
#include "../../def/Flags.hpp"

class Converter final : public Operation {
 public:
    Converter(const std::vector<std::string>& input_paths,
              const std::string& output_path,
              std::unique_ptr<CommandLineFlags>&& flags);
    bool Process() override;
    
 private:
    SequenceFilePtr outFile;
    std::vector<SequenceFilePtr> sequenceInputFiles;
    std::vector<std::unique_ptr<AlignmentFile>> alignmentInputFiles;

    int64_t totalSizeInBytes;
    std::string outputFilePath;
    std::vector<std::string> inputPaths;
    bool fastqFormatConversion{false};
    FastqVariant inputFastqVariant;
    FastqVariant outputFastqVariant;
    bool Init_() override;
};

#endif  // LIBGENE_OPERATIONS_CONVERTER_HPP_
