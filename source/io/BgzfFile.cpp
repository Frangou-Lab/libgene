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

#include "BgzfFile.hpp"
#include "../file/alignment/bam/BamUtils.hpp"
#include "../log/Logger.hpp"

#include <cstdint>
#include <cstdlib>
#include <zlib.h>

namespace gene {

constexpr int32_t kGzipHeaderSize = 12;
constexpr int32_t kExtraHeaderSize = 6;

BgzfFile::BgzfFile(const std::string& path, OpenMode mode)
: current_block_exhausted_(true), block_offset_(0)
{
    file_ = fopen(path.c_str(), (mode == OpenMode::Read ? "rb" : "wb"));
    
    if (!file_)
        return;
    
    fseek(file_, 0L, SEEK_END);
    file_size_ = ftell(file_);
    fseek(file_, 0L, SEEK_SET);
}

void BgzfFile::ReadCompressionBlock_()
{
    uint8_t compressionBlock[kGzipHeaderSize];
    int64_t read = fread(compressionBlock, 1, kGzipHeaderSize, file_);
    if (read == 0) {
        return;
    }
    
    uint8_t id1 = bmtls::getUint8(compressionBlock);
    uint8_t id2 = bmtls::getUint8(compressionBlock + 1);
    if (id1 != 31 || id2 != 139) {
        // Damaged gzip header
        PrintfLog("Damaged gzip header. Unexpected ID1 and ID2 values.");
        return;
    }
    
    header_.XLEN = bmtls::getUint16(compressionBlock + 10);
    if (header_.XLEN != 6) {
        // This can't be a BAM file
        PrintfLog("XLEN != 6. Unknow format. File doesn't contain extra subfields required for BAM.");
        return;
    }
}

void BgzfFile::ReadExtraBlock_()
{
    uint8_t extraBlock[kExtraHeaderSize];
    int64_t read = fread(extraBlock, 1, kExtraHeaderSize, file_);
    if (read == 0) {
        return;
    }
    
    extra_subfields_.si1 = bmtls::getUint8(extraBlock);
    extra_subfields_.si2 = bmtls::getUint8(extraBlock + 1);
    if (extra_subfields_.si1 != 66 || extra_subfields_.si2 != 67) {
        // Damaged BGZF
        PrintfLog("Damaged BGZF file. Unexpected SI1 and SI2 values.");
        return;
    }
    extra_subfields_.subfieldLength = bmtls::getUint16(extraBlock + 2);
    extra_subfields_.blockSize = bmtls::getUint16(extraBlock + 4);
}

void BgzfFile::ReadFooter_()
{
    uint8_t footer[8];
    fread(footer, 1, 8, file_);
    // uint32_t isize = bmtls::getUint32(footer + 4);
}

bool BgzfFile::ReadNextBlock()
{
    ReadCompressionBlock_();
    ReadExtraBlock_();
    if (current_block) {
        int carryOverSize = current_block->size() - block_offset_;
        uint8_t* carryBlock;
        if (carryOverSize > 0) {
            carryBlock = (uint8_t*)malloc(carryOverSize);
            std::memcpy(carryBlock, current_block->data() + block_offset_, carryOverSize);
        } else {
            if (position() == length()) {
                return false;
            }
            carryOverSize = 0;
        }
        block_offset_ = 0;
        current_block = std::make_unique<BgzfBlock>(file_,
                                                    extra_subfields_.blockSize,
                                                    carryOverSize,
                                                    carryBlock);
        if (carryOverSize > 0) {
            free(carryBlock);
        }
    } else {
        current_block = std::make_unique<BgzfBlock>(file_,
                                                    extra_subfields_.blockSize,
                                                    0,
                                                    nullptr);
    }
    ReadFooter_();
    current_block_exhausted_ = false;
    return true;
}

int64_t BgzfFile::position() const noexcept
{
    return ftell(file_);
}

int64_t BgzfFile::length() const noexcept
{
    return file_size_;
}

BgzfFile::~BgzfFile() noexcept
{
    if (file_) {
        fclose(file_);
    }
}

}  // namespace gene
