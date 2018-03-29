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

#ifndef LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERRG_HPP_
#define LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERRG_HPP_

#include <string>

#include "SamHeaderComponent.hpp"

class SamHeaderRG : public SamHeaderComponent {
 public:
    SamHeaderRG() = default;
    explicit SamHeaderRG(const std::string& line);
    ~SamHeaderRG() = default;

    // MANDATORY FIELD.
    // Read group identifier. Each @RG line must have a unique ID. The value of
    // ID is used in the RG tags of alignment records. Must be unique among all
    // read groups in header section. Read group IDs may be modified when
    // merging SAM files in order to handle collisions.
    std::string ID;

    // Name of sequencing center producing the read.
    std::string CN;

    // Description.
    std::string DS;

    // Date the run was produced (ISO8601 date or date/time).
    std::string DT;

    // Flow order. The array of nucleotide bases that correspond to the
    // nucleotides used for each flow of each read. Multi-base flows are
    // encoded in IUPAC format, and non-nucleotide flows by various other
    // characters. Format: /\*|[ACMGRSVTWYHKDBN]+/
    std::string FO;

    // The array of nucleotide bases that correspond to the key sequence of
    // each read.
    std::string KS;

    // Library.
    std::string LB;

    // Programs used for processing the read group.
    std::string PG;

    // Predicted median insert size.
    std::string PI;

    // Platform/technology used to produce the reads. Valid values: CAPILLARY,
    // LS454, ILLUMINA, SOLID, HELICOS, IONTORRENT, ONT and PACBIO.
    std::string PL;

    // Platform model. Free-form text providing further details of the
    // platform/technology used.
    std::string PM;

    // Platform unit (e.g. flowcell-barcode.lane for Illumina or slide for
    // SOLiD). Unique identifier.
    std::string PU;

    // Sample. Use pool name where a pool is being sequenced.
    std::string SM;

    std::string report() const override;
};

#endif  // LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERRG_HPP_
