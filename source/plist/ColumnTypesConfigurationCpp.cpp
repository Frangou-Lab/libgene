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

#include "ColumnTypesConfigurationCpp.hpp"
#include "../utils/CppUtils.hpp"
#include "../utils/StringUtils.hpp"
#include "Plist.hpp"

#include <string>
#include <cassert>

#include "../plist/any/any.hpp"

namespace gene {

static const char ROOT_KEY[] = "column_types";
static const char COLUMNS_KEY[] = "columns";
static const char GROUPS_KEY[] = "groups";
static const char ID_KEY[] = "id";
static const char TYPE_KEY[] = "type";
static const char DESCRIPTION_KEY[] = "description";

//
// ColumnTypeWrapperCpp
//

ColumnTypeWrapperCpp::ColumnTypeWrapperCpp(int column_id, std::string description,
                                           enum ColumnType type)
: column_id(column_id), description(description), type(type)
{

}

ColumnTypeWrapperCpp::ColumnTypeWrapperCpp(const ColumnTypeWrapperCpp &other)
{
    column_id = other.column_id;
    description = other.description;
    type = other.type;
}

std::shared_ptr<ColumnTypeWrapperCpp> ColumnTypeWrapperCpp::columnWithId(int columnId,
                                                                         std::string description,
                                                                         enum ColumnType type)
{
    return std::make_shared<ColumnTypeWrapperCpp>(columnId, description, type);
}

//
// GroupWrapperCpp
//

std::string GroupWrapperCpp::GroupName() const
{
    return group_name_;
}

std::string GroupWrapperCpp::Color() const
{
    return color_;
}

GroupWrapperCpp::GroupWrapperCpp(std::string group_name, std::string color)
: group_name_(group_name), color_(color)
{
    
}

std::map<std::string, std::vector<std::string>> GroupWrapperCpp::Dictionary() const
{
    return {};
}

GroupWrapperCpp::GroupWrapperCpp(std::map<std::string, std::string> dictionary)
{
    
}

std::unique_ptr<GroupWrapperCpp> GroupWrapperCpp::GroupWithName(std::string groupName,
                                                                std::string color)
{
    return std::make_unique<GroupWrapperCpp>(groupName, color);
}

//
// ColumnTypesConfigurationCpp
//
void ColumnTypesConfigurationCpp::saveConfigurationOfColumnTypesForFile(std::string inputFile)
{
    auto plistPath = utils::StringByDeletingPathExtension(inputFile) + ".ctp";
    std::vector<experimental::any> allColumns;
    for (const auto& columnTypeWrapper : m_columns) {
        std::map<std::string, experimental::any> column;
        column[ID_KEY] = columnTypeWrapper->column_id;
        column[TYPE_KEY] = static_cast<int>(columnTypeWrapper->type);
        column[DESCRIPTION_KEY] = columnTypeWrapper->description;
        allColumns.push_back(column);
    }
    
    typedef std::vector<experimental::any> all_groups_type;
    all_groups_type allGroups;
	for (const auto& groupWrapper : m_groups) {
        std::map<std::string, experimental::any> groupDictionary;
        std::vector<experimental::any> columnsForDictionary;
        
        auto columns = groupWrapper->columns;
        for (const auto& column : columns)
            columnsForDictionary.push_back(column->column_id);
    
        groupDictionary[COLUMNS_KEY] = columnsForDictionary;
		allGroups.push_back(groupDictionary);
	}
    
    typedef std::map<std::string, experimental::any> dict_any;
    
    dict_any columns;
    columns[COLUMNS_KEY] = allColumns;
    
    dict_any groups;
    groups[GROUPS_KEY] = allGroups;
    
    dict_any columnsAndGroups;
    columnsAndGroups[GROUPS_KEY] = allGroups;
    columnsAndGroups[COLUMNS_KEY] = allColumns;
    
    dict_any st;
    st[ROOT_KEY] = columnsAndGroups;
    
    plist::Serializer::writePlistXML(plistPath, st);
}

std::vector<std::shared_ptr<ColumnTypeWrapperCpp>>
ColumnTypesConfigurationCpp::ColumnsWithType(ColumnType type) const
{
    std::vector<std::shared_ptr<ColumnTypeWrapperCpp>> columnsWithSameType;
    for (const auto& column : m_columns) {
        if (column->type == type)
            columnsWithSameType.push_back(column);
    }
    return columnsWithSameType;
}

std::vector<std::shared_ptr<ColumnTypeWrapperCpp>> ColumnTypesConfigurationCpp::Columns() const
{
    return m_columns;
}

std::shared_ptr<ColumnTypeWrapperCpp> ColumnTypesConfigurationCpp::IDColumn() const
{
    auto allIdColumns = ColumnsWithType(ID);
    if (allIdColumns.empty())
        return nullptr;
    
    assert(allIdColumns.size() == 1);
    return std::make_shared<ColumnTypeWrapperCpp>(*(allIdColumns.back()));
}

void ColumnTypesConfigurationCpp::AddColumn(int columnId, enum ColumnType type,
                                            std::string description)
{
    m_columns.push_back(std::make_shared<ColumnTypeWrapperCpp>(columnId, description, type));
}

void ColumnTypesConfigurationCpp::AddColumn(int columnId, std::string groupName)
{
    auto column = getColumnById(columnId);
    auto group = getGroupByName(groupName);
    if (group && column)
    {
        group->columns.push_back(column);
        column->groups.push_back(group);
    }
}

void ColumnTypesConfigurationCpp::readConfigurationOfColumnTypesForFile(std::string inputFileName)
{
    auto plistPath = utils::StringByDeletingPathExtension(inputFileName) + ".ctp";
    std::ifstream plistStream(plistPath);
    if (!plistStream) {
        // File probably doesn't exits
        return;
    }
    
    typedef std::map<std::string, experimental::any> dict_type;
    typedef std::vector<experimental::any> vect_type;
    
    experimental::any xmlTree;
    plist::Serializer::readPlist(plistStream, xmlTree);
    experimental::any root = experimental::any_cast<dict_type>(xmlTree)[ROOT_KEY];
    auto allColumns = experimental::any_cast<vect_type>(experimental::any_cast<dict_type>(root)[COLUMNS_KEY]);
    
    for (const auto& column: allColumns) {
        dict_type dict = experimental::any_cast<dict_type>(column);
        int columnId = static_cast<int>(experimental::any_cast<long long>(dict[ID_KEY]));
        std::string description = experimental::any_cast<std::string>(dict[DESCRIPTION_KEY]);
        ColumnType type = static_cast<ColumnType>(experimental::any_cast<long long>(dict[TYPE_KEY]));
        m_columns.push_back(std::make_shared<ColumnTypeWrapperCpp>(columnId, description, type));
    }
    
    vect_type allGroups = experimental::any_cast<vect_type>(experimental::any_cast<dict_type>(root)[GROUPS_KEY]);
    typedef std::map<std::string, std::string> group_dict;
    //for (const auto& groupDictionary: allGroups)
    //{
        //group_dict groupDict = exp::any_cast<group_dict>(groupDictionary);
        //auto group = std::make_shared<GroupWrapperCpp>(groupDict);
        //m_groups.push_back(group);
    
        //std::vector<int> columns = groupDict[COLUMNS_KEY];
        //for (int columnId: columns)
        //{
        //   addColumn(columnId, group->getGroupName());
        //}
    //}
}

std::shared_ptr<ColumnTypeWrapperCpp> ColumnTypesConfigurationCpp::getColumnById(int columnId)
{
    auto e = m_columns.begin();
    while ((*e)->column_id != columnId) {
        ++e;
    }
    return *e;
}

std::shared_ptr<GroupWrapperCpp> ColumnTypesConfigurationCpp::getGroupByName(std::string groupName)
{
    auto e = m_groups.begin();
    while ((*e)->GroupName() != groupName) {
        ++e;
    }
    return *e;
}

}  // namespace gene
