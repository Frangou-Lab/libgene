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

#ifndef LIBGENE_SEARCH_TRIE_HPP
#define LIBGENE_SEARCH_TRIE_HPP

#include <string>
#include <string_view>
#include <stack>
#include <cassert>

namespace gene {

constexpr int char2index_(char c) noexcept;

class Trie {
 private:
    struct TrieNode {
        TrieNode() = default;
        ~TrieNode()
        {
            delete index;
        }

        TrieNode *children[4] = {nullptr /* 'A' */,
                                 nullptr /* 'T' */,
                                 nullptr /* 'G' */,
                                 nullptr /* 'C' */};

        // Defined only for the leaf node. This is an index from the original
        // barcode vector to determine which barcode we found.
        int *index{nullptr};
    };

    int count_{0};
    TrieNode *root;

 public:
    Trie();
    ~Trie();
    bool Add(const std::string& s);

    constexpr
    std::pair<int, int> Search(std::string_view query) const noexcept
    {
        const TrieNode *iter = root;

        for (int i = 0; i < query.length(); ++i) {
            int index = char2index_(query[i]);
            if (index == -1 || iter == nullptr)
                return {-1, 0};

            iter = iter->children[index];
        }
        return std::make_pair(iter ? *(iter->index) : -1, 0);
    }

    const int max_distance{0};
};

constexpr int char2index_(char c) noexcept
{
    switch (c) {
        case 'A':
            return 0;
        case 'T':
            return 1;
        case 'G':
            return 2;
        case 'C':
            return 3;
        default:
            return -1;
    }
}

}  // namespace gene

#endif  // LIBGENE_SEARCH_TRIE_HPP
