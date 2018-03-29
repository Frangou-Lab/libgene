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

#ifndef LIBGENE_DEF_EXTENSIONS_HPP_
#define LIBGENE_DEF_EXTENSIONS_HPP_

#include <string>
#include <vector>

class Extensions {
 public:
    // Formats that are displayed in a dropdown menu
    static std::vector<std::string> kFinderDisplayInputFileFormats();

    // Formats that are used as mask for allowed file types as 'Input'
    static std::vector<std::string> kFinderInputFileFormats();

    // Formats that are used as mask for allowed file types as 'Reference'
    static std::vector<std::string> kFinderReferenceFileFormats();

    // Formats that are used as mask for allowed file types as 'Query file'
    // (.txt)
    static std::vector<std::string> kInputQueriesFileFormat();
};

#endif  // LIBGENE_DEF_EXTENSIONS_HPP_
