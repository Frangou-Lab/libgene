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

#include <cassert>
#include <algorithm>
#include <locale>
#include <cctype>

#include "CppUtils.hpp"
#include "StringUtils.hpp"
#include "../file/sequence/FastaFile.hpp"
#include "../file/sequence/FastqFile.hpp"
#include "../file/TxtFile.hpp"
#include "../file/sequence/GenomicCsvFile.hpp"
#include "../file/sequence/GenomicTsvFile.hpp"
#include "../file/sequence/GenBankFile.hpp"
#include "../file/alignment/sam/SamFile.hpp"
#include "../file/alignment/bam/BamFile.hpp"
#include "../file/alignment/bed/BedFile.hpp"
#include "../def/Flags.hpp"
#include "../io/streams/PlainStringInputStream.hpp"

namespace gene::utils {

std::string
ConstructOutputNameWithFile(std::string inputFileName,
                            FileType type,
                            std::string outputFilePath,
                            const std::unique_ptr<CommandLineFlags>& flags,
                            std::string suffix)
{
    if (!outputFilePath.empty()) {
        if (suffix == "-split") {
            std::string pathExtension = GetExtension(outputFilePath);
            return StringByDeletingPathExtension(outputFilePath) + suffix + '.' + pathExtension;
        } else {
            return outputFilePath;
        }
    }
    // Have to construct output, check out file type flag
    if (flags->outputFormat() != FileType::Unknown)
        type = flags->outputFormat();

    return StringByDeletingPathExtension(inputFileName) + suffix + '.' + type2extension(type);
}

enum FileType str2type(const std::string &str)
{
    if (str == "fasta")
        return FileType::Fasta;
    if (str.find("fastq") != std::string::npos || str == "fq")
        return FileType::Fastq;
    if (str.find("csv") != std::string::npos)
        return FileType::Csv;
    if (str.find("tsv") != std::string::npos)
        return FileType::Tsv;
    if (str == "gbk" || str == "gb")
        return FileType::GenBank;
    if (str == "sam")
        return FileType::Sam;
    if (str == "bam")
        return FileType::Bam;
    if (str == "bed")
        return FileType::Bed;
    if (str == "txt")
        return FileType::PlainTxt;

    return FileType::Unknown;
}

std::string type2str(enum FileType type)
{
    switch (type) {
        case FileType::Fasta:
            return "fasta";
        case FileType::Fastq:
            return "fastq";
        case FileType::Csv:
            return "csv";
        case FileType::Tsv:
            return "tsv";
        case FileType::Sam:
            return "sam";
        case FileType::Bam:
            return "bam";
        case FileType::Bed:
            return "bed";
        case FileType::PlainTxt:
            return "txt";
        case FileType::GenBank:
            return "gbk";
        case FileType::Unknown:
            return "";
    }
}

std::string type2extension(enum FileType type)
{
    switch (type) {
        case FileType::Fasta:
            return FastaFile::defaultExtension();
        case FileType::Fastq:
            return FastqFile::defaultExtension();
        case FileType::Csv:
            return GenomicCsvFile::defaultExtension();
        case FileType::Tsv:
            return GenomicTsvFile::defaultExtension();
        case FileType::GenBank:
            return GenBankFile::defaultExtension();
        case FileType::Sam:
            return SamFile::defaultExtension();
        case FileType::Bam:
            return BamFile::defaultExtension();
        case FileType::Bed:
            return BedFile::defaultExtension();
        case FileType::PlainTxt:
            return TxtFile::defaultExtension();
        default:
            assert(false);
    }
}

enum FileType extension2type(const std::string &ext)
{
    if (Contains(FastaFile::extensions(), ext))
        return FileType::Fasta;

    if (Contains(FastqFile::extensions(), ext))
        return FileType::Fastq;

    if (Contains(GenomicCsvFile::extensions(), ext))
        return FileType::Csv;

    if (Contains(GenomicTsvFile::extensions(), ext))
        return FileType::Tsv;

    if (Contains(GenBankFile::extensions(), ext))
        return FileType::GenBank;

    if (Contains(SamFile::extensions(), ext))
        return FileType::Sam;

    if (Contains(BamFile::extensions(), ext))
        return FileType::Bam;

    if (Contains(BedFile::extensions(), ext))
        return FileType::Bed;

