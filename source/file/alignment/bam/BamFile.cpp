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

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>

#include "BamFile.hpp"
#include "BamUtils.hpp"
#include "../sam/SamFile.hpp"
#include "../sam/SamRecord.hpp"

using std::string;

BamFile::BamFile(const std::string& path,
                 const std::unique_ptr<CommandLineFlags>& flags,
                 OpenMode mode)
: AlignmentFile(path, FileType::Bam, flags, mode), BgzfFile(path, mode) {
    switch (mode) {
        case OpenMode::Read:
            ReadNextBlock();
            ReadSamHeader();
            break;
        case OpenMode::Write:
            // Not supported
            assert(false);
            break;
    }
}

void BamFile::ReadSamHeader() {
    const uint8_t* data = current_block->data();
    int totalLengthInBytes = 0;

    char magic[5];
    std::memcpy(magic, data, 4);
    magic[4] = '\0';
    if (std::strcmp(magic, "BAM\x01")) {
        // Corrupted file
        return;
    }
    data += sizeof(magic) - 1;             // 0 (+ 4)
    totalLengthInBytes += sizeof(magic) - 1;

    int32_t l_text = bmtls::getUint32(data);
    data += sizeof(l_text);             // 4 (+ 4)
    totalLengthInBytes += sizeof(l_text);

    char* text = reinterpret_cast<char*>(malloc(l_text + 1));
    std::memcpy(text, data, l_text);
    text[l_text] = '\0';  // Quote: "...not necessarily NUL-terminated"
    data += l_text;       // 4 (+ l_text)
    totalLengthInBytes += l_text;
    free(text);

    int32_t n_ref = bmtls::getUint32(data);
    data += sizeof(n_ref);              // 4 + l_text (+ 4)
    totalLengthInBytes += sizeof(n_ref);

    /* List of reference information (n = n_ref) */
    refs.resize(n_ref);
    for (int i = 0; i < n_ref; ++i) {
        int32_t l_name = bmtls::getUint32(data);
        data += sizeof(l_name);
        totalLengthInBytes += sizeof(l_name);

        char* name = reinterpret_cast<char*>(malloc(l_name));
        std::memcpy(name, data, l_name);
        data += l_name;
        totalLengthInBytes += l_name;
        refs[i].name.assign(name);
        free(name);

        int32_t l_ref = bmtls::getUint32(data);
        data += sizeof(l_ref);
        totalLengthInBytes += sizeof(l_ref);
        refs[i].l_ref = l_ref;
    }
    for (auto& ref : refs) {
        ref.PrintDebug();
    }
    block_offset_ += totalLengthInBytes;
    return;
}

void BamFile::write(const SamRecord& record) {
}

