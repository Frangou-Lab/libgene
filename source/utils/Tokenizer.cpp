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

#include "Tokenizer.hpp"
#include "StringUtils.hpp"

#include <cassert>
#include <type_traits>

namespace gene {

Tokenizer::Tokenizer(std::string_view text, char delimiter)
: Tokenizer::Tokenizer(delimiter)
{
    SetText(text);
}

Tokenizer::Tokenizer(char delimiter)
: delimiter_(delimiter)
{
    position_ = 0;
}

void Tokenizer::SetText(std::string_view text)
{
    text_ = text;
    position_ = 0;
}

std::string&& Tokenizer::GetNextToken()
{
    return std::move(current_lexeme_);
}

void Tokenizer::FormLexeme_(int64_t end)
{
    lexeme_start_pos_ = position_;
    lexeme_length_ = end - lexeme_start_pos_;
    current_lexeme_.assign(text_.data() + lexeme_start_pos_, lexeme_length_);
    utils::ReplaceOccurrencesOfString(current_lexeme_, "\"\"", "\"");
    position_ = end;
}

void Tokenizer::TrimLexeme_()
{
    if (lexeme_length_ > 0) {
        char c = text_[lexeme_start_pos_];
        while (c == ' ' || c == '\r' || c == '\n') {
            c = text_[++lexeme_start_pos_];
            lexeme_length_--;
        }
    }
    if (lexeme_length_ > 0) {
        char c = text_[lexeme_start_pos_ + lexeme_length_ - 1];
        while (c == ' ' || c == '\r' || c == '\n') {
            lexeme_length_--;
            c = text_[lexeme_start_pos_ + lexeme_length_ - 1];
        }
    }
    current_lexeme_.assign(text_.data() + lexeme_start_pos_, lexeme_length_);
    utils::ReplaceOccurrencesOfString(current_lexeme_, "\"\"", "\"");
}

bool Tokenizer::ReadNext()
{
    lexeme_start_pos_ = 0;
    lexeme_length_ = 0;
    bool escaping = false;
    const int64_t length = text_.size();
    
    // Skip whitespace
    for (; position_ < length; ++position_) {
        char ch = text_[position_];
        
        if (ch == delimiter_)
            break;
        
        if (ch != ' ' && ch != '\r' && ch != '\n')
            break;
    }
    
    if (position_ >= length)
        return false;
    
    if (delimiter_ == text_[position_])
        ++position_; // Skip delimiter
    
    for (int64_t pos = position_; pos < length; ++pos) {
        if (text_[pos] == '\"') {
            if (!escaping) {
                escaping = true;
                continue;
            }
            if (pos + 1 < length && text_[pos + 1] == '\"') {
                pos++;
                continue;
            }
            ++position_;
            FormLexeme_(pos);
            ++position_;
            return true;
        }
        
        if (escaping)
            continue;
        
        if (text_[pos] == delimiter_) {
            FormLexeme_(pos);
            TrimLexeme_();
            return true;
        }
    }
    
    // The last value
    lexeme_start_pos_ = position_;
    lexeme_length_ = length - lexeme_start_pos_;
    current_lexeme_.assign(text_.data() + lexeme_start_pos_, lexeme_length_);
    utils::ReplaceOccurrencesOfString(current_lexeme_, "\"\"", "\"");
    position_ = length;
    return true;
}

}  // namespace gene
