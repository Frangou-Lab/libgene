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

#include "GenBankAnnotation.hpp"

using std::string_view;
using std::string;

GenBankFeature::GenBankFeature(std::string_view feature_section)
{
    // Format of a feature entry goes like this:
    //      record starts with 5 spaces, then the type of the feature and then some
    //      number of spaces to pad the column to 21 charactes total.
    int64_t type_end = feature_section.find(' ');
    type_ = std::string(feature_section.substr(0, type_end));
    
    if (type_ == "gene" || type_ == "exon" || type_ == "STS")
        gene_ = ExtractValueByTag_("/gene", feature_section);
    
    if (type_ == "source")
        chromosome_ = ExtractValueByTag_("/chromosome", feature_section);
    
    if (type_ == "exon")
        number_ = std::stoi(ExtractValueByTag_("/number", feature_section));
    
    if (type_ == "STS")
        sts_ = ExtractValueByTag_("/standard_name", feature_section);
    
    //      From character 22 and onwards follows the range of the feature. The range can be
    //      either 'start..end' or '/join(start1..end1,start2..end2,...)'
    string_view ranges_view = feature_section.substr(16, feature_section.find('/', 17) - 17);
    string ranges_string = std::string(ranges_view);
    RemoveJunkCharacters_(ranges_string, false);
    ParseRanges_(ranges_string);

    product_ = ExtractValueByTag_("/product", feature_section);
}

bool GenBankFeature::ContainsRange(const prim::Range& range) const noexcept
{
    for (const auto& position: positions_) {
        if (position.Contains(range))
            return true;
    }
    return false;
}

std::string GenBankFeature::report_info() const
{
    std::string result;
    if (! product_.empty())
        result += type_ + " | " + product_;
    else if (type_ == "exon")
        result += type_ + + " | " + gene_ + " | number " + std::to_string(number_);
    else if (! gene_.empty() && sts_.empty())
        result += type_ + " | " + gene_;
    else if (! chromosome_.empty())
        result += type_ + " | chromosome " + chromosome_;
    else if (! sts_.empty())
        result += type_ + " | " + sts_ + " | " + gene_;
    else
        result += type_;
    
    return result;
}

std::string GenBankFeature::ExtractValueByTag_(const std::string& tag, string_view view)
{
    // expr: "tag="<value>"
    std::string value;
    
    int64_t start_position_of_value = view.find(tag);
    if (start_position_of_value == std::string::npos)
        return value;
    
    start_position_of_value += tag.size() + 1;
    if (view[start_position_of_value] == '"') {
        start_position_of_value += 1 /* " (double-quote) */;
    }
    
    int64_t end_position_of_value = view.find_first_of("\"\n\r", start_position_of_value);
    value = std::string(view.substr(start_position_of_value, end_position_of_value - start_position_of_value));
    RemoveJunkCharacters_(value, true);
    return value;
}

void GenBankFeature::RemoveJunkCharacters_(std::string& s, bool leave_one_space) const
{
    constexpr char junk_combination[] = "\n               "; // '\n' + 16 spaces.
    int64_t junk = std::string::npos;
    while ((junk = s.rfind(junk_combination)) != std::string::npos) {
        auto start = s.begin() + junk;
        s.erase(start, start + sizeof(junk_combination) - 1);
    }
    if (!leave_one_space) {
        // Clean extra stray spaces if some may be left
        while ((junk = s.rfind(' ')) != std::string::npos) {
            s.erase(s.begin() + junk);
        }
    }
}

void GenBankFeature::ParseRanges_(string_view ranges_section_view, bool complement)
{
    if (ranges_section_view.empty())
        return;
    
    int64_t position;
    if ((position = ranges_section_view.find("join(")) == 0) {
        // Entered 'join' section
        ParseRanges_(ranges_section_view.substr(5, ranges_section_view.size() - 5 - 1), complement);
    } else if ((position = ranges_section_view.find("complement(")) == 0) {
        // Entered 'complement' section
        ParseRanges_(ranges_section_view.substr(11, ranges_section_view.size() - 11 - 1), true);
    } else {
        // Plain single range
        int64_t comma_position = ranges_section_view.find(',');
        if (comma_position == std::string::npos)
            positions_.push_back(RangeFromStringView_(ranges_section_view, complement));
        else {
            positions_.push_back(RangeFromStringView_(ranges_section_view.substr(0, comma_position), complement));
            ParseRanges_(ranges_section_view.substr(comma_position + 1), complement);
        }
    }
}

prim::Range GenBankFeature::RangeFromStringView_(string_view view_of_pair, bool complement)
{
    int64_t dot_position = view_of_pair.find('.');
    std::string first_number = std::string(view_of_pair.substr(0, dot_position));
    bool partial_prime5 = false;
    if (first_number[0] == '<') {
        partial_prime5 = true;
        first_number.erase(first_number.begin());
    }
    
    int64_t start = std::stoll(first_number);
    std::string second_number = std::string(view_of_pair.substr(dot_position + 2));
    
    bool partial_prime3 = false;
    if (second_number[0] == '>') {
        partial_prime3 = true;
        second_number.erase(second_number.begin());
    }
    int64_t end = std::stoll(second_number);
    return prim::Range{start, end};
}

//
// ---- GenBankAnnotation ----
//

GenBankAnnotation::GenBankAnnotation(string_view annotation_view)
{
    while (!annotation_view.empty()) {
        string_view single_feature_section = annotation_view;
        
        // Find the next line starting with a non-space character
        int64_t end_index = -1;
        while (true) {
            end_index = annotation_view.find('\n', end_index + 1);
            
            if ((end_index + 1) == annotation_view.size())
                break;
            
            if (end_index != std::string::npos && annotation_view[end_index + 1] != ' ') {
                // Found a line that starts with a non-space character
                break;
            }
        }
        
        if (end_index == std::string::npos) {
            // This is the last feature. Read to the very end.
            end_index = annotation_view.size();
        }
        single_feature_section.remove_suffix(annotation_view.size() - end_index);
        annotation_view.remove_prefix(single_feature_section.size() + 1);
        features_.push_back(GenBankFeature(single_feature_section));
    }
}

std::vector<std::string> GenBankAnnotation::MatchingFeatures(prim::Range range) const noexcept
{
    std::vector<std::string> matching_features;
    for (const auto& feature: features_) {
        if (feature.ContainsRange(range))
            matching_features.push_back(feature.report_info());
    }
    return matching_features;
}
