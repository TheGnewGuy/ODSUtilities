#pragma once
#include <string>
#include <map>
#include "rapidxml.hpp"

using namespace std;

class RapidXmlWrapper
{
public:
	RapidXmlWrapper(){}
public:
	static rapidxml::xml_node<>* getNodeByName(const string& nodeName, rapidxml::xml_node<>* parentNode);
	static rapidxml::xml_node<>* getNodeByNameAndAttribute(const string& nodeName, const string& attrName, const string& attribValue, rapidxml::xml_node<>* node);
	static string getAttributeValue(const std::string& attributeName, rapidxml::xml_node<>* node);
	static map<string, string> getParamMapsFromNode(rapidxml::xml_node<>* node);
};

