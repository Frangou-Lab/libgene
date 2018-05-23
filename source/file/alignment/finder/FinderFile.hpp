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

#ifndef LIBGENE_FILE_ALIGNMENT_FINDER_FINDERFILE_HPP_
#define LIBGENE_FILE_ALIGNMENT_FINDER_FINDERFILE_HPP_

#include <string>
#include <memory>

#include "../../SeparatedFile.hpp"

namespace gene {

class FinderRecord;

class FinderFile : public SeparatedFile {
 private:
    int surrounding_sequence_col_;
    bool context_enabled_{false};
    bool paired_queries_search_mode_{false};
    bool coupled_queries_search_mode_{false};
    bool mixed_strain_queries_search_mode_{false};
    bool has_annotation_{false};

    void Prepare_(OpenMode mode) override;

 public:
    FinderFile(std::string path,
               const std::unique_ptr<CommandLineFlags>& flags,
               OpenMode mode,
               char separator);

    static FinderFile FileWithName(std::string name,
                                   const std::unique_ptr<CommandLineFlags>& flags,
                                   OpenMode mode);

    void Write(FinderRecord& record);
};

}  // namespace gene

#endif  // LIBGENE_FILE_ALIGNMENT_FINDER_FINDERFILE_HPP_
