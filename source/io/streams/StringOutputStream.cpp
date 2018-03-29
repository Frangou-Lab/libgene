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

#include "StringOutputStream.hpp"

#include <stdexcept>
#include <cstdio>

#include "../../utils/MiscPrimitives.hpp"

static const char kNewlineSequence[] = "\n";

StringOutputStream::StringOutputStream(const std::string& fileName)
: StringStream(fileName)
{
    file_ = fopen(fileName.c_str(), "wt");
    if (file_ == nullptr)
        throw prim::UserVisibleError("Couldn't open '" + fileName + "' for writing.\n\nCheck folder write permissions.");

    length_ = 0;
}

std::unique_ptr<StringOutputStream>
StringOutputStream::StreamWithFileName(const std::string& fileName)
{
    return std::make_unique<StringOutputStream>(fileName);
}

void StringOutputStream::WriteLine(const std::string& str)
{
    fputs(str.c_str(), file_);
    length_ += str.size();
    WriteLine();
}

void StringOutputStream::WriteLine()
{
    fputs(kNewlineSequence, file_);
    length_ += sizeof(kNewlineSequence) - 1 /* \0 - C-string ending length*/;
}

void StringOutputStream::WriteQuoted(const std::string& str)
{
    fputc('\"', file_);
    fputs(str.c_str(), file_);
    fputc('\"', file_);
    length_ += str.size() + 2;
}

void StringOutputStream::Write(const std::string& str)
{
    fputs(str.c_str(), file_);
    length_ += str.size();
}

void StringOutputStream::Write(char c)
{
    fputc(c, file_);
    ++length_;
}
