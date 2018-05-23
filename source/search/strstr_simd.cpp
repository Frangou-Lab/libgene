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

#include <string>
#include <immintrin.h>

#include "strstr_simd.hpp"

#ifdef DEBUG

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
(byte & 0x80 ? '1' : '0'), \
(byte & 0x40 ? '1' : '0'), \
(byte & 0x20 ? '1' : '0'), \
(byte & 0x10 ? '1' : '0'), \
(byte & 0x08 ? '1' : '0'), \
(byte & 0x04 ? '1' : '0'), \
(byte & 0x02 ? '1' : '0'), \
(byte & 0x01 ? '1' : '0')

#endif  // DEBUG

static std::string_view match_by_pairs_sse2(std::string_view haystack,
                                            const char pattern[2]);

#if 0
static const char* match_by_quads_avx2(const char* haystack,
                                       const char pattern[4]);
#endif  // 0

#ifdef DEBUG
/*
 Debug functions to print the contents of XMM and YMM registers.
*/
void Print_v128i_epi8(__m128i v)
{
    fprintf(stderr, "{");
    for (int i = 15; i > 0; --i)
        fprintf(stderr, "" BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY((*(((uint8_t*) &v) + i))));
    fprintf(stderr, "" BYTE_TO_BINARY_PATTERN"}\n", BYTE_TO_BINARY((*(((uint8_t*) &v)))));
    fflush(stderr);
}

void Print_v256i_epi8(__m256i v)
{
    fprintf(stderr, "{");
    for (int i = 31; i > 0; --i)
        fprintf(stderr, "" BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY((*(((uint8_t*) &v) + i))));
    fprintf(stderr, "" BYTE_TO_BINARY_PATTERN"}\n", BYTE_TO_BINARY((*(((uint8_t*) &v)))));
    fflush(stderr);
}
/* */
#endif  // DEBUG

#ifdef _MSC_VER
int ffs(uint32_t s)
{
    if (s == 0)
        return 0;
    
    int bit_position = 0;
    while (s) {
        bit_position++;
        s <<= 1;
    }
    return 32 - bit_position + 1;
}
#endif // _MSC_VER

namespace gene {

std::string_view strstr_simd(std::string_view haystack,
                             const std::string& needle)
{
    if (needle.size() < 2) {
        int64_t position = haystack.find(needle);
        if (position != std::string::npos)
            return haystack.substr(position, needle.size());
        else
            return std::string_view();
    }
    const char *needle_ = needle.c_str();
    const int64_t length = needle.size();
    for (; !(haystack = match_by_pairs_sse2(haystack, needle_)).empty(); haystack.remove_prefix(1))
    {
        if (haystack.size() >= length && !memcmp(haystack.data() + 2, needle_ + 2, length - 2))
            return haystack;
    }
    return std::string_view();
}

}  // namespace gene

static std::string_view match_by_pairs_sse2(std::string_view haystack,
                                            const char pattern[2])
{
    const __m128i zero_mask = _mm_setzero_si128();
    const __m128i c0_pattern = _mm_set1_epi8(pattern[0]);
    const __m128i c1_pattern = _mm_set1_epi8(pattern[1]);
    uint8_t unaligned_length = 0x0F & (intptr_t)haystack.data(); // a part of the string, which
                                                                 // requires special treatment.

    uint16_t pair = *(uint16_t *)(pattern);
    uint32_t eq_zero_mask, match_mask;

    if (unaligned_length != 0) {
        __m128i unaligned_chars = _mm_load_si128((const __m128i*)(haystack.data() - unaligned_length));
        eq_zero_mask = _mm_movemask_epi8(_mm_cmpeq_epi8(zero_mask, unaligned_chars));
        eq_zero_mask >>= unaligned_length;
        
        match_mask =  _mm_movemask_epi8(_mm_cmpeq_epi8(c0_pattern, unaligned_chars));
        match_mask &= _mm_movemask_epi8(_mm_cmpeq_epi8(c1_pattern, unaligned_chars)) >> 1;
        match_mask >>= unaligned_length;
        match_mask &= ~eq_zero_mask & (eq_zero_mask - 1);
        
        if (match_mask) {
            int first_bit_set = ffs(match_mask);
            return haystack.substr(first_bit_set - 1);
        }
        if (eq_zero_mask)
            return std::string_view();
        
        // Store a copy because we can't increase the length of the string_view
        // after we've shrunk it
        std::string_view temp_copy = haystack;
        haystack.remove_prefix(16 - unaligned_length);
        
        if (*((uint16_t*)(haystack.data() - 1)) == pair)
            return temp_copy.substr(15 - unaligned_length); // Used to be: '(16 - unaligned_length) - 1'
    }
    
    for (;;) {
        __m128i next16_chars = _mm_load_si128((const __m128i*)haystack.data());
        eq_zero_mask = _mm_movemask_epi8(_mm_cmpeq_epi8(zero_mask, next16_chars));
        
        match_mask =  _mm_movemask_epi8(_mm_cmpeq_epi8(c0_pattern, next16_chars));
        match_mask &= _mm_movemask_epi8(_mm_cmpeq_epi8(c1_pattern, next16_chars)) >> 1;
        match_mask &= ~eq_zero_mask & (eq_zero_mask - 1);
        if (match_mask) {
            int first_bit_set = ffs(match_mask);
            return haystack.substr(first_bit_set - 1);
        }
        if (eq_zero_mask)
            return std::string_view();
        
        std::string_view copy = haystack;
        haystack.remove_prefix(16);
        
        if (*((uint16_t*)(haystack.data() - 1)) == pair)
            return copy.substr(15); // 16 - 1
    }
}
