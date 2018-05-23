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

#include "BKtree-Hamming.hpp"

#include <iostream>

namespace gene {

BKTree::Node::Node(const BarcodeRecord& barcode) : barcode_ref{barcode}
{
}

int BKTree::Node::ChildIndexForDistance(int dist) const noexcept
{
    for (const auto& [distance, index] : children)
        if (distance == dist)
            return index;
    return -1;
}

bool BKTree::Node::ContainsKey(int dist) const noexcept
{
    return ChildIndexForDistance(dist) != -1;
}

void BKTree::Node::AddChild(int dist, int array_index)
{
    children.push_back({dist, array_index});
}

void BKTree::Node::SortLayer()
{
    std::sort(children.begin(), children.end(),
              [](const auto& c1, const auto& c2) {
                  return c1.distance < c2.distance;
              });
}

BKTree::BKTree(int max_distance)
: max_distance{max_distance}
{
}

bool BKTree::Add(const BarcodeRecord& record)
{
    if (tree_.empty()) {
        tree_.emplace_back(Node{record});
        return true;
    }

    int current_index = 0;
    int dist = HammingDistance(tree_[current_index].barcode_ref.barcode,
                               record.barcode);

    while (tree_[current_index].ContainsKey(dist)) {
        if (dist == 0) {
            // This occurs if the word has already been inserted into
            // the tree.
            return false;
        }

        current_index = tree_[current_index].ChildIndexForDistance(dist);
        dist = HammingDistance(tree_[current_index].barcode_ref.barcode,
                               record.barcode);
    }
    int new_index = static_cast<int>(tree_.size());
    tree_.emplace_back(Node{record});
    tree_[current_index].AddChild(dist, new_index);
    return true;
}

void BKTree::Sort()
{
    std::vector<int32_t> index_stack = {0};

    while (!index_stack.empty()) {
        const int node_index = index_stack.back();
        auto& tree_node = tree_[node_index];
        index_stack.pop_back();

        tree_node.SortLayer();
        for (const auto& [distance, index] : tree_node.children)
            index_stack.push_back(index);
    }
}

std::pair<int, int> BKTree::Search(std::string_view word) const
{
    std::vector<int32_t> index_stack = {0};
    std::pair<int, int> result(-1, 0);

    while (!index_stack.empty()) {
        const auto index = index_stack.back();
        const auto& tree_node = tree_[index];
        index_stack.pop_back();

        const int node_distance = HammingDistance(tree_node.barcode_ref.barcode,
                                                  word);
        if (node_distance <= max_distance) {
            // That's enough, we found what we were looking for
            result.first = tree_node.barcode_ref.original_index;
            result.second = node_distance;
            break;
        }

        int min_dist = node_distance - max_distance;
        int max_dist = node_distance + max_distance;
        for (const auto& [distance, index] : tree_node.children)
            if (distance >= min_dist && distance <= max_dist)
                index_stack.push_back(index);
    }
    return result;
}

// BKTree2

BKTree2::Node::Node(const BarcodeRecord& barcode) : barcode_ref{barcode}
{
}

int BKTree2::Node::ChildIndexForDistance(int dist) const noexcept
{
    for (const auto& [distance, index] : children)
        if (distance == dist)
            return index;
    return -1;
}

bool BKTree2::Node::ContainsKey(int dist) const noexcept
{
    return ChildIndexForDistance(dist) != -1;
}

void BKTree2::Node::AddChild(int dist, int array_index)
{
    children.push_back({dist, array_index});
}

void BKTree2::Node::SortLayer()
{
    std::sort(children.begin(), children.end(),
              [](const auto& c1, const auto& c2) {
                  return c1.distance < c2.distance;
              });
}

BKTree2::BKTree2(int max_distance)
: max_distance{max_distance}
{
}

bool BKTree2::Add(const BarcodeRecord& record)
{
    if (tree_.empty()) {
        tree_.emplace_back(Node{record});
        return true;
    }

    int current_index = 0;
    int dist = HammingDistance(tree_[current_index].barcode_ref.barcode2,
                               record.barcode2);

    while (tree_[current_index].ContainsKey(dist)) {
        if (dist == 0) {
            // This occurs if the word has already been inserted into
            // the tree.
            return false;
        }

        current_index = tree_[current_index].ChildIndexForDistance(dist);
        dist = HammingDistance(tree_[current_index].barcode_ref.barcode2,
                               record.barcode2);
    }
    int new_index = static_cast<int>(tree_.size());
    tree_.emplace_back(Node{record});
    tree_[current_index].AddChild(dist, new_index);
    return true;
}

void BKTree2::Sort()
{
    std::vector<int32_t> index_stack = {0};

    while (!index_stack.empty()) {
        const int node_index = index_stack.back();
        auto& tree_node = tree_[node_index];
        index_stack.pop_back();

        tree_node.SortLayer();
        for (const auto& [distance, index] : tree_node.children)
            index_stack.push_back(index);
    }
}

std::pair<int, int> BKTree2::Search(std::string_view word) const
{
    std::vector<int32_t> index_stack = {0};
    std::pair<int, int> result(-1, 0);

    while (!index_stack.empty()) {
        const auto index = index_stack.back();
        const auto& tree_node = tree_[index];
        index_stack.pop_back();

        const int node_distance = HammingDistance(tree_node.barcode_ref.barcode2,
                                                  word);
        if (node_distance <= max_distance) {
            // That's enough, we found what we were looking for
            result.first = tree_node.barcode_ref.original_index;
            result.second = node_distance;
            break;
        }

        int min_dist = node_distance - max_distance;
        int max_dist = node_distance + max_distance;
        for (const auto& [distance, index] : tree_node.children)
            if (distance >= min_dist && distance <= max_dist)
                index_stack.push_back(index);
    }
    return result;
}

// -- BKTree2

int HammingDistance(std::string_view first, std::string_view second) noexcept
{
    int d = 0;
    for (int i = 0; i < first.length(); ++i) {
        if (first[i] != second[i])
            d++;
    }
    return d;
}


BarcodeRecord::BarcodeRecord(std::vector<std::string>&& components)
{
    for (int i = 0; i < components.size(); ++i) {
        auto&& current_str = std::move(components[i]);
        if      (i == 0) id          = current_str;
        else if (i == 1) description = current_str;
        else if (i == 2) barcode     = current_str;
        else if (i == 3) barcode2    = current_str;
    }
}

}  // namespace gene
