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

#include "Logger.hpp"

#include <cstdlib>
#include <cstdarg>
#include <vector>

int PrintfLog(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    if (gene::logger::logLambda) {
        size_t size = vsnprintf(nullptr, 0, format, args);
        va_end(args);
        // Re-initialize 'args' variable since vsnprintf invalidates it every time.
        va_start(args, format);
        std::vector<char> buffer(size + 1, '\0');
        vsnprintf(buffer.data(), size + 1, format, args);
        gene::logger::logLambda(buffer.data());
    }
    va_end(args);
    return 1;
}


namespace gene::logger {

void Log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    if (logLambda) {
        size_t size = vsnprintf(nullptr, 0, format, args);
        va_end(args);
        // Re-initialize the 'args' variable since vsnprintf invalidates it every time.
        va_start(args, format);
        std::vector<char> buffer(size + 1, '\0');
        vsnprintf(buffer.data(), size + 1, format, args);
        logLambda(buffer.data());
    }
    va_end(args);
}

void Log(std::string message)
{
    if (logLambda)
        logLambda(message);
}

std::function<void(std::string)> logLambda;

}  // namespace gene::logger
