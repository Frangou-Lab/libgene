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

#ifndef CompressedStringInputStream_hpp
#define CompressedStringInputStream_hpp

#include <string>
#include <zlib.h>

#include "StringInputStream.hpp"

namespace gene {

class CompressedStringInputStream final : public StringInputStream {    
 public:
    explicit CompressedStringInputStream(const std::string& file_path);
    ~CompressedStringInputStream() = default;

    std::string ReadLine() override;
    int Peek() override;
    void ResetFilePointer() override;

    int64_t position() const noexcept override;

 private:
    gzFile gzfile_;
};

}  // namespace gene

#endif  // CompressedStringInputStream_hpp
