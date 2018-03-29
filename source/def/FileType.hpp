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

#ifndef LIBGENE_DEF_FILETYPE_HPP_
#define LIBGENE_DEF_FILETYPE_HPP_

#ifdef __cplusplus
enum class FileType {
    Fasta,
    Fastq,
    Csv,
    Tsv,
    Sam,
    Bam,
    Bed,
    PlainTxt,
    GenBank,
    Unknown
};

enum class FastqVariant {
    Illumina1_8,
    Illumina1_5,
    Illumina1_3,
    Solexa,
    Sanger
};

#else  // __cplusplus

// For 'legacy' classes which must be eventually removed.
// If not for the 'Mutator' all of them would have been gone.
enum FileType {
    Fasta,
    Fastq,
    Csv,
    Tsv,
    Unknown
};

#endif  // __cplusplus

#endif  // LIBGENE_DEF_FILETYPE_HPP_
