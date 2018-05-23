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

#include <cmath>
#include <type_traits>

#include "SequenceRecord.hpp"
#include "../../utils/CppUtils.hpp"
#include "../alignment/sam/SamRecord.hpp"
#include "../../search/FuzzySearch.hpp"

namespace gene {

SequenceRecord::SequenceRecord(SamRecord&& sam)
{
    name = std::move(sam.QNAME);
    desc = " reference_sequence_name:" + std::move(sam.RNAME);
    seq = std::move(sam.SEQ);

    if (sam.QUAL != "*") {
        quality = std::move(sam.QUAL);
    } else {
        // Write long string from default quality
        std::string pad_string(seq.size(), 'I');
        quality = pad_string;
    }
}

SequenceRecord::SequenceRecord(const SequenceRecord& other)
{
    name = other.name;
    desc = other.desc;
    seq = other.seq;
    quality = other.quality;
    if (other.annotation) {
        annotation = std::make_unique<GenBankAnnotation>(*other.annotation);
    }
}

SequenceRecord& SequenceRecord::operator=(const SequenceRecord& other)
{
    if (this != &other) {
        name = other.name;
        desc = other.desc;
        seq = other.seq;
        quality = other.quality;
        if (other.annotation) {
            annotation = std::make_unique<GenBankAnnotation>(*other.annotation);
        }
    }
    return *this;
}

/*
 * Returns 'true' iff the sequence contains the barcode AND what is left is
 * long enough to consider.
*/
bool SequenceRecord::trimBarcodeSingleEnd(const std::string& barcode,
                                          const int length,
                                          bool mismatch_allowed)
{
    int64_t barcode_position;
    if (mismatch_allowed)
        barcode_position = FuzzySearch::FindByHamming1(seq, barcode);
    else
        barcode_position = FuzzySearch::NAwareFind(seq, barcode);
    
    if (barcode_position == std::string::npos)
        return false;
    
    const int64_t barcode_length = barcode.size();
    const int64_t sequence_length = seq.size();
    const int64_t barcode_end_position = barcode_position + barcode_length;
    
    if (sequence_length - barcode_end_position < length)
        // The resulting sequence would be too short,
        // so don't bother – discard it
        return false;
    
    auto trimBeginningOfString = [](std::string& str, int64_t prefix_length)
    {
        if (prefix_length > (int64_t)str.size()) {
            // Doesn't make sense to remove beginning that is longer than the
            // length of the string.
            str.clear();
            return;
        }
        auto start = str.begin();
        str.erase(start, start + prefix_length);
    };
    
    auto trimEndingOfString = [](std::string& str, int64_t suffix_length)
    {
        if (suffix_length > (int64_t)str.size()) {
            // Doesn't make sense to remove ending that is longer than the
            // length of the string.
            str.clear();
            return;
        }
        auto end = str.end();
        str.erase(end - suffix_length, end);
    };
    
    trimBeginningOfString(seq, barcode_end_position);
    trimEndingOfString(seq, (int64_t)seq.size() - length);
    
    if (!quality.empty()) {
        trimBeginningOfString(quality, barcode_end_position);
        trimEndingOfString(quality, (int64_t)quality.size() - length);
    }
    return true;
}

void SequenceRecord::UppercaseBases()
{
    for (char& c: seq) {
        if (c > 'Z')
            c -= 32;
    }
}

void SequenceRecord::ShiftQuality(FastqVariant from, FastqVariant to)
{
    auto fastqVariantToMinChar = [](FastqVariant variant)
    {
        switch (variant) {
            case FastqVariant::Illumina1_8:
            case FastqVariant::Sanger:
                return 33;
            case FastqVariant::Illumina1_5:
                return 66;
            case FastqVariant::Illumina1_3:
                return 64;
            case FastqVariant::Solexa:
                return 59;
        }
    };
    
    auto fastqVariantToMaxChar = [](FastqVariant variant)
    {
        switch (variant) {
            case FastqVariant::Illumina1_8:
                return 74;
            case FastqVariant::Illumina1_5:
            case FastqVariant::Illumina1_3:
            case FastqVariant::Solexa:
                return 104;
            case FastqVariant::Sanger:
                return 73;
        }
    };
    
    int offset = fastqVariantToMinChar(to) - fastqVariantToMinChar(from);
    int clamp = fastqVariantToMaxChar(to);
    for (char& c: quality) {
        c += offset;
        if (c > clamp)
            c = clamp;
    }
}

}  // namespace gene
