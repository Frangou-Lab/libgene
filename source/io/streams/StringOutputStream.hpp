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

#ifndef LIBGENE_IO_STREAMS_STRING_OUTPUTSTREAM_HPP_
#define LIBGENE_IO_STREAMS_STRING_OUTPUTSTREAM_HPP_

#include <memory>

#include "StringStream.hpp"

class StringOutputStream : public StringStream {
 public:
    StringOutputStream(const std::string& fileName);
    static std::unique_ptr<StringOutputStream> StreamWithFileName(const std::string& fileName);
    
    void Write(const std::string& str);
    void WriteLine(const std::string& str);
    void WriteLine();
    void WriteQuoted(const std::string& str);
    void Write(char c);
};

#endif  // LIBGENE_IO_STREAMS_STRING_OUTPUTSTREAM_HPP_
