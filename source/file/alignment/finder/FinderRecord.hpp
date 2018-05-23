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

#ifndef LIBGENE_FILE_ALIGNMENT_FINDER_FINDERRECORD_HPP_
#define LIBGENE_FILE_ALIGNMENT_FINDER_FINDERRECORD_HPP_

#include <string>

namespace gene {

class FinderRecord final {
 public:
    std::string id;
    std::string target_sequence;
    std::string query;
    std::string query_id;
    std::string left_query;
    std::string left_query_id;
    std::string right_query;
    std::string right_query_id;
    std::string annotation;
    long start;
    long end;

    long left_match_length;
    long right_match_length;

    std::string upstream_seq;
    std::string downstream_seq;
    std::string internal_seq;
};

}  // namespace gene

#endif  // LIBGENE_FILE_ALIGNMENT_FINDER_FINDERRECORD_HPP_
