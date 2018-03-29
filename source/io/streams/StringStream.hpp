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

#ifndef StringStream_hpp
#define StringStream_hpp

#include <string>

class StringStream {
protected:
    FILE *file_{nullptr};
    char buf_[BUFSIZ];
    int64_t pos_{0};
    int64_t read_{0};
    int64_t length_{0};
    std::string path_;

 public:
    explicit StringStream(const std::string& file_path);
    virtual ~StringStream();

    virtual std::string name() const;
    virtual std::string path() const;
    virtual int64_t position() const noexcept;
    virtual int64_t length() const noexcept;
};

#endif  // StringStream_hpp
