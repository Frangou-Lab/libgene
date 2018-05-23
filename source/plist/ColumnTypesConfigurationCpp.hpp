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

#ifndef ColumnTypesConfigurationCpp_hpp
#define ColumnTypesConfigurationCpp_hpp

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "../def/ColumnType.h"

namespace gene {

class GroupWrapperCpp;

class ColumnTypeWrapperCpp {
 public:
    typedef std::shared_ptr<ColumnTypeWrapperCpp> ColumnTypeWrapperPtr;
    
    std::string description;
    int column_id;
    enum ColumnType type;
    
    std::vector<std::shared_ptr<GroupWrapperCpp>> groups;
    ColumnTypeWrapperCpp(int columnId, std::string description, enum ColumnType type);
    ColumnTypeWrapperCpp(const ColumnTypeWrapperCpp &);
    
    ~ColumnTypeWrapperCpp() = default;
    static std::shared_ptr<ColumnTypeWrapperCpp> columnWithId(int columnId, std::string description,
                                                              enum ColumnType type);
};

class GroupWrapperCpp {
 private:
    std::string group_name_;
    std::string color_;
    
 public:
    std::string GroupName() const;
    std::string Color() const;
    std::map<std::string, std::vector<std::string>> Dictionary() const;
    std::vector<ColumnTypeWrapperCpp::ColumnTypeWrapperPtr> columns;

    GroupWrapperCpp(std::map<std::string, std::string> dictionary);
    GroupWrapperCpp(std::string groupName, std::string color);
    
    static std::unique_ptr<GroupWrapperCpp> GroupWithName(std::string groupName, std::string color);
};

class ColumnTypesConfigurationCpp {
 private:
    std::vector<std::shared_ptr<ColumnTypeWrapperCpp>> m_columns;
    std::vector<std::shared_ptr<GroupWrapperCpp>> m_groups;
    
 public:
    void saveConfigurationOfColumnTypesForFile(std::string inputFileName);
    void readConfigurationOfColumnTypesForFile(std::string inputFileName);
    std::shared_ptr<ColumnTypeWrapperCpp> getColumnById(int columnId);
    std::shared_ptr<GroupWrapperCpp> getGroupByName(std::string groupName);
    
    void AddColumn(int columnId, enum ColumnType type, std::string description);
    void AddColumn(int columnId, std::string description);
    
    std::vector<std::shared_ptr<ColumnTypeWrapperCpp>> ColumnsWithType(enum ColumnType type) const;
    std::vector<std::shared_ptr<ColumnTypeWrapperCpp>> Columns() const;
    std::shared_ptr<ColumnTypeWrapperCpp> IDColumn() const;
};

}  // namespace gene

#endif /* ColumnTypesConfigurationCpp_hpp */
