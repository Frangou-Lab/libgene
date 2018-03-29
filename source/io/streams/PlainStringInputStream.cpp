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

#include "PlainStringInputStream.hpp"
#include "StringStream.hpp"
#include "../../utils/MiscPrimitives.hpp"

PlainStringInputStream::PlainStringInputStream(const std::string& file_path)
: StringInputStream(file_path)
{
    if (file_ = fopen(path_.c_str(), "rt"); file_ == nullptr)
        throw prim::UserVisibleError("Couldn't open input file");

    fseek(file_, 0L, SEEK_END);
    length_ = ftell(file_);
    fseek(file_, 0L, SEEK_SET);
}

int PlainStringInputStream::Peek()
{
    if (pos_ < read_) {
        // Have something in buffer
        return buf_[pos_];
    } else {
        int c = fgetc(file_);
        ungetc(c, file_);
        return c;
    }
}

std::string PlainStringInputStream::ReadLine()
{
    std::string ret_str;
    ret_str.reserve(100);
    do {
        if (pos_ < read_) {  // Have something in buf_
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
            } else
                ret_str.append(buf_ + pos_, read_ - pos_);
        }
        // Read more data
        pos_ = 0;
        read_ = fread(buf_, 1, BUFSIZ, file_);
    } while (read_);
    return ret_str;
}

void PlainStringInputStream::ResetFilePointer()
{
    fseek(file_, 0L, SEEK_SET);
    pos_ = 0;
    read_ = 0;
    memset(buf_, '\0', BUFSIZ);
}

PlainStringInputStream::operator bool() const
{
    return (file_ != nullptr) && (ftell(file_) < length_);
}
