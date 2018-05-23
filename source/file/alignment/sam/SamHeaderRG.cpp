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

#include "SamHeaderRG.hpp"

namespace gene {

SamHeaderRG::SamHeaderRG(const std::string& line)
{
    int64_t tag_position = std::string::npos;
    if ((tag_position = line.find("ID")) != std::string::npos) {
        ID = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("CN")) != std::string::npos) {
        CN = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("DS")) != std::string::npos) {
        DS = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("DT")) != std::string::npos) {
        DT = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("FO")) != std::string::npos) {
        FO = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("KS")) != std::string::npos) {
        KS = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("LB")) != std::string::npos) {
        LB = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("PG")) != std::string::npos) {
        PG = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("PI")) != std::string::npos) {
        PI = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("PL")) != std::string::npos) {
        PL = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("PM")) != std::string::npos) {
        PM = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("PU")) != std::string::npos) {
        PU = ExtractNextTagValue_(line, &tag_position);
    }
    if ((tag_position = line.find("SM")) != std::string::npos) {
        SM = ExtractNextTagValue_(line, &tag_position);
    }
}

std::string SamHeaderRG::report() const
{
    std::string str = "RG:\n";
    if (!ID.empty())
        str += "\tID: " + ID + '\n';
    if (!CN.empty())
        str += "\tCn: " + CN + '\n';
    if (!DS.empty())
        str += "\tDS: " + DS + '\n';
    if (!DT.empty())
        str += "\tDT: " + DT + '\n';
    if (!FO.empty())
        str += "\tFO: " + FO + '\n';
    if (!KS.empty())
        str += "\tKS: " + KS + '\n';
    if (!LB.empty())
        str += "\tLB: " + PG + '\n';
    if (!PI.empty())
        str += "\tPI: " + PI + '\n';
    if (!PL.empty())
        str += "\tPL: " + PL + '\n';
    if (!PM.empty())
        str += "\tPM: " + PM + '\n';
    if (!PU.empty())
        str += "\tPU: " + PU + '\n';
    if (!SM.empty())
        str += "\tSM: " + SM + '\n';

    return str;
}

}  // namespace gene
