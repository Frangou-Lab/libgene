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

#include <chrono>

#include "Converter.hpp"
#include "../../utils/StringUtils.hpp"
#include "../../utils/CppUtils.hpp"
#include "../../file/alignment/sam/SamRecord.hpp"
#include "../../file/sequence/SequenceRecord.hpp"
#include "../../log/Logger.hpp"

Converter::Converter(const std::vector<std::string>& input_paths,
                     const std::string& output_path,
                     std::unique_ptr<CommandLineFlags>&& flags)
: Operation(std::move(flags)), inputPaths(input_paths), outputFilePath(output_path), totalSizeInBytes(0)
{
    bool hasInputFormatSet = (flags_->GetSetting(Flags::kInputFormat) != nullptr);
    auto outputFormat = *flags_->GetSetting(Flags::kOutputFormat);
    bool fastqWithScale = (outputFormat.find("fastq") != std::string::npos &&
                           outputFormat != "fastq");
    
    bool fastqInputFormat = false;
    if (hasInputFormatSet) {
        fastqInputFormat = flags_->GetSetting(Flags::kInputFormat)->find("fastq") !=
                           std::string::npos;
    }
    fastqFormatConversion = hasInputFormatSet && fastqInputFormat && fastqWithScale;
    if (fastqFormatConversion) {
        auto inputFormat = *flags_->GetSetting(Flags::kInputFormat);
        inputFastqVariant = utils::FormatNameToVariant(inputFormat);
        outputFastqVariant = utils::FormatNameToVariant(outputFormat);
    }
}

bool Converter::Init_()
{
    for (const auto& filePath: inputPaths) {
        auto extension = utils::GetExtension(filePath);
        FileType type = utils::str2type(extension);
        
        if (type != FileType::Sam && type != FileType::Bam) {
            auto inFile = SequenceFile::FileWithName(filePath, flags_, OpenMode::Read);
            if (inFile) {
                totalSizeInBytes += inFile->length();
                sequenceInputFiles.push_back(std::move(inFile));
            }
        } else {
            auto inFile = AlignmentFile::FileWithName(filePath, flags_, OpenMode::Read);
            if (inFile) {
                totalSizeInBytes += inFile->length();
                alignmentInputFiles.push_back(std::move(inFile));
            }
        }
    }
    
    if (!((sequenceInputFiles.empty() || sequenceInputFiles[0]->isValidGeneFile()) &&
          (alignmentInputFiles.empty() || alignmentInputFiles[0]->isValidAlignmentFile()))) {
        PrintfLog("Input file has an invalid format\n");
        return false;
    }
    
    if (outputFilePath.empty()) {
        outputFilePath = utils::ConstructOutputNameWithFile(inputPaths.front(),
                                                            FileType::Unknown,
                                                            outputFilePath,
                                                            flags_,
                                                            "-converted");
    }
    
    if (!(outFile = SequenceFile::FileWithName(outputFilePath, flags_, OpenMode::Write))) {
        PrintfLog("Can't create output file\n");
        return false;
    }
    return true;
}

bool Converter::Process()
{
    if (!Operation::Process())
        return false;
    
    if (flags_->verbose && !sequenceInputFiles.empty()) {
        PrintfLog("Converting %s(%s) -> %s(%s)\n", sequenceInputFiles[0]->filePath().c_str(),
               sequenceInputFiles[0]->strFileType().c_str(), outFile->filePath().c_str(),
               outFile->strFileType().c_str());
    }
    
    SequenceRecord record;
    auto start = std::chrono::high_resolution_clock::now();
    int64_t counter = 0;
    int64_t bytesProcessed = 0;

    for (const auto& inputFile : sequenceInputFiles) {
        while (!(record = inputFile->Read()).Empty()) {
            if (HasToUpdateProgress_(counter) && update_progress_callback) {
                bool hasToCancelOperation = update_progress_callback((inputFile->position() + bytesProcessed)/static_cast<float>(totalSizeInBytes*100.0));
                if (hasToCancelOperation)
                    return true;
            }
            
            ++counter;
            if (fastqFormatConversion)
                record.ShiftQuality(inputFastqVariant, outputFastqVariant);
            
            outFile->Write(record);
        }
        bytesProcessed += inputFile->length();
    }
    
    SamRecord samRecord;
    for (const auto& inputFile : alignmentInputFiles) {
        while (!(samRecord = inputFile->read()).SEQ.empty()) {
            if (HasToUpdateProgress_(counter) && update_progress_callback) {
                bool hasToCancelOperation = update_progress_callback((inputFile->position() + bytesProcessed)/static_cast<float>(totalSizeInBytes*100.0));
                if (hasToCancelOperation)
                    return true;
            }
            
            ++counter;
            SequenceRecord r{std::move(samRecord)};
            outFile->Write(r);
        }
        bytesProcessed += inputFile->length();
    }
    
    if (counter == 0) {
        PrintfLog("Input file was either empty, or it had an incorrect format\n");
        return false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> secondsElapsed = end - start;
    
    if (flags_->verbose)
        PrintfLog("%ld records processed in %.2f seconds\n", counter, secondsElapsed.count());

    return true;
}
