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

#ifndef LIBGENE_FILE_SEQUENCE_SEQUENCERECORD_HPP_
#define LIBGENE_FILE_SEQUENCE_SEQUENCERECORD_HPP_

#include <string>
#include <vector>
#include <memory>

#include "../../def/FileType.hpp"
#include "GenBankAnnotation.hpp"

class SamRecord;

class SequenceRecord {
 public:
    std::string name;
    std::string desc;
    std::string seq;  // Alternatively, just this single string is set with the original line (see .txt file)
    std::string quality;
    std::unique_ptr<GenBankAnnotation> annotation{nullptr};

    SequenceRecord() = default;
    SequenceRecord(const SequenceRecord& other);
    SequenceRecord(SamRecord&& sam);
    
    SequenceRecord(std::string &&name, std::string &&desc, std::string &&seq) noexcept
    : name(name), desc(desc), seq(seq)
    {
    }

    SequenceRecord(std::string &&name, std::string &&desc, std::string &&seq, GenBankAnnotation&& annot) noexcept
    : SequenceRecord(std::move(name), std::move(desc), std::move(seq))
    {
        annotation = std::make_unique<GenBankAnnotation>(annot);
    }

    SequenceRecord(std::string &&name, std::string &&desc, std::string &&seq, std::string &&qual) noexcept
    : SequenceRecord(std::move(name), std::move(desc), std::move(seq))
    {
        quality = qual;
    }
    SequenceRecord& operator=(const SequenceRecord& other);
    ~SequenceRecord() = default;

    constexpr
    bool Empty() const noexcept
    {
        return seq.empty();
    }

    bool trimBarcodeSingleEnd(const std::string& barcode, const int length, bool mismatchAllowed);
    void UppercaseBases();
    void ShiftQuality(FastqVariant from, FastqVariant to);
};

class AminoAcidCpp {
 public:
    AminoAcidCpp();
    explicit AminoAcidCpp(const char *seq);
    explicit AminoAcidCpp(std::string aa);

    AminoAcidCpp(const std::string& seq, char mutated, int pos);
    AminoAcidCpp(char _1, char _2, char _3);
    AminoAcidCpp(const std::string& aa, int position);

    AminoAcidCpp mutate2(const std::string& nucleotides, const std::string& nucleotides2);
    std::vector<AminoAcidCpp> mutate(char nucleotide, char nucleotide2);
    AminoAcidCpp mutate(char nucleotide, char nucleotide2, int pos);
    std::string seq;
    char name;
};

#endif  // LIBGENE_FILE_SEQUENCE_SEQUENCERECORD_HPP_
