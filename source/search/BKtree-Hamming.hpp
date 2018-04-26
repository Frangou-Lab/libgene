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

#ifndef BKtree_hpp
#define BKtree_hpp

#include <string>
#include <vector>
#include <deque>
#include <string_view>
#include <algorithm>
#include <map>
#include <cassert>

#include "../def/Def.hpp"
#include "../utils/MiscPrimitives.hpp"

struct BarcodeRecord;

class BKTree {
 public:
    BKTree(int max_distance);
    bool Add(const BarcodeRecord& word);
    void Sort();
    std::pair<int, int> Search(std::string_view word) const;

    const int max_distance;

 private:
    struct Node {
        struct ChildPair {
            int32_t distance;
            int32_t location;
        };

        const BarcodeRecord& barcode_ref;
        std::vector<ChildPair> children;

        explicit Node(const BarcodeRecord& barcode_ref);

        int ChildIndexForDistance(int dist) const noexcept;
        bool ContainsKey(int dist) const noexcept;
        void AddChild(int dist, int array_index);
        void SortLayer();
    };

    std::vector<Node> tree_;
};

class BKTree2 {
public:
    BKTree2(int max_distance);
    bool Add(const BarcodeRecord& word);
    void Sort();
    std::pair<int, int> Search(std::string_view word) const;

    const int max_distance;

private:
    struct Node {
        struct ChildPair {
            int32_t distance;
            int32_t location;
        };

        const BarcodeRecord& barcode_ref;
        std::vector<ChildPair> children;

        explicit Node(const BarcodeRecord& barcode_ref);

        int ChildIndexForDistance(int dist) const noexcept;
        bool ContainsKey(int dist) const noexcept;
        void AddChild(int dist, int array_index);
        void SortLayer();
    };

    std::vector<Node> tree_;
};

int HammingDistance(std::string_view first, std::string_view second) noexcept;

struct BarcodeRecord {
    BarcodeRecord() = default;
    BarcodeRecord(std::vector<std::string>&& components);

    std::string id;             // #0
    std::string description;    // #1
    std::string barcode;        // #2

    /* Optional. May be empty */
    std::string barcode2;       // #3

    bool Empty() const noexcept
    {
        return barcode.empty();
    }
    int original_index{-1};
};

#endif  // BKtree_hpp
