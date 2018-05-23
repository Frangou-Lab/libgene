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

#ifndef LIBGENE_FILE_ALIGNMENT_SAM_SAMTAG_HPP_
#define LIBGENE_FILE_ALIGNMENT_SAM_SAMTAG_HPP_

#include <string>
#include <cstdint>
#include <map>

namespace gene {

class SamTag {
 private:
    union value {
        int8_t c;    // 'c'
        uint8_t C;   // 'C'
        int16_t s;   // 's'
        uint16_t S;  // 'S'
        int32_t i;   // 'i'
        uint32_t I;  // 'I'
        struct {     // 'A'
            uint64_t metadata;
            void* contents;
        } A;
    } val;           // Tag value

    char tag[2];        // Two-character tag
    char val_type;      // Value type: AcCsSiIfZHB

 public:
    SamTag() = default;
    int readTag(const uint8_t* start, int32_t tag_length);

    std::map<std::string, int32_t> intTags;
    std::map<std::string, std::string> stringTags;
    std::map<std::string, double> doubleTags;
};

}  // namespace gene

#endif  // LIBGENE_FILE_ALIGNMENT_SAM_SAMTAG_HPP_
