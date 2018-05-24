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

#include "SearchPrimer.hpp"

#include "../utils/StringUtils.hpp"
#include "../utils/CppUtils.hpp"

namespace gene {

constexpr int kNumberOfIdDigits = 7;

SearchPrimer::SearchPrimer(const std::string& seq,
                           bool isRnaTarget, bool basesSubstituted)
: rnaTarget_(isRnaTarget)
{
    forwardSequence_ = seq;
    reverseSequence_.resize(forwardSequence_.size());
    utils::MakeReverseComplement(forwardSequence_.cbegin(), forwardSequence_.cend(),
                                 reverseSequence_.begin(), reverseSequence_.end(), isRnaTarget);
    if (basesSubstituted)
    {
        std::string seqMutable = seq;
        bool rna = false;
        for (char& c: seqMutable)
        {
            if (c == 'T')
            {
                c = 'U';
                rna = true;
            }
            else if (c == 'U')
                c = 'T';
        }
        
        if (isForward_)
        {
            displayName_ = seqMutable;
            displayNameReverse_.resize(displayName_.size());
            utils::MakeReverseComplement(displayName_.cbegin(), displayName_.cend(),
                                         displayNameReverse_.begin(), displayNameReverse_.end(), rna);
        }
        else
        {
            displayNameReverse_ = seqMutable;
            displayName_.resize(displayNameReverse_.size());
            utils::MakeReverseComplement(displayNameReverse_.cbegin(), displayNameReverse_.cend(),
                                         displayName_.begin(), displayName_.end(), rna);
        }
    }
}

SearchPrimer::SearchPrimer(const std::string& seq, int number, bool isRnaTarget,
                           bool basesSubstituted)
: SearchPrimer(seq, isRnaTarget, basesSubstituted)
{
    const auto paddedNumber = utils::PaddedToLengthString(number, kNumberOfIdDigits);
    forwardId_ = "GF_F_" + paddedNumber;
    reverseId_ = "GF_R_" + paddedNumber;
}

SearchPrimer::SearchPrimer(const std::string& seq, int number, bool isRnaTarget,
                           bool isForward, bool basesSubstituted)
: isForward_(isForward), rnaTarget_(isRnaTarget)
{
    const auto paddedNumber = utils::PaddedToLengthString(number, kNumberOfIdDigits);
    if (isForward)
    {
        forwardSequence_ = seq;
        reverseSequence_.resize(forwardSequence_.size());
        utils::MakeReverseComplement(forwardSequence_.cbegin(), forwardSequence_.cend(),
                                     reverseSequence_.begin(), reverseSequence_.end(), isRnaTarget);
    }
    else
    {
        reverseSequence_ = seq;
        forwardSequence_.resize(reverseSequence_.size());
        utils::MakeReverseComplement(reverseSequence_.cbegin(), reverseSequence_.cend(),
                                     forwardSequence_.begin(), forwardSequence_.end(), isRnaTarget);
    }
    reverseId_ =  "GF_R_" + paddedNumber;
    forwardId_ = "GF_F_" + paddedNumber;
    if (basesSubstituted)
    {
        std::string seqMutable = seq;
        bool rna = false;
        for (char& c: seqMutable)
        {
            if (c == 'T')
            {
                c = 'U';
                rna = true;
            }
            else if (c == 'U')
                c = 'T';
        }
        
        if (isForward_)
        {
            displayName_ = seqMutable;
            displayNameReverse_.resize(displayName_.size());
            utils::MakeReverseComplement(displayName_.cbegin(), displayName_.cend(),
                                         displayNameReverse_.begin(), displayNameReverse_.end(), rna);
        }
        else
        {
            displayNameReverse_ = seqMutable;
            displayName_.resize(displayNameReverse_.size());
            utils::MakeReverseComplement(displayNameReverse_.cbegin(), displayNameReverse_.cend(),
                                         displayName_.begin(), displayName_.end(), rna);
        }

    }
}

SearchPrimer::SearchPrimer(const std::string& id, const std::string& seq, bool isRnaTarget,
                           bool basesSubstituted)
: rnaTarget_(isRnaTarget)
{
    isForward_ = (id.find("_R") == std::string::npos);
    if (isForward_)
    {
        forwardSequence_ = seq;
        forwardId_ = id;
        reverseSequence_.resize(seq.size());
        utils::MakeReverseComplement(forwardSequence_.cbegin(), forwardSequence_.cend(),
                                     reverseSequence_.begin(), reverseSequence_.end(), isRnaTarget);
        reverseId_ = id;
        int64_t fIdentifier = id.find("_F");
        if (fIdentifier != std::string::npos)
            reverseId_[fIdentifier + 1] = 'R';
        else
            reverseId_ += "_R";
    }
    else
    {
        reverseSequence_ = seq;
        reverseId_ = id;
        forwardSequence_.resize(reverseSequence_.size());
        utils::MakeReverseComplement(reverseSequence_.cbegin(), reverseSequence_.cend(),
                                     forwardSequence_.begin(), forwardSequence_.end(), isRnaTarget);
        forwardId_ = id;
        forwardId_[id.find("_R") + 1] = 'F';
    }
    
    if (basesSubstituted)
    {
        std::string seqMutable = seq;
        bool rna = false;
        for (char& c: seqMutable)
        {
            if (c == 'T')
            {
                c = 'U';
                rna = true;
            }
            else if (c == 'U')
                c = 'T';
        }
        
        if (isForward_)
        {
            displayName_ = seqMutable;
            displayNameReverse_.resize(displayName_.size());
            utils::MakeReverseComplement(displayName_.cbegin(), displayName_.cend(),
                                         displayNameReverse_.begin(), displayNameReverse_.end(), rna);
        }
        else
        {
            displayNameReverse_ = seqMutable;
            displayName_.resize(displayNameReverse_.size());
            utils::MakeReverseComplement(displayNameReverse_.cbegin(), displayNameReverse_.cend(),
                                         displayName_.begin(), displayName_.end(), rna);
        }
    }
}

SearchPrimer::SearchPrimer(const std::string& id, const std::string& seq,
                           bool isRnaTarget, bool isForward, bool basesSubstituted)
: SearchPrimer(id, seq, isRnaTarget, basesSubstituted)
{
    bool strainAlreadyExplicitlyStatedInId = (forwardId_.find("_F") != std::string::npos);
    if (!isForward_)
        strainAlreadyExplicitlyStatedInId &= (reverseId_.find("_R") != std::string::npos);
    
    if (strainAlreadyExplicitlyStatedInId)
        return;
    
    if (isForward != isForward_)
    {
        ToggleStrain();
        isForward_ = isForward;
    }
}

bool SearchPrimer::operator==(const SearchPrimer& other) const
{
    if (forwardSequence_ == other.forwardSequence_)
        return true;
    if (forwardSequence_ == other.reverseSequence_)
        return true;
    return false;
}

bool SearchPrimer::operator!=(const SearchPrimer& other) const
{
    return !(this->operator==(other));
}

const std::string& SearchPrimer::ForwardSeq() const
{
    return forwardSequence_;
}

const std::string& SearchPrimer::ReverseSeq() const
{
    return reverseSequence_;
}

const std::string& SearchPrimer::ForwardId() const
{
    return forwardId_;
}

const std::string& SearchPrimer::ReverseId() const
{
    return reverseId_;
}

size_t SearchPrimer::PrimerLength() const
{
    return forwardSequence_.size();
}

const std::string& SearchPrimer::Complement()
{
    if (complement_.empty())
    {
        auto complement = [this](char c) {
            switch (c)
            {
                case 'A':
                    return (rnaTarget_ ? 'U' : 'T');
                case 'U':
                    return 'D';
                case 'G':
                    return 'C';
                case 'C':
                    return 'G';
                case 'T':
                    return 'A';
                default:
                    return 'N';
            }
        };
        complement_.resize(forwardSequence_.size());
        std::transform(forwardSequence_.begin(), forwardSequence_.end(),
                       complement_.begin(), complement);
    }
    return complement_;
}

}  // namespace gene
