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

#ifndef LIBGENE_FILE_ALIGNMENT_BAM_BAMUTILS_HPP_
#define LIBGENE_FILE_ALIGNMENT_BAM_BAMUTILS_HPP_

#include <algorithm>
#include <cstdint>

namespace bmtls {

static inline uint8_t getUint8(const uint8_t arr[]) {
    return arr[0];
}

static inline uint16_t getUint16(const uint8_t arr[]) {
    return uint16_t(uint8_t(arr[0])) |
    uint16_t(uint8_t(arr[1])) << 8;
}

static inline uint32_t getUint32(const uint8_t arr[]) {
    return uint32_t(uint8_t(arr[0])) |
    uint32_t(uint8_t(arr[1])) << 8  |
    uint32_t(uint8_t(arr[2])) << 16 |
    uint32_t(uint8_t(arr[3])) << 24;
}

static inline uint64_t getUint64(const uint8_t arr[]) {
    return uint64_t(uint8_t(arr[0])) |
    uint64_t(uint8_t(arr[1])) << 8  |
    uint64_t(uint8_t(arr[2])) << 16 |
    uint64_t(uint8_t(arr[3])) << 24 |
    uint64_t(uint8_t(arr[4])) << 32 |
    uint64_t(uint8_t(arr[5])) << 40 |
    uint64_t(uint8_t(arr[6])) << 48 |
    uint64_t(uint8_t(arr[7])) << 56;
}

}  // namespace bmtls

#endif  // LIBGENE_FILE_ALIGNMENT_BAM_BAMUTILS_HPP_
