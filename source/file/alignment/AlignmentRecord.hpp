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

#ifndef LIBGENE_FILE_ALIGNMENT_ALIGNMENTRECORD_HPP_
#define LIBGENE_FILE_ALIGNMENT_ALIGNMENTRECORD_HPP_

namespace gene {

class AlignmentRecord {
 public:
    AlignmentRecord() = default;
    virtual ~AlignmentRecord() = default;
};

}  // namespace gene

#endif  // LIBGENE_FILE_ALIGNMENT_ALIGNMENTRECORD_HPP_
