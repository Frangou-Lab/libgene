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

#include "SamHeaderSQ.hpp"

SamHeaderSQ::SamHeaderSQ(const std::string& line)
: LN(255) {
    int64_t tag_position = std::string::npos;
    if ((tag_position = line.find("SN")) != std::string::npos) {
        SN = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("AS")) != std::string::npos) {
        AS = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("M5")) != std::string::npos) {
        M5 = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("SP")) != std::string::npos) {
        SP = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("UR")) != std::string::npos) {
        UR = ExtractNextTagValue_(line, &tag_position);
    }

    // uint8_t field
    if ((tag_position = line.find("LN")) != std::string::npos) {
        std::string value;
        value = ExtractNextTagValue_(line, &tag_position);
        LN = std::stoi(value);
    }
}

std::string SamHeaderSQ::report() const {
    std::string str = "SQ:\n";
    if (!SN.empty())
        str += "\tSN: " + SN + '\n';
    if (!AS.empty())
        str += "\tAS: " + AS + '\n';
    if (!M5.empty())
        str += "\tMD5 hash: " + M5 + '\n';
    if (!SP.empty())
        str += "\tSP: " + SP + '\n';
    if (!UR.empty())
        str += "\tUR: " + UR + '\n';
    if (LN != 255)
        str += "\tLN: " + std::to_string(static_cast<int>(LN)) + '\n';
    return str;
}
