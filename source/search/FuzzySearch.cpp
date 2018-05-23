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

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <cassert>

#include "../def/Def.hpp"

#include "FuzzySearch.hpp"
#include "strstr_simd.hpp"

namespace gene {

FuzzySearch::FuzzySearch(int8_t errorCount) : max_error_count_(errorCount)
{

}

std::vector<SearchResult> FuzzySearch::AllOccurrencesOfQuery(const SearchPrimer& query,
                                                             Strand strandToSearch,
                                                             bool levenshteinMetricSearch) const
{
    std::vector<SearchResult> result;
    switch (strandToSearch) {
        case Strand::Forward:
        {
            auto forward = AllOccurrencesOfQuery(query.IsForward() ?
                                                 query.ForwardSeq() : query.ReverseSeq(),
                                                 levenshteinMetricSearch);
            for (const auto& position : forward)
                result.push_back(SearchResult{position, Strand::Forward});
            break;
        }
        case Strand::Reverse:
        {
            auto reverse = AllOccurrencesOfQuery(query.IsForward() ?
                                                 query.ReverseSeq() : query.ForwardSeq(),
                                                 levenshteinMetricSearch);
            for (const auto& position : reverse)
                result.push_back(SearchResult{position, Strand::Reverse});
            break;
        }
        case Strand::Both:
        {
            auto forward = AllOccurrencesOfQuery(query.IsForward() ?
                                                 query.ForwardSeq() : query.ReverseSeq(),
                                                 levenshteinMetricSearch);
            for (const auto& position : forward)
                result.push_back(SearchResult{position, Strand::Forward});
            
            auto reverse = AllOccurrencesOfQuery(query.IsForward() ?
                                                 query.ReverseSeq() : query.ForwardSeq(),
                                                 levenshteinMetricSearch);
            for (const auto& position : reverse)
                result.push_back(SearchResult{position, Strand::Reverse});
            break;
        }
    }
    return result;
}

void HammingDistanceSearch(std::vector<std::pair<int64_t, int64_t>>& occurrences,
                           const std::string_view& reference,
                           const std::string& query,
                           const int distance)
{
    const int64_t query_length = query.size();
    const int64_t end = static_cast<int64_t>(reference.size()) - query_length + 1;
    auto ref = &reference[0];
    int mismatch_count = 0;
    auto pat = query.c_str();
    bool skipIndex = false;
    
    if (query_length < 4) {
        for (int64_t start = 0; start < end; ++start) {
            for (int i = 0; i < query_length; ++i) {
                if (reference[start + i] != query[i]) {
                    ++mismatch_count;
                    if (mismatch_count > distance) {
                        mismatch_count = 0;
                        break;
                    }
                }
            }
        }
    } else {
        for (int64_t start = 0; start < end; ++start) {
            skipIndex = false;
            int64_t i = 0;
            mismatch_count = 0;
            for (; i < query_length - 3;) {
                int32_t ref4 = *(int32_t*)(ref + i + start);
                int32_t pat4 = *(int32_t*)(pat + i);
                int32_t mask = ref4 ^ pat4;
                if (mask) {
                    int16_t half = (int16_t)(mask);
                    if (half) {
                        if ((int8_t)(half))
                            mismatch_count++;

                        if ((int8_t)(half >> 8))
                            mismatch_count++;
                    }
                    
                    half = (int16_t)(mask >> 16);
                    if (half) {
                        if ((int8_t)(half))
                            mismatch_count++;
                        
                        if ((int8_t)(half >> 8))
                            mismatch_count++;
                    }
                    
                    if (mismatch_count > distance) {
                        skipIndex = true;
                        break;
                    }
                }
                i += 4;
            }
            
            if (skipIndex)
                continue;
            
            if (i < query_length - 1) {
                int16_t ref2 = *(int16_t*)(ref + i + start);
                int16_t pat2 = *(int16_t*)(pat + i);
                int16_t mask = ref2 ^ pat2;
                if (mask) {
                    if ((int8_t)(mask))
                        mismatch_count++;

                    if ((int8_t)(mask >> 8))
                        mismatch_count++;
                }
                if (mismatch_count > distance)
                    continue;
                i += 2;
            }
            
            if (i < query_length && reference[i + start] != pat[i] && mismatch_count + 1 > distance)
                continue;
            
            occurrences.push_back({start, query_length});
        }
    }
}

static int MatchScoreImpl(std::string_view haystack,
                          std::string_view needle,
                          int distance, int& resulting_length);

//
// Computes the inverse of the Levenshtein distance (Actual_distance = allowed_distance - result)
// between 'haystack' and 'needle' string views.
// Additionally, it computes the length of the matching sequence – 'resulting_length'
//
// Return value:
//    std::numeric_limits<int>::min() means that Levenshtein distance between the two
//    input views is greater than 'distance'.
//
static int MatchScore(std::string_view haystack, std::string_view needle, int distance, int& resulting_length)
{
    resulting_length = 0;
    return MatchScoreImpl(haystack, needle, distance, resulting_length);
}

static int MatchScoreImpl(std::string_view haystack,
                          std::string_view needle,
                          int distance, int& resulting_length)
{
    if (haystack.empty() && needle.size() <= distance)
        return distance;
    
    if (distance < 1) {
        for (int i = 0; i < needle.size(); ++i) {
            if (haystack[i] != needle[i])
                return std::numeric_limits<int>::min();
        }
        resulting_length += needle.size();
        return 0;
    }

    for (int i = 0; i < needle.size(); ++i) {
        resulting_length++;
        
        if (haystack[i] != needle[i]) {
            int mismatch = std::numeric_limits<int>::min();
            int gap = std::numeric_limits<int>::min();
            int deletion = std::numeric_limits<int>::min();
            
            int length_mismatch = 0;
            int length_gap = 0;
            int length_deletion = 0;
            
            
            // The following code is written this way to decrase the ammount of jumps required
            // One can simiplify it by leaving only one assignment to each variable.
            if (i + 1 < haystack.size()) {
                if (i + 1 < needle.size()) {
                    mismatch = MatchScore(haystack.substr(i + 1), needle.substr(i + 1), distance - 1, length_mismatch);
                    gap = MatchScore(haystack.substr(i + 1), needle.substr(i), distance - 1, length_gap);
                    deletion = MatchScore(haystack.substr(i), needle.substr(i + 1), distance - 1, length_deletion);
                } else if (i < needle.size()) {
                    gap = MatchScore(haystack.substr(i + 1), needle.substr(i), distance - 1, length_gap);
                }
            } else if (i < haystack.size() && i + 1 < needle.size()) {
                deletion = MatchScore(haystack.substr(i), needle.substr(i + 1), distance - 1, length_deletion);
            }
            
            if (gap >= deletion && gap >= mismatch && gap != std::numeric_limits<int>::min()) {
                resulting_length += length_gap;
                return gap;
            } else if (deletion >= gap && deletion >= mismatch && deletion != std::numeric_limits<int>::min()) {
                // We decided that it was deletion -> matching sequene's length decreases
                resulting_length += length_deletion;
                resulting_length -= 1;
                return deletion;
            } else {
                resulting_length += length_mismatch;
                return mismatch;
            }
        }
    }
    return distance;
}

// A special-case function for 'Reference' search which looks for the 'needle' comparing by
// Levenshtein distance of 2. This function performs approximately 2x faster than the general one.
static int MatchScoreLevenshtein2(std::string_view haystack, std::string_view needle)
{
    if (haystack.empty() && needle.size() <= 2)
        return 2;
    
    auto exactMatch = [](std::string_view haystack, std::string_view needle)
    {
        for (int i = 0; i < needle.size(); ++i) {
            if (haystack[i] != needle[i])
                return std::numeric_limits<int>::min();
        }
        return 0;
    };
    
    auto match1 = [&exactMatch](std::string_view haystack, std::string_view needle)
    {
        for (int i = 0; i < needle.size(); ++i) {
            if (haystack[i] != needle[i]) {
                int mismatch = std::numeric_limits<int>::min();
                int gap = std::numeric_limits<int>::min();
                int deletion = std::numeric_limits<int>::min();
                
                
                if (i + 1 < haystack.size()) {
                    if (i + 1 < needle.size()) {
                        mismatch = exactMatch(haystack.substr(i + 1), needle.substr(i + 1));
                        if (mismatch != 0) {
                            gap = exactMatch(haystack.substr(i + 1), needle.substr(i));
                            if (gap != 0)
                                deletion = exactMatch(haystack.substr(i), needle.substr(i + 1));
                        }
                    } else if (i < needle.size()) {
                        gap = exactMatch(haystack.substr(i + 1), needle.substr(i));
                    }
                } else if (i < haystack.size() && i + 1 < needle.size()) {
                    deletion = exactMatch(haystack.substr(i), needle.substr(i + 1));
                }
                return std::max(deletion, std::max(gap, mismatch));
            }
        }
        return 1;
    };
    
    for (int i = 0; i < needle.size(); ++i) {
        if (haystack[i] != needle[i]) {
            int mismatch = std::numeric_limits<int>::min();
            int gap = std::numeric_limits<int>::min();
            int deletion = std::numeric_limits<int>::min();
 
            if (i + 1 < haystack.size()) {
                if (i + 1 < needle.size()) {
                    mismatch = match1(haystack.substr(i + 1), needle.substr(i + 1));
                    if (mismatch != 0) {
                        gap = match1(haystack.substr(i + 1), needle.substr(i));
                        if (gap != 0)
                            deletion = match1(haystack.substr(i), needle.substr(i + 1));
                    }
                } else if (i < needle.size()) {
                    gap = match1(haystack.substr(i + 1), needle.substr(i));
                }
            } else if (i < haystack.size() && i + 1 < needle.size()) {
                deletion = match1(haystack.substr(i), needle.substr(i + 1));
            }
            return std::max(deletion, std::max(gap, mismatch));
        }
    }
    return 2;
}

static void LevenshteinDistanceSearch(std::vector<std::pair<int64_t, int64_t>>& occurrences,
                                      const std::string_view& reference,
                                      const std::string& query,
                                      const int distance)
{
    if (query.size() > reference.size() + distance)
        return;
    
    const int64_t high = static_cast<int64_t>(reference.size()) -
                          static_cast<int64_t>(query.size()) + distance;
    int resulting_length;
    for (auto i = 0; i <= high; ++i) {
        if (i >= reference.size())
            break;

        const int score = MatchScore(reference.substr(i), query, distance, resulting_length);
        if (score != std::numeric_limits<int>::min())
            occurrences.push_back({i, resulting_length});
    }
}

void ExactSearch(std::vector<std::pair<int64_t, int64_t>>& occurrences,
                 const std::string_view& reference,
                 const std::string& query)
{
    int64_t index = -1;
    std::string_view found;
    const char * const start = reference.data();
    
    while (!(found = strstr_simd(reference.substr(index + 1), query)).empty()) {
        index = found.data() - start;
        occurrences.push_back({index, query.size()});
    }
}


auto FuzzySearch::AllOccurrencesOfQuery(const std::string& query,
                                        bool levenshtein_search) const
-> std::vector<std::pair<int64_t, int64_t>>
{
    int64_t queryLength = query.size();
    std::vector<std::pair<int64_t, int64_t>> occurrences;
    
    if (reference.size() < queryLength || queryLength == 0)
        return occurrences;
    
    if (max_error_count_ == 0)
        ExactSearch(occurrences, reference, query);
    else if (levenshtein_search)
        LevenshteinDistanceSearch(occurrences, reference, query, max_error_count_);
    else
        HammingDistanceSearch(occurrences, reference, query, max_error_count_);

    return occurrences;
}

std::vector<PositionAndScore> FuzzySearch::AllOccurrencesByLevenshtein2(const std::string& query) const
{
    std::vector<PositionAndScore> result;
    if (query.size() > reference.size())
        return result;
    
    const uint32_t high = static_cast<uint32_t>(reference.size()) - static_cast<uint32_t>(query.size());
    for (uint32_t i = 0; i < high; ++i) {
        const int score = MatchScoreLevenshtein2(reference.substr(i), query);
        if (score != std::numeric_limits<int>::min()) {
            result.push_back({i, (uint32_t)score});
        }
    }
    return result;
}

std::vector<int64_t> FuzzySearch::AllOccurrencesOfQueryAsBindingPrimer(const std::string& query) const
{
    const int64_t query_length = query.size();
    int mismatch_count = 0;
    
    if (reference.size() < query_length || query_length == 0)
        return {};
    
    const int64_t end = static_cast<int64_t>(reference.size()) - query_length + 1;
    bool skip_current_index = false;
    std::vector<int64_t> occurrences;
    for (int64_t start = 0; start < end; ++start) {
        skip_current_index = false;
        for (int64_t i = 0; i < query_length; ++i) {
            char ref_char = reference[i + start];
            char pat_char = query[i];
            if (ref_char != pat_char) {
                if ((pat_char != 'D' || !(ref_char == 'A' || ref_char == 'T')) &&
                    (ref_char != 'U' || !(pat_char == 'T' || pat_char == 'A'))) {

                    mismatch_count++;
                    if (mismatch_count > max_error_count_) {
                        skip_current_index = true;
                        break;
                    }
                }
            }
        }
        mismatch_count = 0;
        
        if (skip_current_index)
            continue;
        
        occurrences.push_back(start);
    }
    return occurrences;
}

int64_t FuzzySearch::FindByHamming1(std::string_view haystack, std::string_view needle)
{
    const int64_t needle_length = needle.size();
    const int64_t high = (int64_t)haystack.size() - needle_length;

    if (high < 0) {
        // haystack is shorter than needle
        return std::string::npos;
    }

    for (int64_t i = 0; i <= high; ++i) {
        int64_t j = 0;
        bool next_iteration = false;
        for (; j < needle_length; ++j) {
            if (haystack[i + j] != needle[j] && needle[j] != 'N') {
                // Check if what we've ecnountered is a separator and this is
                // not the last iteration (being last and in this state is
                // still within Hamming distance of 1).
                next_iteration = (haystack[i + j] == '+' || haystack[i + j] == ':') && (j != needle_length - 1);
                // One mismatch is OK. Moving on to the next state
                j++;
                break;
            }
        }
        for (; j < needle_length && !next_iteration; ++j) {
            if (haystack[i + j] != needle[j] && needle[j] != 'N')
                next_iteration = true;
        }
        if (next_iteration)
            continue;

        return i;
    }
    return std::string::npos;
}

int64_t FuzzySearch::FindByHamming2(std::string_view haystack,
                                    std::string_view needle)
{
    const int64_t needle_length = needle.size();
    const int64_t high = (int64_t)haystack.size() - needle_length;

    if (high < 0) {
        // haystack is shorter than needle
        return std::string::npos;
    }

    for (int64_t i = 0; i <= high; ++i) {
        int64_t j = 0;
        bool next_iteration = false;

        for (; j < needle_length; ++j) {
            if (haystack[i + j] != needle[j] && needle[j] != 'N') {
                // Check if what we've ecnountered is a separator and this is
                // not the last iteration (being last and in this state is
                // still within Hamming distance of 1).
                next_iteration = (haystack[i + j] == '+' || haystack[i + j] == ':') && (j != needle_length - 1);
                // One mismatch is OK. Moving on to the next state
                j++;
                break;
            }
        }

        for (; j < needle_length; ++j) {
            next_iteration = false;
            if (haystack[i + j] != needle[j] && needle[j] != 'N') {
                // Check if what we've ecnountered is a separator and this is
                // not the last iteration (being last and in this state is
                // still within Hamming distance of 1).
                next_iteration = (haystack[i + j] == '+' || haystack[i + j] == ':') && (j != needle_length - 1);
                // One mismatch is OK. Moving on to the next state
                j++;
                break;
            }
        }
        for (; j < needle_length && !next_iteration; ++j) {
            if (haystack[i + j] != needle[j] && needle[j] != 'N')
                next_iteration = true;
        }
        if (next_iteration)
            continue;

        return i;
    }
    return std::string::npos;
}

//
// Searches for a query in a reference string and returns a position which is
// closest to either end of the string.
//
// 'alreadyFound' is a positon of an already found query used to discern which edge in
// gives longer internal sequence.
//
std::pair<int64_t, int64_t> FuzzySearch::EdgemostOccurrenceOfQuery(const SearchPrimer& query,
                                                                   const int64_t already_found,
                                                                   const int64_t query_length,
                                                                   bool searchForComplements,
                                                                   const bool levenshtein_search) const
{
    if (reference.size() < query.PrimerLength())
        return {std::string::npos, 0LL};
    
    const auto& sequence = (searchForComplements ? query.ReverseSeq() : query.ForwardSeq());
    
    std::pair<int64_t, int64_t> leftmost_stats = LeftmostPosition_(sequence, levenshtein_search);
    int64_t leftmost_position = std::get<0>(leftmost_stats);
    
    std::pair<int64_t, int64_t> rightmost_stats;
    if (leftmost_position != std::string::npos) {
        // Look for the rightmost occurrences
        rightmost_stats = RightmostPosition_(sequence, levenshtein_search);
        int64_t rightmost_position = std::get<0>(rightmost_stats);
        
        if (rightmost_position == leftmost_position) {
            // There is only one occurrence of this string
            return rightmost_stats;
        } else {
            // Return the occurrence, whichever one is closer to the end the reference string
            // int64_t end = static_cast<int64_t>(reference.size()) - rightmost + queryLength;
            int64_t reference_length = static_cast<int64_t>(sequence.size());
            if (already_found < leftmost_position) {
                if (std::llabs(leftmost_position - (already_found + query_length)) <
                    std::llabs(rightmost_position - (already_found + query_length))) {
                    return rightmost_stats;
                }
            } else if (already_found < rightmost_position) {
                if (std::llabs(already_found - (leftmost_position + reference_length)) <
                    std::llabs(rightmost_position - (already_found + query_length))) {
                    return rightmost_stats;
                }
            } else if (std::llabs(already_found - (leftmost_position + reference_length)) <
                    std::llabs(already_found - (rightmost_position + reference_length))) {
                return rightmost_stats;
            }
            else
                return leftmost_stats;
        }
    }
    return {std::string::npos, 0LL};
}

std::pair<int64_t, int64_t> FuzzySearch::LeftmostPosition_(const std::string &query,
                                                           bool levenshtein_search) const
{
    if (max_error_count_ == 0)
        return {reference.find(query), query.size()};
    
    if (levenshtein_search) {
        int resulting_length;
        for (int64_t i = 0; i < reference.size(); ++i) {
            int score = MatchScore(reference.substr(i), query, max_error_count_, resulting_length);
            if (score != std::numeric_limits<int>::min()) {
                // Score is within the allowed limit
                return {i, resulting_length};
            }
        }
    } else {
        int mismatchCount = 0;
        const int64_t queryLength = query.size();
        const int64_t end = static_cast<int64_t>(reference.size()) - queryLength;
        
        for (int64_t start = 0; start < end; ++start) {
            for (int64_t i = 0; (i < queryLength) && (mismatchCount <= max_error_count_); ++i) {
                if (reference[i + start] != query[i])
                    mismatchCount++;
            }
            
            if (mismatchCount <= max_error_count_)
                return {start, queryLength};
            
            mismatchCount = 0;
        }
    }
    return {std::string::npos, 0LL};
}

std::pair<int64_t, int64_t> FuzzySearch::RightmostPosition_(const std::string &query,
                                                            bool levenshtein_search) const
{
    if (max_error_count_ == 0)
        return {reference.rfind(query), query.size()};
    
    if (levenshtein_search) {
        int resulting_length;
        int64_t start = static_cast<int64_t>(reference.size()) - (int64_t)query.length() - 1 - max_error_count_;
        for (; start >= 0; start--) {
            int score = MatchScore(reference.substr(start, query.length() + max_error_count_), query, max_error_count_, resulting_length);
            if (score != std::numeric_limits<int>::min()) {
                // Score is within the allowed limit
                return {start, resulting_length};
            }
        }
    } else {
        int mismatchCount = 0;
        const int64_t query_length = query.size();
        int64_t start = static_cast<int64_t>(reference.size()) - query_length - 1;
        for (; start >= 0; start--) {
            for (int64_t i = 0; (i < query_length) && (mismatchCount <= max_error_count_); ++i) {
                if (reference[start + i] != query[i])
                    mismatchCount++;
            }
            
            if (mismatchCount <= max_error_count_)
                return {start, query_length};
            
            mismatchCount = 0;
        }
    }
    return {std::string::npos, 0LL};
}

}  // namespace gene
