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

#ifndef GenomicTsvFile_hpp
#define GenomicTsvFile_hpp

#include "GenomicSeparatedFile.hpp"

#include <string>

class GenomicTsvFile : public GenomicSeparatedFile
{
public:
    GenomicTsvFile(const std::string& path, const std::unique_ptr<CommandLineFlags>& flags, OpenMode mode);
    virtual ~GenomicTsvFile() = default;
    
    static std::string defaultExtension();
    static std::string displayExtension();
    static std::vector<std::string> extensions();
};

#endif /* GenomicTsvFile_hpp */