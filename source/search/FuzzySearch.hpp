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

#ifndef LIBGENE_SEARCH_FUZZYSEARCH_HPP_
#define LIBGENE_SEARCH_FUZZYSEARCH_HPP_

#include <string>
#include <vector>
#include <string_view>

#include "../operations/filter/Filter.hpp"
#include "../search/SearchPrimer.hpp"

namespace gene {

enum class Strand {
    Forward,
    Reverse,
    Both
};

struct PositionAndScore {
    uint32_t position{0}; // I think, 4GB addressing in a single sequence should be enough
    uint32_t score{0};
};

struct SearchResult {
    int64_t position;
    int64_t matched_length;
    Strand where;
    
    SearchResult(int64_t position, Strand where)
    : position(position), where(where) {
        
    }
    SearchResult(std::pair<int64_t, int64_t> position, Strand where)
    : position(std::get<0>(position)),
      matched_length(std::get<1>(position)),
      where(where) {
        
    }
};

class FuzzySearch final {
 public:
    explicit FuzzySearch(int8_t errorCount);
    ~FuzzySearch() = default;

    std::vector<SearchResult> AllOccurrencesOfQuery(const SearchPrimer& query,
                                                    Strand search_strand,
                                                    bool levenshtein_search) const;

    std::vector<std::pair<int64_t, int64_t>> AllOccurrencesOfQuery(const std::string &query,
                                                                   bool levenshtein_search) const;
    std::vector<int64_t> AllOccurrencesOfQueryAsBindingPrimer(const std::string& query) const;
    std::vector<PositionAndScore> AllOccurrencesByLevenshtein2(const std::string& query) const;

    std::pair<int64_t, int64_t> EdgemostOccurrenceOfQuery(const SearchPrimer& query,
                                                          const int64_t already_found,
                                                          const int64_t query_length,
                                                          bool complementary_search,
                                                          const bool levenshtein_search) const;
    std::string_view reference;

    static int64_t FindByHamming1(std::string_view haystack, std::string_view needle);
    static int64_t FindByHamming2(std::string_view haystack, std::string_view needle);

    static int64_t Find(std::string_view haystack, std::string_view needle)
    {
        const int64_t needle_length = needle.size();
        const int64_t high = (int64_t)(haystack.size()) - needle_length;

        bool next_iteration;
        for (int64_t i = 0; i <= high; ++i) {
            next_iteration = false;
            for (int64_t j = 0; j < needle_length; ++j) {
                if (haystack[i + j] != needle[j]) {
                    next_iteration = true;
                    break;
                }
            }
            if (next_iteration)
                continue;

            // Found a match
            return i;
        }
        return std::string::npos;
    }

    template <int MaxDistance_>
    static int64_t FindHammingLimit(std::string_view haystack,
                                    std::string_view needle,
                                    int& distance,
                                    bool ignore_unrecognized)
    {
        const int64_t needle_length = needle.size();
        const int64_t high = (int64_t)(haystack.size()) - needle_length;

        for (int64_t i = 0; i <= high; ++i) {
            distance = 0;
            for (int64_t j = 0; j < needle_length; ++j) {
                if (haystack[i + j] != needle[j]) {
                    ++distance;
                    if (distance > MaxDistance_)
                        break;
                }
            }
            if (distance <= MaxDistance_)
                return i;
        }
        return std::string::npos;
    }

    template <int N_Limit_ = std::numeric_limits<int>::max()>
    static int64_t NAwareFind(std::string_view haystack, std::string_view needle)
    {
        int64_t needle_length = needle.size();
        int64_t high = (int64_t)haystack.size() - needle_length;

        bool next_iteration;
        int N_counted;
        for (int64_t i = 0; i <= high; ++i) {
            next_iteration = false;
            N_counted = 0;
            for (int64_t j = 0; j < needle_length; ++j) {
                if (haystack[i + j] != needle[j]) {
                    if (haystack[i + j] == 'N' || needle[j] == 'N') {
                        ++N_counted;
                        if (N_counted > N_Limit_) {
                            next_iteration = true;
                            break;
                        }
                    } else {
                        next_iteration = true;
                        break;
                    }
                }
            }
            if (next_iteration)
                continue;

            // Found a match
            return i;
        }
        return std::string::npos;
    }

 private:
    std::pair<int64_t, int64_t> LeftmostPosition_(const std::string &query,
                                                  bool levenshtein_search = false) const;
    std::pair<int64_t, int64_t> RightmostPosition_(const std::string &query,
                                                   bool levenshtein_search = false) const;

    const int16_t max_error_count_;
};

}  // namespace gene

#endif  // LIBGENE_SEARCH_FUZZYSEARCH_HPP_
