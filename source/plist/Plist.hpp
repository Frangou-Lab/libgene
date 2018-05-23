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

#ifndef Plist_hpp
#define Plist_hpp

#include <string>
#include <iosfwd>

#include "pugixml.hpp"
#include "any/any.hpp"

namespace gene::plist {

class Serializer {
public:
    static void writePlistXML(const std::string& plistPath,
                              const experimental::any& message);
    static void readPlist(std::istream& stream,
                          experimental::any& message);
};
    
}  // namespace gene::plist

#endif /* Plist_hpp */
