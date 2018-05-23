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

#ifndef LIBGENE_UTILS_MISCPRIMITIVES_H_
#define LIBGENE_UTILS_MISCPRIMITIVES_H_

#include <string>
#include <stdexcept>

namespace gene::prim {

/*

struct Range {
    constexpr Range() = default;
    constexpr explicit Range(int64_t begin, // inclusive
                          int64_t end);  // non-inclusive

    constexpr bool Contains(const Range& other) const noexcept;
    constexpr bool Within(const Range& other) const noexcept;
    constexpr bool Empty() const noexcept;
    constexpr int64_t Length() const noexcept;

    int64_t begin{-1};
    int64_t end{-1};
};

*/

struct Range {
    constexpr Range() = default;
    constexpr explicit Range(int64_t begin /* inclusive */,
                             int64_t end   /* non-inclusive */)
    : begin(begin)
    , end(end)
    {
    }

    constexpr
    bool Contains(const Range& other) const noexcept
    {
        return other.begin >= begin && other.end <= end;
    }

    constexpr
    bool Contains(int64_t point) const noexcept
    {
        return point >= begin && point < end;
    }

    constexpr
    bool Within(const Range& other) const noexcept
    {
        return begin >= other.begin && end <= other.end;
    }

    constexpr
    bool Empty() const noexcept
    {
        return Length() == 0;
    }

    constexpr
    int64_t Length() const noexcept
    {
        return end - begin;
    }

    int64_t begin{-1};
    int64_t end{-1};
};

/*

A class for exceptions that are OK to show to the user as error messages.

*/
class UserVisibleError : public std::runtime_error {
 public:
    explicit UserVisibleError(const std::string& msg)
    : std::runtime_error(msg) {}

    explicit UserVisibleError(const char *msg)
    : std::runtime_error(msg) {}

    UserVisibleError(const UserVisibleError& error) noexcept
    : UserVisibleError(error.what()) {}
};

}  // namespace gene::prim

#endif  // LIBGENE_UTILS_MISCPRIMITIVES_H_
