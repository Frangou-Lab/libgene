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

#ifndef LIBGENE_OPERATIONS_MERGER_HPP_
#define LIBGENE_OPERATIONS_MERGER_HPP_

#include <vector>
#include <string>
#include <functional>

namespace gene {

class SequenceFile;
class CommandLineFlags;

class Merger final {
 public:
    Merger(std::vector<std::string> input_paths,
           std::string output_path,
           std::unique_ptr<CommandLineFlags>&& flags);
    bool Process();
    std::function<bool(float)> update_progress_callback;

 private:
    std::vector<std::unique_ptr<SequenceFile>> inputFiles;
    std::vector<std::string> inputFilePaths;
    std::unique_ptr<SequenceFile> outFile;
    std::unique_ptr<CommandLineFlags> flags_;
    std::string outputPath;
    int64_t total_size_in_bytes_{0};
    bool Init_();
};

}  // namespace gene

#endif  // LIBGENE_OPERATIONS_MERGER_HPP_
