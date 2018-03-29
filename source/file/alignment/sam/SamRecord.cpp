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

#include <string>

#include "SamRecord.hpp"

using std::string;

SamRecord::SamRecord(const string& line) {
    auto nextToken = [](int64_t& previousTabPosition, const string& line) {
        int64_t tabPosition = line.find('\t', previousTabPosition + 1);
        int64_t temp = previousTabPosition;
        previousTabPosition = tabPosition;
        if (temp == 0)
            temp = -1;
        return line.substr(temp + 1, tabPosition - temp - 1);
    };

    int64_t tabPosition = 0;

    QNAME = nextToken(tabPosition, line);
    if (tabPosition == string::npos)
        return;

    auto token = nextToken(tabPosition, line);
    FLAG = std::stoi(token);
    RNAME = nextToken(tabPosition, line);
    POS = std::stoi(nextToken(tabPosition, line));
    token = nextToken(tabPosition, line);
    MAPQ = std::stoi(token);
    CIGAR = nextToken(tabPosition, line);
    RNEXT = nextToken(tabPosition, line);
    token = nextToken(tabPosition, line);
    PNEXT = std::stoi(token);
    token = nextToken(tabPosition, line);
    TLEN = std::stoi(token);
    SEQ = nextToken(tabPosition, line);
    QUAL = nextToken(tabPosition, line);
}

