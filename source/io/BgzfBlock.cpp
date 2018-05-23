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

#include "BgzfBlock.hpp"
#include <cstdlib>
#include <cstring>
#include <zlib.h>

namespace gene {

constexpr int kBgzfBlockMaxSize = 16 << 20;

BgzfBlock::BgzfBlock(FILE* file,
                     int32_t block_size,
                     int32_t carryover_length,
                     uint8_t* carryover_data)
: compressed_size_(block_size),
  carryover_length_(carryover_length)
{
    decompressed_data_ = (uint8_t*)malloc(kBgzfBlockMaxSize);
    if (carryover_data && carryover_length_ > 0) {
        std::memcpy(decompressed_data_, carryover_data, carryover_length_);
    }

    ReadBlock_(file);
    InflateBlock_();
}

bool BgzfBlock::ReadBlock_(FILE* file)
{
    compressed_data_ = (uint8_t *)malloc(compressed_size_ - 25);
    fread(compressed_data_, 1, compressed_size_ - 25, file);
    return true;
}

bool BgzfBlock::InflateBlock_()
{
    z_stream stream;
    stream.avail_in = compressed_size_;
    stream.avail_out = kBgzfBlockMaxSize;
    stream.next_in = compressed_data_;
    stream.next_out = decompressed_data_ + carryover_length_;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    if (int err = inflateInit2(&stream, -15); err == Z_OK) {
        err = inflate(&stream, Z_FINISH);
        inflateEnd(&stream);
    }
    decompressed_size_ = static_cast<int32_t>(stream.total_out) + carryover_length_;
    
    if (compressed_data_) {
        free(compressed_data_);
    }
    return true;
}

const uint8_t* BgzfBlock::data() const noexcept
{
    return decompressed_data_;
}

int32_t BgzfBlock::size() const noexcept
{
    return decompressed_size_;
}

BgzfBlock::~BgzfBlock() noexcept
{
    if (decompressed_data_)
        free(decompressed_data_);
}

}  // namespace gene
