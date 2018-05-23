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

#ifndef GenBankAnnotation_hpp
#define GenBankAnnotation_hpp

#include <vector>
#include <string>
#include <string_view>

#include "../../utils/MiscPrimitives.hpp"

namespace gene {

enum class RangeKind {
    Complete,
    Prime5,
    Prime3,
    Complement
};

class GenBankFeature {
 public:
    GenBankFeature(std::string_view feature_section);
    ~GenBankFeature() = default;
    
    bool ContainsRange(const prim::Range& range) const noexcept;
    
    std::string type() const { return type_; }
    std::string product() const { return product_; }
    
    //
    // Returs a string that should be used in the output report file.
    //
    std::string report_info() const;
    
 private:
    std::string ExtractValueByTag_(const std::string& tag,
                                   std::string_view view);
    void RemoveJunkCharacters_(std::string& s, bool leave_one_space) const;
    void ParseRanges_(std::string_view ranges_section_view,
                      bool complement = false);
    prim::Range RangeFromStringView_(std::string_view view_of_pair,
                                     bool complement);
    
    std::string type_;
    std::string gene_;
    std::string chromosome_;
    std::string product_;
    std::string sts_;
    int number_;
    std::vector<prim::Range> positions_;
};

class GenBankAnnotation {
 public:
    GenBankAnnotation() = default;
    GenBankAnnotation(std::string_view annotation_view);
    ~GenBankAnnotation() = default;
    
    std::vector<std::string> MatchingFeatures(prim::Range range) const noexcept;

 private:
    std::vector<GenBankFeature> features_;
};

}  // namespace gene

#endif /* GenBankAnnotation_hpp */
