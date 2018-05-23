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

#include "Plist.hpp"
#include "pugixml.hpp"

#include <fstream>
#include <cstdint>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>

#include "any/any.hpp"

typedef std::string string_type;
typedef std::map<std::string, experimental::any> dictionary_type;
typedef std::vector<experimental::any> array_type;
typedef std::vector<char> data_type;

void writeXMLDictionary(pugi::xml_node& node, const dictionary_type& message);
void writeXMLArray(pugi::xml_node& node, const array_type& array);

template<typename T>
static std::string StringFromValue(const T& value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

dictionary_type parseDictionary(pugi::xml_node& node);
array_type parseArray(pugi::xml_node& node);
std::vector<char> base64Decode(const char *data);
void base64Encode(std::string& dataEncoded, const std::vector<char>& data);
experimental::any parse(pugi::xml_node& doc);
    
template<typename T>
void writeXMLSimpleNode(pugi::xml_node& node, const char* name, const experimental::any& obj)
{
    pugi::xml_node newNode;
    newNode = node.append_child(name);
    newNode.append_child(pugi::node_pcdata).set_value(StringFromValue(experimental::any_cast<const T&>(obj)).c_str());
}

void writeXMLNode(pugi::xml_node& node, const experimental::any& obj)
{
    const std::type_info &objType = obj.type();
    
    if (objType == typeid(int32_t))
        writeXMLSimpleNode<int32_t>(node, "integer", obj);
    else if (objType == typeid(int64_t))
        writeXMLSimpleNode<int64_t>(node, "integer", obj);
    else if (objType == typeid(int))
        writeXMLSimpleNode<int>(node, "integer", obj);
    else if (objType == typeid(long))
        writeXMLSimpleNode<long>(node, "integer", obj);
    else if (objType == typeid(short))
        writeXMLSimpleNode<short>(node, "integer", obj);
    else if (objType == typeid(dictionary_type))
        writeXMLDictionary(node, experimental::any_cast<const dictionary_type&>(obj));
    else if (objType == typeid(string_type))
        writeXMLSimpleNode<string_type>(node, "string", obj);
    else if (objType == typeid(array_type))
        writeXMLArray(node, experimental::any_cast<const array_type&>(obj));
    else if (objType == typeid(double))
        writeXMLSimpleNode<double>(node, "real", obj);
    else if (objType == typeid(float))
        writeXMLSimpleNode<float>(node, "real", obj);
    else if (objType == typeid(bool)) {
        bool value = experimental::any_cast<const bool&>(obj);
        node.append_child(value ? "true" : "false");
    } else
        throw std::runtime_error(std::string("Plist Error: Can't serialize type ") + objType.name());
}

void writeXMLArray(pugi::xml_node& node, const array_type& array)
{
    pugi::xml_node newNode = node.append_child("array");
    for (const auto& elem : array)
        writeXMLNode(newNode, elem);
}

void writeXMLDictionary(pugi::xml_node& node, const dictionary_type& message)
{
    pugi::xml_node newNode = node.append_child("dict");
    for (const auto& elem : message) {
        pugi::xml_node keyNode = newNode.append_child("key");
        keyNode.append_child(pugi::node_pcdata).set_value(elem.first.c_str());
        writeXMLNode(newNode, elem.second);
    }
}

void writePlistXML(pugi::xml_document& doc, const experimental::any& message)
{
    // declaration node
    pugi::xml_node decNode = doc.append_child(pugi::node_declaration);
    decNode.append_attribute("version") = "1.0";
    decNode.append_attribute("encoding") = "UTF-8";
    
    // doctype node
    doc.append_child(pugi::node_doctype).set_value("plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
    
    // root node
    pugi::xml_node plistNode = doc.append_child("plist");
    plistNode.append_attribute("version") = "1.0";
    
    writeXMLNode(plistNode, message);
}

void writePlistXML(std::ostream& stream, const experimental::any& message)
{
    pugi::xml_document doc;
    writePlistXML(doc, message);
    doc.save(stream);
}

void readPlist(const char* byteArrayTemp, int64_t size, experimental::any& message)
{
    const unsigned char *byteArray = reinterpret_cast<const unsigned char*>(byteArrayTemp);
    if (!byteArray || size == 0)
        throw std::runtime_error("Plist: Empty plist data");

    // assume it's XML plist
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(byteArray, (size_t)size);
    if (!result)
        throw std::runtime_error(std::string("Plist: XML parsed with error ") + result.description());

    pugi::xml_node rootNode = doc.child("plist").first_child();
    message = parse(rootNode);
}

namespace gene::plist {

void Serializer::writePlistXML(const std::string& plistPath,
                               const experimental::any& message)
{
    std::ofstream stream(plistPath, std::ios::binary);
    ::writePlistXML(stream, message);
    stream.close();
}

void Serializer::readPlist(std::istream& stream, experimental::any& message)
{
    int64_t start = stream.tellg();
    stream.seekg(0, std::ifstream::end);
    int64_t size = static_cast<int64_t>(stream.tellg()) - start;
    if (size > 0) {
        stream.seekg(0, std::ifstream::beg);
        std::vector<char> buffer(size);
        stream.read(static_cast<char *>(&buffer[0]), size);
        ::readPlist(&buffer[0], size, message);
    } else {
        throw std::runtime_error("Can't read zero length data");
    }
}

} // namespace gene::plist

experimental::any parse(pugi::xml_node& node)
{
    std::string nodeName = node.name();
    experimental::any result;
    if ("dict" == nodeName)
        result = parseDictionary(node);
    else if ("array" == nodeName)
        result = parseArray(node);
    else if ("string" == nodeName)
        result = std::string(node.first_child().value());
    else if ("integer" == nodeName)
        result = (int64_t)atoll(node.first_child().value());
    else if ("real" == nodeName)
        result = atof(node.first_child().value());
    else if ("false" == nodeName)
        result = bool(false);
    else if ("true" == nodeName)
        result = bool(true);
    else
        throw std::runtime_error("Plist: XML unknown node type " + nodeName);
    
    return result;
}

dictionary_type parseDictionary(pugi::xml_node& node)
{
    dictionary_type dict;
    for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it) {
        if (std::string("key") != it->name())
            throw std::runtime_error("Plist: XML dictionary key expected but not found");
        
        std::string key{it->first_child().value()};
        ++it;
        
        if (it == node.end()) {
            throw std::runtime_error("Plist: XML dictionary value expected for key " + key +
                                     "but not found");
        } else if (std::string("key") == it->name()) {
            throw std::runtime_error("Plist: XML dictionary value expected for key " + key +
                                     "but found another key node");
        }
        dict[key] = parse(*it);
    }
    return dict;
}
    
array_type parseArray(pugi::xml_node& node)
{
    array_type array;
    for (auto& xml_node : node)
        array.push_back(parse(xml_node));
    
    return array;
}
