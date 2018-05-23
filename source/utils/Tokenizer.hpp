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

#ifndef LIBGENE_UTILS_TOKENIZER_HPP_
#define LIBGENE_UTILS_TOKENIZER_HPP_

#include <string>
#include <string_view>

namespace gene {

class Tokenizer {
 private:
    std::string_view text_;
    int64_t position_;
    const char delimiter_;
    int64_t lexeme_start_pos_;
    int64_t lexeme_length_;
    std::string current_lexeme_;
    
    void FormLexeme_(int64_t pos);
    void TrimLexeme_();
    
 public:
    explicit Tokenizer(char delimiter);
    Tokenizer(std::string_view text, char delimiter);
    Tokenizer(std::string_view text, const std::string& delimiter);

    void SetText(std::string_view expr);
    bool ReadNext();
    
    // Allows to read the value only once!
    std::string&& GetNextToken();
};

}  // namespace gene

#endif  // LIBGENE_UTILS_TOKENIZER_HPP_
