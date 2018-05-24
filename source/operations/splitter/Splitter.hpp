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

#ifndef Splitter_h
#define Splitter_h

#include <string>
#include <memory>
#include <functional>

namespace gene {

class CommandLineFlags;
class SequenceFile;

class Splitter final {
 public:
    Splitter(const std::string& input_path,
             const std::string& output_path,
             std::unique_ptr<CommandLineFlags>&& flags);
    bool Process();
    std::function<bool(float)> update_progress_callback;

 private:
    bool Init_();

    std::unique_ptr<SequenceFile> input_file_;
    std::unique_ptr<CommandLineFlags> flags_;

    std::string outFileName;
    int recordLimit;
    int64_t fileLimit;
    int64_t sizeLimit;
    std::string inputFilePath;
    std::string outputFilePath;
};

}  // namespace gene

#endif /* Splitter_h */
