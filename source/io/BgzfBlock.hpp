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

#ifndef BgzfBlock_hpp
#define BgzfBlock_hpp

#include <cstdint>
#include <cstdio>

class BgzfBlock {
 private:
    bool InflateBlock_();
    bool ReadBlock_(FILE* file);
    
    uint8_t* compressed_data_;
    uint8_t* decompressed_data_;
    int32_t compressed_size_;
    int32_t decompressed_size_;
    int32_t carryover_length_;

 public:
    BgzfBlock(FILE* file,
              int32_t block_size,
              int32_t carryover_length,
              uint8_t* carryover_data);
    ~BgzfBlock() noexcept;
    const uint8_t* data() const noexcept;
    int32_t size() const noexcept;
};

#endif /* BgzfBlock_hpp */
