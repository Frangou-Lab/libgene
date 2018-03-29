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

#ifndef LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERHD_HPP_
#define LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERHD_HPP_

#include <string>
#include <memory>

#include "SamHeaderComponent.hpp"

class SamHeaderHD : public SamHeaderComponent {
 public:
    SamHeaderHD() = default;
    explicit SamHeaderHD(const std::string& line);
    ~SamHeaderHD() = default;

    /// MANDATORY FIELD. Format version. Accepted format: /^[0-9]+\.[0-9]+$/.
    std::string VN;

    // Sorting order of alignments.
    std::string SO;

    // Grouping of alignments, indicating that similar alignment records are
    // grouped together but the file is not necessarily sorted overall.
    // Valid values: none (default), query (alignments are grouped
    // by QNAME), and reference (alignments are grouped by RNAME/POS).
    std::string GO;

    std::string report() const override;
};

#endif  // LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERHD_HPP_

