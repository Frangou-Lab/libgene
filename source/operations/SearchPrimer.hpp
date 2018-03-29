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

#ifndef SearchPrimer_hpp
#define SearchPrimer_hpp

#include <string>

class SearchPrimer
{
public:
    SearchPrimer(const std::string& seq, bool isRnaTarget, bool basesSubstituted);
    SearchPrimer(const std::string& seq, int number,
                 bool isRnaTarget, bool basesSubstituted);
    SearchPrimer(const std::string& seq, int number,
                 bool isRnaTarget, bool isForward, bool basesSubstituted);
    SearchPrimer(const std::string& id, const std::string& seq,
                 bool isRnaTarget, bool basesSubstituted);
    SearchPrimer(const std::string& id, const std::string& seq, bool isRnaTarget,
                 bool isForward, bool basesSubstituted);
    
    const std::string& ForwardSeq() const;
    const std::string& ReverseSeq() const;
    const std::string& ForwardId() const;
    const std::string& ReverseId() const;
    size_t PrimerLength() const;
    
    bool operator==(const SearchPrimer& other) const;
    bool operator!=(const SearchPrimer& other) const;
    
    // Returns wether the orginal primer was forward-stranded
    bool IsForward() const
    {
        return isForward_;
    }

    const std::string& Query() const
    {
        if (displayName_.empty())
            return Sequence();
        else
            return (IsForward() ? displayName_ : displayNameReverse_);
    }

    const std::string& ForwardQuery() const
    {
        if (displayName_.empty())
            return Sequence();
        else
            return displayName_;
    }

    const std::string& ReverseQuery() const
    {
        if (displayName_.empty())
            return Sequence();
        else
            return displayNameReverse_;
    }

    const std::string& Sequence() const
    {
        return (IsForward() ? ForwardSeq() : ReverseSeq());
    }

    const std::string& Id() const
    {
        return (IsForward() ? ForwardId() : ReverseId());
    }
    
    void ToggleStrain()
    {
        std::swap(forwardSequence_, reverseSequence_);
        isForward_ = !isForward_;
    }
    
    bool IsRnaPrimer() const;
    const std::string& Complement();
    
private:
    std::string forwardSequence_;
    std::string reverseSequence_;
    std::string complement_;
    std::string reverseId_;
    std::string forwardId_;
    std::string displayName_;
    std::string displayNameReverse_;
    bool isForward_{true};
    bool rnaPrimer_{false};
    bool rnaTarget_{false};
};

#endif /* SearchPrimer_hpp */