    if (Contains(TxtFile::extensions(), ext))
        return FileType::PlainTxt;

    return FileType::Unknown;
}

std::string str2extension(const std::string &str)
{
    return type2extension(str2type(str));
}

std::string extension2str(const std::string &ext)
{
    return type2str(extension2type(ext));
}

//
// Searches for a reverse  string 'what' which is interpreted as a reverse
// complement without having to actually create this reverse complement
//
int64_t FindAsReverseComplement(const std::string& where,
                                const std::string& what)
{
    const int64_t targetLength = where.size();
    const int64_t adapterLength = what.size();

    if (adapterLength > targetLength)
        return std::string::npos;

    int64_t j;
    for (int64_t i = 0; i < targetLength - adapterLength; ++i) {
        for (j = adapterLength - 1;
             j >= 0 && AreComplements(where[i + (j - adapterLength - 1)],
                                      what[j]); --j) {
            ;
        }

        if (j == 0) {
            // Found a complete match starting from 'i'
            return i;
        }
    }
    return std::string::npos;
}

//
// Searches for a reverse string 'what' which is interpreted as a reverse
// complement without having to actually create this reverse complement
// starting from the end of 'where'
//
int64_t RfindAsReverseComplement(const std::string& where, const std::string& what)
{
    const int64_t targetLength = where.size();
    const int64_t adapterLength = what.size();

    if (adapterLength > targetLength)
        return std::string::npos;

    for (int64_t i = targetLength - 1; i > adapterLength; --i) {
        int64_t j;
        for (j = adapterLength - 1; j >= 0; --j) {
            if (!AreComplements(where[i - (j - adapterLength - 1)], what[j]))
                break;
        }

        if (j == 0) {
            // Found a complete match starting from 'i + adapterLength'
            return i + adapterLength;
        }
    }
    return std::string::npos;
}

bool AreComplements(char a, char b)
{
    if (a == 'G')
        return b == 'C';
    if (a == 'C')
        return b == 'G';
    if (a == 'A')
        return b == 'T' || b == 'U';
    if (a == 'T')
        return b == 'A';
    if (a == 'U')
        return b == 'A';

    return false;
}

FastqVariant FormatNameToVariant(const std::string& format)
{
    if (format.find(Flags::kIllumina1_8Suffix) != std::string::npos) {
        return FastqVariant::Illumina1_8;
    } else if (format.find(Flags::kIllumina1_5Suffix) != std::string::npos) {
        return FastqVariant::Illumina1_5;
    } else if (format.find(Flags::kIllumina1_3Suffix) != std::string::npos) {
        return FastqVariant::Illumina1_3;
    } else if (format.find(Flags::kSangerSuffix) != std::string::npos) {
        return FastqVariant::Sanger;
    } else if (format.find(Flags::kSolexaSuffix) != std::string::npos) {
        return FastqVariant::Solexa;
    } else {
        assert(false);
    }
}

std::string FastqVariantToSuffix(FastqVariant variant)
{
    switch (variant) {
        case FastqVariant::Illumina1_8:
            return Flags::kIllumina1_8Suffix;
        case FastqVariant::Illumina1_5:
            return Flags::kIllumina1_5Suffix;
        case FastqVariant::Illumina1_3:
            return Flags::kIllumina1_3Suffix;
        case FastqVariant::Sanger:
            return Flags::kSangerSuffix;
        case FastqVariant::Solexa:
            return Flags::kSolexaSuffix;
    }
}

std::vector<std::string> LoadQueriesFromFile(std::string path)
{
    std::vector<std::string> queries;
    PlainStringInputStream queries_file(path);
    if (queries_file) {
        std::string line;
        while (!(line = queries_file.ReadLine()).empty()) {
            int64_t spacePosition = line.find(' ');
            int64_t nextSpace = line.find(' ', spacePosition + 1);
            if (nextSpace != std::string::npos &&
                nextSpace != (int64_t)line.size() - 1) {
                spacePosition = nextSpace;
            }
            
            if (spacePosition != std::string::npos &&
                spacePosition != 0 &&
                spacePosition != (int64_t)line.size() - 1) {
                line[spacePosition] = '&';
            }
            queries.push_back(line);
        }
    }
    return queries;
}

}
