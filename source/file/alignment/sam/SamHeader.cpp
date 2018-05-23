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

#include <vector>
#include <string>
#include <memory>

#include "SamHeader.hpp"
#include "../../../log/Logger.hpp"

namespace gene {

SamHeader::SamHeader()
{
}

SamHeader::SamHeader(const std::vector<std::string>& headerLines) {
    for (const auto& line : headerLines) {
        if (line.size() < 3) {
            PrintfLog("Malformed header line detected. Skipping this line");
            continue;
        }

        auto tag = line.substr(1, 2);
        if (tag == "HD")
            hd.push_back(std::make_unique<SamHeaderHD>(line));
        else if (tag == "SQ")
            sq.push_back(std::make_unique<SamHeaderSQ>(line));
        else if (tag == "RG")
            rg.push_back(std::make_unique<SamHeaderRG>(line));
        else if (tag == "PG")
            pg.push_back(std::make_unique<SamHeaderPG>(line));
        else if (tag == "CO")
            comment += line.substr(3);
        else
            PrintfLog("ERROR: Unknown tag: @%s\n", tag.c_str());
    }
}

}  // namespace gene
