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

#include <memory>
#include <cassert>

#include "StringInputStream.hpp"
#include "CompressedStringInputStream.hpp"
#include "PlainStringInputStream.hpp"
#include "../../utils/StringUtils.hpp"

std::unique_ptr<StringInputStream>
StringInputStream::StreamWithFileName(const std::string& file_path)
{
    if (utils::HasExtension(file_path, "gz"))
        return std::make_unique<CompressedStringInputStream>(file_path);
    else
        return std::make_unique<PlainStringInputStream>(file_path);
}

StringInputStream::StringInputStream(const std::string& file_path)
: StringStream(file_path)
{
}

bool StringInputStream::empty() const
{
    // The second condition checks if we've reached the end of file, but still
    // have something in buffer
    return feof(file_) != 0 && pos_ == read_;
}