SamRecord BamFile::read() {
    if (current_block_exhausted_) {
        bool good = ReadNextBlock();
        if (!good) {
            return SamRecord();
        }
    }

    SamRecord record;
    const uint8_t* data = current_block->data() + block_offset_;

    int32_t size = current_block->size();
    if (block_offset_ + 4 > size) {
        current_block_exhausted_ = true;
        return read();
    }
    // std::cout << "Block size: " << currentBlock->size() <<
    // " Block offset: " << blockOffset_ << '\n';
    int total_read_bytes = 0;
    int read_bytes_so_far = 0;

    // 0 (+ 4) Length of the remainder of the alignment record
    int32_t block_size = bmtls::getUint32(data);

    if (block_offset_ + block_size + 4 > size) {
        current_block_exhausted_ = true;
        return read();
    }

    data += sizeof(block_size);
    read_bytes_so_far += sizeof(block_size);

    int32_t refId = bmtls::getUint32(data);         // 4 (+ 4)
    data += sizeof(refId);
    read_bytes_so_far += sizeof(refId);
    record.RNAME = refs[refId].name;

    record.POS = bmtls::getUint32(data) + 1;           // 8 (+ 4)
    data += sizeof(record.POS);
    read_bytes_so_far += sizeof(record.POS);

    uint32_t bin_mq_nl = bmtls::getUint32(data);     // 12 (+ 4)
    data += sizeof(bin_mq_nl);
    uint8_t l_read_name = bin_mq_nl & 0x000000FF;
    record.MAPQ = (bin_mq_nl & 0x0000FF00) >> 8;
    // uint16_t bin = (bin_mq_nl & 0xFFFF0000) >> 16;
    read_bytes_so_far += sizeof(bin_mq_nl);

    uint32_t flag_nc = bmtls::getUint32(data);       // 16 (+ 4)
    data += sizeof(flag_nc);
    record.FLAG = flag_nc >> 16;
    uint16_t n_cigar_op = flag_nc & 0xFFFF;
    read_bytes_so_far += sizeof(flag_nc);

    uint32_t l_seq = bmtls::getUint32(data);         // 20 (+ 4)
    data += sizeof(l_seq);
    read_bytes_so_far += sizeof(l_seq);

    int32_t next_refId = bmtls::getUint32(data);    // 24 (+ 4)
    data += sizeof(next_refId);
    read_bytes_so_far += sizeof(next_refId);

    record.PNEXT = bmtls::getUint32(data) + 1;      // 28 (+ 4)
    data += sizeof(record.PNEXT);
    read_bytes_so_far += sizeof(record.PNEXT);

    int32_t tlen = bmtls::getUint32(data);          // 32 (+ 4)
    data += sizeof(tlen);
    read_bytes_so_far += sizeof(tlen);

    // 36 (+ l_read_name)
    std::vector<uint8_t> read_name(l_read_name);
    std::memcpy(read_name.data(), data, l_read_name);
    data += l_read_name;
    read_bytes_so_far += l_read_name;
    record.QNAME.assign(read_name.data(), read_name.data() + l_read_name);

    std::vector<uint32_t> cigar(n_cigar_op);
    std::memcpy(cigar.data(), data, n_cigar_op*sizeof(uint32_t));
    data += n_cigar_op*sizeof(uint32_t);
    read_bytes_so_far += n_cigar_op*sizeof(uint32_t);

    auto intToOp = [](uint8_t byte) -> char {
        // ‘MIDNSHP=X’→‘012345678’
        switch (byte & 0xF) {
            case 0:
                return 'M';
            case 1:
                return 'I';
            case 2:
                return 'D';
            case 3:
                return 'N';
            case 4:
                return 'S';
            case 5:
                return 'H';
            case 6:
                return 'P';
            case 7:
                return '=';
            case 8:
                return 'X';
            default:
                return 0;
                // Impossible operation code
                // cassert(false);
        }
    };

    auto getCigarString = [&record, cigar, n_cigar_op, &intToOp]() {
        for (int i = 0; i < n_cigar_op; ++i) {
            record.CIGAR += std::to_string(cigar[i] >> 4);
            record.CIGAR += intToOp(cigar[i]);
        }
    };
    getCigarString();

    std::vector<uint8_t> seq((l_seq + 1)/2);
    std::memcpy(seq.data(), data, (l_seq + 1)/2);
    data += (l_seq+1)/2;
    read_bytes_so_far += (l_seq+1)/2;

    record.SEQ.resize(l_seq);
    auto nybbleToBase = [](uint8_t byte) -> char {
        // ‘=ACMGRSVTWYHKDBN’→ [0, 15]
        switch (byte & 0x0F) {
            case 0:
                return '=';
            case 1:
                return 'A';
            case 2:
                return 'C';
            case 3:
                return 'M';
            case 4:
                return 'G';
            case 5:
                return 'R';
            case 6:
                return 'S';
            case 7:
                return 'V';
            case 8:
                return 'T';
            case 9:
                return 'W';
            case 10:
                return 'Y';
            case 11:
                return 'H';
            case 12:
                return 'K';
            case 13:
                return 'D';
            case 14:
                return 'B';
            case 15:
                return 'N';
            default:
                return 'N';
        }
    };

    for (int i = 0; i < (l_seq+1)/2; ++i) {
        record.SEQ[i*2] = nybbleToBase(seq[i] >> 4);
        record.SEQ[i*2 + 1] = nybbleToBase(seq[i]);
    }

    std::vector<uint8_t> qual(l_seq + 1);
    std::memcpy(qual.data(), data, l_seq);
    data += l_seq;
    read_bytes_so_far += l_seq;
    if (l_seq > 1 && qual[0] != 0xFF) {
        std::for_each(qual.begin(), qual.end(), [](uint8_t& q){q += 33;});
        record.QUAL.assign(qual.data(), qual.data() + l_seq);
    } else {
        record.QUAL = '*';
    }

    /* Auxillary data (unti the end of alignment block) */
    int32_t tag_length = block_size - read_bytes_so_far + 4;
    if (block_size > read_bytes_so_far) {
        int32_t tag_size = record.tag.readTag(current_block->data() +
                                              block_offset_,
                                              tag_length);
        read_bytes_so_far += tag_size;
        data += tag_size;
    } else {
        data += (block_size - read_bytes_so_far) + 4;
        total_read_bytes += tag_length;
    }
    if (block_offset_ + read_bytes_so_far > size) {
        block_offset_ += block_size;
    }
    block_offset_ += read_bytes_so_far;
    return record;
}

int64_t BamFile::position() const {
    return BgzfFile::position();
}

int64_t BamFile::length() const {
    return BgzfFile::length();
}

bool BamFile::isValidAlignmentFile() const {
    return true;
}

string BamFile::strFileType() const {
    return "bam";
}

string BamFile::defaultExtension() {
    return "bam";
}

std::vector<std::string> BamFile::extensions() {
    return {"bam"};
}
