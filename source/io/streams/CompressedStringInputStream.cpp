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

#include <algorithm>
#include <string>
#include <stdexcept>
#include <zlib.h>

#include "CompressedStringInputStream.hpp"
#include "../../utils/MiscPrimitives.hpp"

CompressedStringInputStream::CompressedStringInputStream(const std::string& file_path)
: StringInputStream(file_path)
{
    file_ = fopen(path_.c_str(), "rt");
    if (!file_)
        throw prim::UserVisibleError("Couldn't open input compressed file");

    fseek(file_, 0L, SEEK_END);
    length_ = ftell(file_);
    fseek(file_, 0L, SEEK_SET);
    fclose(file_);
    file_ = nullptr;

    gzfile_ = gzopen(path_.c_str(), "rb");
    if (!gzfile_)
        throw prim::UserVisibleError("Couldn't open compressed file");

    // Increase the default buffer size from 8192 bytes to 32K
    gzbuffer(gzfile_, 32768);
}

std::string CompressedStringInputStream::ReadLine()
{
    std::string ret_str;
    ret_str.reserve(100);
    do {
        if (pos_ < read_) {  // Buffer is not exhausted
            char *record_start = buf_ + pos_;
            char *p1 = (char *)memchr((void *)record_start, '\r', read_ - pos_);
            char *p2 = (char *)memchr((void *)record_start, '\n', read_ - pos_);
            if (p1 || p2) {
                char *p;
                if (p1 && p2)
                    p = std::min(p1, p2);
                else if (p1)
                    p = p1;
                else
                    p = p2;

                int64_t length = std::distance(record_start, p);
                if (length == 0 && ret_str.empty()) {
                    // This check was added to cope with problem arising with \r\n split over
                    // several fread's (we would skip \r, but leave \n on the next read
                    // resulting in empty line).
                    // Genuinely empty lines (with only newline char) will be skipped,
                    // which I think is a reasonable behaviour.
                    while (pos_ < read_ && (buf_[pos_] == '\r' || buf_[pos_] == '\n'))
                        pos_++;
                    continue;
                }
                std::string ret(record_start, length);
                pos_ += length;

                // Skip newline characters (if exist)
                while (pos_ < read_ && (buf_[pos_] == '\r' || buf_[pos_] == '\n'))
                    pos_++;

                if (ret_str.empty())
                    ret_str = std::move(ret);
                else
                    ret_str.append(ret);

                return ret_str;
            } else {
                ret_str.append(buf_ + pos_, read_ - pos_);
            }
        }
        // Read more data
        pos_ = 0;
        read_ = gzread(gzfile_, buf_, BUFSIZ);
    } while (read_);
    return ret_str;
}

int CompressedStringInputStream::Peek()
{
    if (pos_ < read_) {
        // Have something in buffer
        return buf_[pos_];
    } else {
        int c = gzgetc(gzfile_);
        gzungetc(c, gzfile_);
        return c;
    }
}

void CompressedStringInputStream::ResetFilePointer()
{
    gzseek(gzfile_, 0L, SEEK_SET);
    pos_ = 0;
    read_ = 0;
    memset(buf_, 0, BUFSIZ);
}

int64_t CompressedStringInputStream::position() const noexcept
{
    if (gzfile_)
        return gzoffset(gzfile_);
    else
        return -1;
}

