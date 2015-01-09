#include "stdafx.h"
#include "RapidXmlWrapper.h"
#include <sstream>


/**
* Gets a node object from given node name and parent node.
*/
rapidxml::xml_node<>* RapidXmlWrapper::getNodeByName(const string& nodeName, rapidxml::xml_node<>* parentNode)
{
	if(parentNode == NULL)
	{
		// parent node must not be null
		ostringstream errorBuilder;
		errorBuilder << " parentNode object should not be null: ";
		throw runtime_error(errorBuilder.str());
	}

   rapidxml::xml_node<>* node = parentNode->first_node();

   while(node != NULL)
   {
      if(node->name() == nodeName)
      {
         return node;
      }

      node = node->next_sibling(); 
   }
   return NULL;
}



/**
* Get node object from given node name, attribName, attribValue and parent node.
*/
//! Gets node object from given node name, attribName, attribValue and parent node. 
//! \param Noded Name, me of node to find
//! \param attribName, Name of attribute to find
//! \param attribValue, Value of the attribute
//! \param parent node, Parent node
//! \return Pointer to found Node, or NULL if not found.
rapidxml::xml_node<>* RapidXmlWrapper::getNodeByNameAndAttribute(const string& nodeName, const string& attrName, const string& attribValue, rapidxml::xml_node<>* parentNode)
{
	if(parentNode == NULL)
	{
		// parent node must not be null
		ostringstream errorBuilder;
		errorBuilder << " parentNode object should not be null: ";
		throw runtime_error(errorBuilder.str());
	}

	rapidxml::xml_attribute<>* attr = parentNode->first_attribute(attrName.c_str());
	if( (attr != NULL) && (attr->value() == attribValue) && (parentNode->name() == nodeName))
	{
		return parentNode;
	}

	rapidxml::xml_node<>* node = parentNode->first_node();

	while(node != NULL)
	{
		if(node->name() == nodeName)
		{
			// If attribute name and value empty then return node
			if(attrName.empty() || attribValue.empty())
			{
				return node;
			}
			else
			{
				rapidxml::xml_attribute<>* attr = node->first_attribute(attrName.c_str());
				if( (attr != NULL) && (attr->value() == attribValue) )
				{
					return node;
				}
			}
		}
		node = node->next_sibling(); 
	}
	return NULL;
}


/**
* Get attribute value from given attribute name and given node object.
*/
string RapidXmlWrapper::getAttributeValue(const std::string& attributeName, rapidxml::xml_node<>* node)
{
	string attributValue = "";

	if(node == NULL)
	{
		// throw This node must be initialized
		ostringstream errorBuilder;
		errorBuilder << "node object should not be null,for attribute name:" + string(attributeName);
		throw runtime_error(errorBuilder.str());
	}

	rapidxml::xml_attribute<>* attribute = node->first_attribute(attributeName.c_str());
	if(NULL == attribute)
	{
		ostringstream errorBuilder;
		errorBuilder << "Attribute not found, for attribute name:" + attributeName;
		throw runtime_error(errorBuilder.str());
	}
	attributValue = attribute->value();
	return attributValue;
}


/**
* Get map of string, string i.e. key value pair from given node object.
* ex: <Employees>
		<Employee Name="John" Age="30"/>
		<Employee Name="Harry" Age="27"/>
		<Employee Name="Kate" Age="29"/>
*     </Employees> 
*    
*    Method will result the map of Employees with Name and age as key value pairs 
*/
map<string, string> RapidXmlWrapper::getParamMapsFromNode(rapidxml::xml_node<>* node)
{
   map<string, string> paramsMap;

   if(node == NULL) 
   {
      // This node must be initialized
	   ostringstream errorBuilder;
	   errorBuilder << "node object should not be null";
	   throw runtime_error(errorBuilder.str());
   }


   for(rapidxml::xml_node<>* currNode = node->first_node(); currNode != NULL; currNode = currNode->next_sibling())
   {
      string nodeName = currNode->name();
      rapidxml::xml_attribute<>* nameAttr = currNode->first_attribute("Name");
      rapidxml::xml_attribute<>* valueAttr = currNode->first_attribute("Age");
      if(nodeName == "Employee" && nameAttr != NULL && valueAttr != NULL)
      {
         paramsMap.insert(pair<string, string>(nameAttr->value(), valueAttr->value()));
      }
   }

   return paramsMap;
}