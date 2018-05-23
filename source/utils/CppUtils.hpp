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

#ifndef LIBGENE_UTILS_CPPUTILS_HPP_
#define LIBGENE_UTILS_CPPUTILS_HPP_

#include <string>
#include <algorithm>
#include <vector>
#include <type_traits>

#include "../def/FileType.hpp"
#include "../flags/CommandLineFlags.hpp"

namespace gene::utils {

std::string ConstructOutputNameWithFile(std::string inputFileName,
                                        FileType type,
                                        std::string output,
                                        const std::unique_ptr<CommandLineFlags>& flags,
                                        std::string suffix);

enum FileType str2type(const std::string &str);
std::string type2str(enum FileType type);

std::string type2extension(enum FileType type);
enum FileType extension2type(const std::string &ext);

std::string str2extension(const std::string &str);
std::string extension2str(const std::string &ext);

template <typename C, typename F, typename = void>
struct has_find : public std::false_type {};

template <typename C, typename R, typename... A>
struct has_find<C, R(A...),
    typename std::enable_if<std::is_same<R, void>::value ||
    std::is_convertible<decltype(std::declval<C>().find(std::declval<A>()...)), R>::value>::type> : public std::true_type {
};

template<typename ContainerT, typename ElemT>
constexpr bool ContainerHasElement_No_find_(const ContainerT& container, const ElemT& elem)
{
    return std::find(container.begin(), container.end(), elem) != container.end();
}

template<typename ContainerT, typename ElemT>
constexpr bool ContainerHasElement_Has_find_(const ContainerT& container, const ElemT& elem)
{
    return container.find(elem) != container.end();
}

template<typename ContainerT, typename ElemT>
constexpr bool Contains(const ContainerT& container, const ElemT& elem)
{
    if constexpr (has_find<ContainerT, typename ContainerT::const_iterator(const ElemT&)>::value)
        return ContainerHasElement_Has_find_(container, elem);
    else
        return ContainerHasElement_No_find_(container, elem);
}

//
// Searches for a reverse string 'what' which is interpreted as a reverse complement
// without having to actually create this reverse complement
//
int64_t FindAsReverseComplement(const std::string& where, const std::string& what);

//
// Searches for a reverse string 'what' which is interpreted as a reverse complement
// without having to actually create this reverse complement starting from the end of 'where'
//
int64_t RfindAsReverseComplement(const std::string& where, const std::string& what);

//
bool AreComplements(char a, char b);

template <class BiIter, class OutIter>
void MakeReverseComplement(BiIter srcBegin, BiIter srcEnd,
                           OutIter destBegin, OutIter destEnd, bool isRna)
{
    std::reverse_copy(srcBegin, srcEnd, destBegin);
    std::for_each(destBegin, destEnd, [isRna](auto& c)
                  {
                      switch (c) {
                          case 'A':
                          case 'a':
                              c = (isRna ? 'U' : 'T');
                              break;
                          case 'T':
                          case 't':
                              c = 'A';
                              break;
                          case 'G':
                          case 'g':
                              c = 'C';
                              break;
                          case 'C':
                          case 'c':
                              c = 'G';
                              break;
                          case 'U':
                          case 'u':
                              c = 'A';
                              break;
                          default:
                              c = 'N';
                      }
                  });
}

inline std::string MakeReverseComplement(const std::string& original,
                                         bool is_rna)
{
    std::string complement = original;

    std::reverse(complement.begin(), complement.end());
    std::for_each(complement.begin(), complement.end(), [is_rna](auto& c)
                  {
                      switch (c) {
                          case 'A':
                          case 'a':
                              c = (is_rna ? 'U' : 'T');
                              break;
                          case 'T':
                          case 't':
                              c = 'A';
                              break;
                          case 'G':
                          case 'g':
                              c = 'C';
                              break;
                          case 'C':
                          case 'c':
                              c = 'G';
                              break;
                          case 'U':
                          case 'u':
                              c = 'A';
                              break;
                          default:
                              c = 'N';
                      }
                  });
    return complement;
}

FastqVariant FormatNameToVariant(const std::string& format);
std::string FastqVariantToSuffix(FastqVariant variant);
std::vector<std::string> LoadQueriesFromFile(std::string path);

}  // namespace gene::utils

#endif // LIBGENE_UTILS_CPPUTILS_HPP_
