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

#include <iostream>

#include "SamTag.hpp"
#include "../bam/BamUtils.hpp"

namespace gene {

int SamTag::readTag(const uint8_t* start, int32_t tag_length)
{
    // int read_bytes_so_far = 0;
    // char tag[2];
    if (tag_length > 0) {
    //    std::memcpy(tag, start, 2);
    //    std::cout << "\nTAG: " << tag << ':' <<
    //                 (start + 2)[0] << ':' << start+3 << '\n';
    }
    // if (tag[0] != 'A' &&
    //    tag[0] != 'B' &&
    //    tag[0] != 'C' &&
    //    tag[0] != 'E')
    {
        return tag_length;
    }

//    start += sizeof(tag);
//    read_bytes_so_far += sizeof(tag);
//
//    char val_type = *start;
//    start += sizeof(val_type);
//    read_bytes_so_far += sizeof(val_type);
//
//    val.A.contents = malloc(tag_length - read_bytes_so_far);
//    std::memcpy(val.A.contents, start, tag_length - read_bytes_so_far);
//
//    switch (val_type)
//    {
//        case 'A':
//        case 'Z':
//            // String following
//        case 'B':
//            val.A.metadata = bmtls::getUint64(start);
//            start += sizeof(val.A.metadata);
//            read_bytes_so_far += sizeof(val.A.metadata);
//            break;
//        case 'c':
//        case 'C':
//            val.C = bmtls::getUint8(start);
//            start += sizeof(val.c);
//            read_bytes_so_far += sizeof(val.c);
//            break;
//        case 's':
//        case 'S':
//            val.S = bmtls::getUint16(start);
//            start += sizeof(val.S);
//            read_bytes_so_far += sizeof(val.S);
//            break;
//        case 'i':
//        case 'I':
//            val.I = bmtls::getUint16(start);
//            start += sizeof(val.I);
//            read_bytes_so_far += sizeof(val.I);
//            break;
//        default:
//            // Some unknown tag
//            break;
//    }
//    return read_bytes_so_far;
}

}  // namespace gene
