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

#ifndef LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERPG_HPP_
#define LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERPG_HPP_

#include <string>

#include "SamHeaderComponent.hpp"

class SamHeaderPG : public SamHeaderComponent {
 public:
    SamHeaderPG() = default;
    explicit SamHeaderPG(const std::string& line);
    ~SamHeaderPG() = default;

    // MANDATORY FIELD.
    // Program record identifier. Each @PG line must have a unique ID. The
    // value of ID is used in the alignment PG tag and PP tags of other @PG
    // lines. PG IDs may be modified when merging SAM files in order to handle
    // collisions.
    std::string ID;

    // Program name.
    std::string PN;

    // Command line.
    std::string CL;

    // Previous @PG-ID. Must match another @PG header’s ID tag. @PG records may
    // be chained using PP tag, with the last record in the chain having no PP
    // tag. This chain defines the order of programs that have been applied to
    // the alignment. PP values may be modified when merging SAM files in order
    // to handle collisions of PG IDs. The first PG record in a chain (i.e. the
    // one referred to by the PG tag in a SAM record) describes the most recent
    // program that operated on the SAM record. The next PG record in the chain
    // describes the next most recent program that operated on the SAM record.
    // The PG ID on a SAM record is not required to refer to the newest PG
    // record in a chain. It may refer to any PG record in a chain, implying
    // that the SAM record has been operated on by the program in that PG
    // record, and the program(s) referred to via the PP tag.
    std::string PP;

    // Description.
    std::string DS;

    // Program version.
    std::string VN;

    std::string report() const override;
};

#endif  // LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERPG_HPP_
