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

#include "StringStream.hpp"
#include "../../utils/StringUtils.hpp"

#include <cstdio>

StringStream::StringStream(const std::string& file_path)
: path_(file_path)
{
}

StringStream::~StringStream()
{
    if (file_) {
        fclose(file_);
        file_ = nullptr;
    }
}

int64_t StringStream::length() const noexcept
{
    return length_;
}

int64_t StringStream::position() const noexcept
{
    return ftell(file_) - read_ + pos_;
}

std::string StringStream::name() const
{
    return utils::GetLastPathComponent(path_);
}

std::string StringStream::path() const
{
    return path_;
}

