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

#ifndef LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERCOMPONENT_HPP_
#define LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERCOMPONENT_HPP_

#include <string>
#include <cassert>

class SamHeaderComponent {
 public:
    virtual ~SamHeaderComponent() = default;

 protected:
    std::string ExtractNextTagValue_(const std::string& line,
                                     int64_t *tag_position) {
        assert(tag_position != nullptr);
        int64_t next_space_position = line.find(' ', *tag_position);
        *tag_position += 3;
        if (next_space_position == std::string::npos)
            return line.substr(*tag_position);
        else
            return line.substr(*tag_position,
                               next_space_position - *tag_position);
    }
    virtual std::string report() const = 0;
};

#endif  // LIBGENE_FILE_ALIGNMENT_SAM_SAMHEADERCOMPONENT_HPP_
