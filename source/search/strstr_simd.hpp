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

#ifndef strstr_simd_hpp
#define strstr_simd_hpp

#include <string>
#include <string_view>

namespace gene {

std::string_view strstr_simd(std::string_view haystack,
                             const std::string& needle);

}  // namespace gene

#endif  /* strstr_simd_hpp */
