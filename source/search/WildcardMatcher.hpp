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

#ifndef LIBGENE_SEARCH_WILDCARDMATCHER_HPP_
#define LIBGENE_SEARCH_WILDCARDMATCHER_HPP_

#include <string_view>

namespace gene {

class WildcardMatcher {
 public:
    static bool Match(std::string_view expression, std::string_view str)
    {
        std::string_view cp;
        std::string_view mp;
        
        if (expression.empty())
            return true;
        
        while (! str.empty() && expression[0] != '*') {
            if (expression[0] != str[0] && expression[0] != '?') {
                return false;
            }
            expression.remove_prefix(1);
            str.remove_prefix(1);
        }
        
        while (!str.empty()) {
            if (expression[0] == '*') {
                expression.remove_prefix(1);
                if (expression.empty())
                    return true;

                mp = expression;
                cp = str.substr(1);
            } else if (expression[0] == str[0] || expression[0] == '?') {
                expression.remove_prefix(1);
                str.remove_prefix(1);
            } else {
                expression = mp;
                str = cp;
                cp.remove_prefix(1);
            }
        }
        
        while (expression[0] == '*') {
            expression.remove_prefix(1);
        }
        return expression.empty();
    }
};

}  // namespace gene

#endif  // LIBGENE_SEARCH_WILDCARDMATCHER_HPP_
