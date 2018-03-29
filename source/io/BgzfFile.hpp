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

#ifndef BgzfFile_hpp
#define BgzfFile_hpp

#include <string>
#include <memory>

#include "BgzfBlock.hpp"
#include "../file/alignment/sam/SamHeader.hpp"
#include "../file/alignment/sam/SamRecord.hpp"
#include "../io/IOFile.hpp"

class BgzfFile {
 private:
    struct BgzfHeader {
        uint8_t ID1; // gzip IDentifier1
        uint8_t ID2; // gzip IDentifier2
        uint8_t CM;  // gzip Compression Method
        uint8_t FLG; // gzip FLaGs
        uint32_t MTIME;  // gzip Modification TIME
        uint8_t XFL; // gzip eXtra FLags
        uint8_t OS; // gzip Operating System
        uint16_t XLEN; // gzip eXtra LENgth
    } header_;
    
    struct ExtraGzipSubfields {
        uint8_t si1;    // Subfield identifier 1
        uint8_t si2;    // Subfield identifier 2
        uint16_t subfieldLength;
        uint16_t blockSize;
    } extra_subfields_;
    
    int64_t file_size_;
    
    void ReadCompressionBlock_();
    void ReadExtraBlock_();
    void ReadFooter_();

 protected:
    FILE* file_;
    bool current_block_exhausted_;
    int32_t block_offset_;
    std::unique_ptr<SamHeader> sam_header_;

 public:
    BgzfFile(const std::string& path, OpenMode mode);
    virtual ~BgzfFile() noexcept;

    bool ReadNextBlock();
    std::unique_ptr<BgzfBlock> current_block;

    int64_t length() const noexcept;
    int64_t position() const noexcept;
};

#endif /* BgzfFile_hpp */
