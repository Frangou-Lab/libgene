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

#ifndef LIBGENE_OPERATIONS_OPERATION_HPP_
#define LIBGENE_OPERATIONS_OPERATION_HPP_

#include <memory>
#include <functional>
#include <cstdint>

#include "../flags/CommandLineFlags.hpp"
#include "../file/sequence/SequenceFile.hpp"
#include "../file/alignment/AlignmentFile.hpp"

class CommandLineFlags;

class Operation {
 protected:
    template <int ThrottleCount = 1024 /* for some operations this value is way too small */>
    bool HasToUpdateProgress_(int64_t count) const
    {
         return (count % ThrottleCount) == 0;
    }

    std::unique_ptr<CommandLineFlags> flags_;

    virtual bool Init_() = 0;

 public:
    typedef std::unique_ptr<SequenceFile> SequenceFilePtr;
    typedef std::unique_ptr<AlignmentFile> AlignmentFilePtr;
    std::function<bool(float)> update_progress_callback;

    Operation(std::unique_ptr<CommandLineFlags>&& flags);
    virtual ~Operation() = default;
    virtual bool Process();
};

#endif  // LIBGENE_OPERATIONS_OPERATION_HPP_