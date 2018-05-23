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

#ifndef LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERSQ_HPP_
#define LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERSQ_HPP_

#include <string>
#include <cstdint>

#include "SamHeaderComponent.hpp"

namespace gene {

// Description:
// Reference sequence dictionary. The order of @SQ lines defines the alignment
// sorting order.
class SamHeaderSQ : public SamHeaderComponent {
 public:
    SamHeaderSQ() = default;
    explicit SamHeaderSQ(const std::string& line);
    ~SamHeaderSQ() = default;

    // MANDATORY FIELD
    // Reference sequence name. Each @SQ line must have a unique SN tag. The
    // value of this field is used in the alignment records in RNAME and RNEXT
    // fields. Regular expression: [!-)+-<>-~][!-~]*
    std::string SN;

    // MANDATORY FIELD. Reference sequence length. Range: [1,231-1]
    uint8_t LN;

    // Genome assembly identifier.
    std::string AS;

    // MD5 checksum of the sequence in the uppercase, excluding spaces but
    // including pads (as ‘*’s).
    std::string M5;

    // Species.
    std::string SP;

    // URI of the sequence. This value may start with one of the standard
    // protocols, e.g http: or ftp:. If it does not start with one of these
    // protocols, it is assumed to be a file-system path.
    std::string UR;

    std::string report() const override;
};

}  // namespace gene

#endif  // LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERSQ_HPP_

