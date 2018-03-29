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

#ifndef LIBGENE_UTILS_FILEUTILS_H_
#define LIBGENE_UTILS_FILEUTILS_H_

#define BEGIN_NAMESPACE_LIBGENE_UTILS_ namespace utils {
#define END_NAMESPACE_LIBGENE_UTILS_ }

BEGIN_NAMESPACE_LIBGENE_UTILS_

#include <fstream>

bool IsDirectory(const std::string& path);
std::vector<std::string> GetDirectoryContents(const std::string& path);
int64_t GetFileSize(const std::string& path);

bool CheckFstreamsEqual(std::ifstream& f1, std::ifstream& f2);
bool CheckFstreamsEqualUnordered(std::ifstream& f1, std::ifstream& f2);

END_NAMESPACE_LIBGENE_UTILS_

#undef BEGIN_NAMESPACE_LIBGENE_UTILS_
#undef END_NAMESPACE_LIBGENE_UTILS_

#endif  // LIBGENE_UTILS_FILEUTILS_H_
