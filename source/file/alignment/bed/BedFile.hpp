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

#ifndef BedFile_hpp
#define BedFile_hpp

#include <string>
#include <vector>
#include <memory>

#include "../../../io/streams/StringInputStream.hpp"
#include "../AlignmentFile.hpp"
#include "../sam/SamHeader.hpp"

namespace gene {

class AlignmentRecord;

class BedFile : public AlignmentFile {
 private:
    void readHeader();
    void writeHeader();
    
 public:
    BedFile(const std::string& path,
            const std::unique_ptr<CommandLineFlags>& flags,
            OpenMode mode);
    
    virtual std::string strFileType() const override;
    virtual bool isValidAlignmentFile() const override;
    
    virtual SamRecord read() override;
    virtual void write(const SamRecord& record) override;
    
    static std::string defaultExtension();
    static std::vector<std::string> extensions();
    
    virtual int64_t position() const override;
    virtual int64_t length() const override;
};

}  // namespace gene

#endif /* BedFile_hpp */
