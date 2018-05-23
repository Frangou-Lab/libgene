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

#include "SamHeaderPG.hpp"

namespace gene {

SamHeaderPG::SamHeaderPG(const std::string& line)
{
    // Program version.
    std::string VN;

    int64_t tag_position = std::string::npos;
    if ((tag_position = line.find("ID")) != std::string::npos) {
        ID = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("PN")) != std::string::npos) {
        PN = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("CL")) != std::string::npos) {
        CL = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("PP")) != std::string::npos) {
        PP = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("DS")) != std::string::npos) {
        DS = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("VN")) != std::string::npos) {
        VN = ExtractNextTagValue_(line, &tag_position);
    }
}

std::string SamHeaderPG::report() const
{
    std::string str = "PG:\n";
    if (!ID.empty())
        str += "\tID: " + ID + '\n';
    if (!PN.empty())
        str += "\tPN: " + PN + '\n';
    if (!CL.empty())
        str += "\tCL: " + CL + '\n';
    if (!PP.empty())
        str += "\tPP: " + PP + '\n';
    if (!DS.empty())
        str += "\tDS: " + DS + '\n';
    if (!VN.empty())
        str += "\tVN: " + VN + '\n';

    return str;
}

}  // namespace gene
