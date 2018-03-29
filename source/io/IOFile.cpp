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

#include "../io/IOFile.hpp"
#include "../flags/CommandLineFlags.hpp"

IOFile::IOFile(const std::string& path,
               FileType type,
               OpenMode mode)
: type_(type), name_(path)
{
    switch (mode) {
        case OpenMode::Read:
            in_file_ = StringInputStream::StreamWithFileName(path);
            break;
        case OpenMode::Write:
            out_file_ = StringOutputStream::StreamWithFileName(path);
            break;
    }
}

IOFile::IOFile(const std::string& path, FileType type)
: type_(type), name_(path)
{
    // This constructor is meant for GenomicSeparatedFile instantiation, which opens its file using
    // its own resources
}

IOFile::~IOFile() noexcept
{
    in_file_ = nullptr;
    out_file_ = nullptr;
}

std::string IOFile::fileName() const
{
    if (out_file_)
        return out_file_->name();
    else
        return in_file_->name();
}

std::string IOFile::filePath() const
{
    if (out_file_)
        return out_file_->path();
    else
        return in_file_->path();
}

FileType IOFile::fileType() const
{
    return type_;
}

int64_t IOFile::length() const
{
    if (in_file_)
        return in_file_->length();
    if (out_file_)
        return out_file_->length();
    return -1LL;
}

int64_t IOFile::position() const
{
    if (in_file_)
        return in_file_->position();
    if (out_file_)
        return out_file_->position();
    return -1LL;
}

void IOFile::ResetFilePointer()
{
    if (in_file_)
        in_file_->ResetFilePointer();
}
