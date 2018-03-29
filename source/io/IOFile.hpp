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

#ifndef IOFile_hpp
#define IOFile_hpp

#include <string>
#include <memory>
#include <fstream>

#include "streams/StringInputStream.hpp"
#include "streams/StringOutputStream.hpp"
#include "../def/FileType.hpp"

enum class OpenMode {
    Read,
    Write
};

class IOFile {
 protected:
    std::unique_ptr<StringInputStream> in_file_;
    std::unique_ptr<StringOutputStream> out_file_;
    std::string name_;
    FileType type_;

 public:
    IOFile(const std::string& path, FileType type, OpenMode mode);
    IOFile(const std::string& path, FileType type);

    virtual ~IOFile() noexcept;
    
    virtual std::string fileName() const;
    virtual std::string filePath() const;
    virtual FileType fileType() const;
    
    virtual int64_t length() const;
    virtual int64_t position() const;
    void ResetFilePointer();
};

#endif /* IOFile_hpp */
