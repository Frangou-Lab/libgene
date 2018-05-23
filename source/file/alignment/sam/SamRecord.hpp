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

#ifndef LIBGENE_FILE_ALIGNMENT_SAM_SAMRECORD_HPP_
#define LIBGENE_FILE_ALIGNMENT_SAM_SAMRECORD_HPP_

#include <string>
#include <cstdint>

#include "../AlignmentRecord.hpp"
#include "SamTag.hpp"

namespace gene {

class SamRecord : public AlignmentRecord {
 public:
    SamRecord() = default;
    explicit SamRecord(const std::string& line);
    virtual ~SamRecord() = default;

    std::string QNAME;
    uint32_t FLAG;
    std::string RNAME;
    int32_t POS;
    int32_t MAPQ;
    std::string CIGAR;
    std::string RNEXT;
    int32_t PNEXT;
    int32_t TLEN;
    std::string SEQ;
    std::string QUAL;

    SamTag tag;

    // BED-related fields
    std::string chrom;
    int64_t chromStart;
    int64_t chromEnd;
    // The 9 additional optional BED fields are:
    std::string name;
    int score;
    char strand;
    int64_t thickStart;
    int64_t thickEnd;
    int32_t itemRgb;
    int64_t blockCount;
    int64_t blockSizes;
    int64_t blockStart;
};

}  // namespace gene

#endif  // LIBGENE_FILE_ALIGNMENT_SAM_SAMRECORD_HPP_

