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

#include "Splitter.hpp"
#include "../../utils/CppUtils.hpp"
#include "../../utils/StringUtils.hpp"
#include "../../log/Logger.hpp"

Splitter::Splitter(const std::string& input_path,
                   const std::string& output_path,
                   std::unique_ptr<CommandLineFlags>&& flags)
: Operation(std::move(flags)), inputFilePath(input_path), outputFilePath(output_path)
{
}

bool Splitter::Init_()
{
    if (!(inFile = SequenceFile::FileWithName(inputFilePath, flags_, OpenMode::Read))) {
        PrintfLog("Can't open input file\n");
        return false;
    }
    if (!inFile->isValidGeneFile()) {
        PrintfLog("Input file has an invalid format\n");
        return false;
    }
    
    outFileName = utils::ConstructOutputNameWithFile(inFile->filePath(), inFile->fileType(), outputFilePath, flags_, "-split");
    if (outFileName.empty())
        return false;
    
    recordLimit = flags_->GetIntSetting("r");
    fileLimit = flags_->GetIntSetting("f");
    int kb = flags_->GetIntSetting("sk");
    int mb = flags_->GetIntSetting("sm");
    
    int definedFlagsNo = (recordLimit ? 1 : 0) + (fileLimit ? 1 : 0) + (kb ? 1 : 0) + (mb ? 1 : 0);
    
    if (definedFlagsNo != 1) {
        PrintfLog("One and exactly one of -r, -f, -sk, -sm flags should be specified\n");
        return false;
    }
    sizeLimit = mb*1024*1024 + kb*1024;
    return true;
}

bool Splitter::Process()
{
    if (!Operation::Process()) {
        return false;
    }
    
    if (fileLimit) {
        // Estimate file size
        sizeLimit = (inFile->length()/(double)fileLimit)*101l/100l;
    }
    
    if (flags_->verbose) {
        PrintfLog("Splitting <-%s(%s)\n", inFile->filePath().c_str(), inFile->strFileType().c_str());
        if (fileLimit)
            PrintfLog("Trying to get %lld files each of approximately %lldKB\n", fileLimit, sizeLimit/1024);
        else if (recordLimit)
            PrintfLog("Writing maximum %d records per file\n", recordLimit);
        else
            PrintfLog("Writing maximum %lld bytes per file\n", sizeLimit);
    }
    
    SequenceRecord record;
    auto start = std::chrono::high_resolution_clock::now();
    long counter = 0;
    int recordCounter = 0;
    
    SequenceFilePtr outFile = nullptr;
    
    int fileNumber = 0;
    int64_t lastChunkStart = 0;
    
    while (!(record = inFile->Read()).Empty()) {
        if (!outFile) {
            // Open next
            ++fileNumber;
            std::string outPath = utils::InsertSuffixBeforePathExtension(outFileName, std::to_string(fileNumber));

            if (!(outFile = SequenceFile::FileWithName(outPath, flags_, OpenMode::Write))) {
                PrintfLog("Can't create output file %s\n", outPath.c_str());
                return false;
            }
            if (flags_->verbose)
                PrintfLog("Splitting into ->%s(%s)\n", outFile->filePath().c_str(), outFile->strFileType().c_str());
        }
        outFile->Write(record);
        ++counter;
        
        if (recordLimit) {
            // by records
            if (++recordCounter > recordLimit) {
                recordCounter = 0;
                outFile = nullptr;
            }
        } else {
            // By size
            if (inFile->position() - lastChunkStart >= sizeLimit) {
                lastChunkStart = inFile->position();
                outFile = nullptr;
            }
        }
        if (HasToUpdateProgress_(counter) && update_progress_callback) {
            bool hasToCancelOperation = update_progress_callback(inFile->position()/(float)inFile->length()*100.0);
            if (hasToCancelOperation) {
                return true;
            }
        }
    }
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    
    if (flags_->verbose) {
        PrintfLog("%ld records processed in %.2f seconds\n", counter, elapsed.count());
    }
    return true;
}