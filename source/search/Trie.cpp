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

#include "Trie.hpp"

#include <stack>

Trie::Trie()
{
    root = new TrieNode();
}

bool Trie::Add(const std::string& s)
{
    TrieNode *iter = root;
    bool branched_at_least_once = false;

    for (int i = 0; i < s.size(); ++i) {
        int index = char2index_(s[i]);
        if (iter->children[index] == nullptr) {
            // No such node, need to create one
            iter->children[index] = new TrieNode();
            branched_at_least_once = true;
        }
        iter = iter->children[index];
    }

    if (branched_at_least_once) {
        // The last node is different, it contains an index of the original sequence
        iter->index = new int(count_++);
        return true;
    }
    return false;
}

Trie::~Trie()
{
    std::stack<TrieNode *> to_delete({root});

    while (!to_delete.empty()) {
        TrieNode *node = to_delete.top();

        to_delete.pop();
        for (int i = 0; i < 4; ++i) {
            if (node->children[i])
                to_delete.push(node->children[i]);
        }
        delete node;
    }
}
