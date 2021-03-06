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

#ifndef Logger_hpp
#define Logger_hpp

#ifdef __cplusplus
#define EXTERNC extern "C"

#include <string>
#include <functional>

namespace gene::logger {

extern std::function<void(std::string)> logLambda;

void Log(const char *format, ...);
void Log(std::string message);

}  // namespace gene::logger

#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int PrintfLog(const char* format, ...);

#undef EXTERNC

#endif // Logger_hpp
