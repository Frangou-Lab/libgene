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

#include "SamHeaderHD.hpp"

namespace gene {

SamHeaderHD::SamHeaderHD(const std::string& line)
{
    int64_t tag_position = std::string::npos;
    if ((tag_position = line.find("VN")) != std::string::npos) {
        VN = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("SO")) != std::string::npos) {
        SO = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("GO")) != std::string::npos) {
        GO = ExtractNextTagValue_(line, &tag_position);
    }
}

std::string SamHeaderHD::report() const
{
    std::string str = "HD:\n";
    if (!VN.empty())
        str += "\tVN: " + VN + '\n';
    if (!SO.empty())
        str += "\tSO: " + SO + '\n';
    if (!GO.empty())
        str += "\tVN: " + GO + '\n';

    return str;
}

}  // namespace gene
