// RapidXML.cpp : main project file.

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <processenv.h>
#include <tchar.h>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "RapidXmlWrapper.h"

using namespace System;
using namespace std;
using namespace rapidxml;

void Pause()
{
	cout << "Press Enter to continue...";
	cout.flush();
	cin.ignore(10000, '\n');
	return;
}

int main(array<System::String ^> ^args)
{
	#define BUFSIZE MAX_PATH

	string input_xml;
	string line;
	TCHAR Buffer[BUFSIZE];
	DWORD dwRet;

	if ( !SetCurrentDirectory(_T("..\\Input")) )
	{
		printf("SetCurrentDirectory failed (%d)\n", GetLastError());
		Pause();
		return 1;
	}

	dwRet = GetCurrentDirectory(BUFSIZE, Buffer);
	//_tprintf(TEXT("Current directory is: (%s)\n"), Buffer);

//	ifstream in("demo.xml");
	ifstream in("content.xml");

	// read file into input_xml
	while (getline(in, line))
		input_xml += line;

	// make a safe-to-modify copy of input_xml
	// (you should never modify the contents of an std::string directly)
	vector<char> xml_copy(input_xml.begin(), input_xml.end());
	xml_copy.push_back('\0');

	// only use xml_copy from here on!
	xml_document<> doc;
	// we are choosing to parse the XML declaration
	// parse_no_data_nodes prevents RapidXML from using the somewhat surprising
	// behavior of having both values and data nodes, and having data nodes take
	// precedence over values when printing
	// >>> note that this will skip parsing of CDATA nodes <<<
	doc.parse<parse_declaration_node | parse_no_data_nodes>(&xml_copy[0]);

	cout << "Name of my first node is: " << doc.first_node()->name() << "\n";  /*test the xml_document */

	// alternatively, use one of the two commented lines below to parse CDATA nodes,
	// but please note the above caveat about surprising interactions between
	// values and data nodes (also read http://www.ffuts.org/blog/a-rapidxml-gotcha/)
	// if you use one of these two declarations try to use data nodes exclusively and
	// avoid using value()
	//doc.parse<parse_declaration_node>(&xml_copy[0]); // just get the XML declaration
	//doc.parse<parse_full>(&xml_copy[0]); // parses everything (slowest)

	// since we have parsed the XML declaration, it is the first node
	// (otherwise the first node would be our root node)
	string encoding = doc.first_node()->first_attribute("encoding")->value();
	// encoding == "utf-8"

	// we didn't keep track of our previous traversal, so let's start again
	// we can match nodes by name, skipping the xml declaration entirely

	xml_node<>* root_node = doc.first_node("office:document-content");

	string rootnode_type = root_node->first_attribute("xmlns:table")->value();

	xml_node<>* office_body = root_node->first_node("office:body");
	xml_node<>* office_spreadsheet = office_body->first_node("office:spreadsheet");

	// Iterate over the brewerys
	for (xml_node<> * table_table = office_spreadsheet->first_node("table:table"); table_table; table_table = table_table->next_sibling())
	{
		printf("Sheet name %s. \n",
			table_table->first_attribute("table:name")->value());
		string sheetname = table_table->first_attribute("table:name")->value();
		if (sheetname == "Sheet1")
			table_table->first_attribute("table:name")->value("EditedSheet1");
		if (sheetname == "Sheet2")
			table_table->first_attribute("table:name")->value("EditedSheet2");
		if (sheetname == "Sheet3")
			table_table->first_attribute("table:name")->value("EditedSheet3");
		// Interate over the rows
		for (xml_node<> * table_tablerow = table_table->first_node("table:table-row"); table_tablerow; table_tablerow = table_tablerow->next_sibling())
		{
			// Interate over the cells
			for (xml_node<> * table_tablecell = table_tablerow->first_node("table:table-cell"); table_tablecell; table_tablecell = table_tablecell->next_sibling())
			{
				// Iterate through all of the nodes attributes looking for a given string.
				// In this case, repeated should be the only attribute but I wanted to have complete code.
				int foundstring = false;
				for (const rapidxml::xml_attribute<>* a = table_tablecell->first_attribute(); a; a = a->next_attribute())
				{
					string attrtemp = a->name();
					if (attrtemp == "table:number-columns-repeated")
						foundstring = true;
				}

				// When we have a match we are done. The repeated is repeating empty cells.
				if (foundstring) break;

				// The actual data is in the "text:p" field
				xml_node<> * table_celltext = table_tablecell->first_node("text:p");
				string myval = table_celltext->value();
				printf("Cell value %s. \n",
					table_celltext->value());
			}
			cout << endl;
		}
		cout << endl;
	}

	// Delete a selected node
	rapidxml::xml_node<>* dNode = RapidXmlWrapper::getNodeByNameAndAttribute("table:table", "table:name", "EditedSheet3", office_spreadsheet);
	office_spreadsheet->remove_node(dNode);

	cout << "Write file out" << endl;
	Pause();

	ofstream file;
	//file.open(filename.c_str());
	file.open("updated.xml");
	// The following required #include "rapidxml_print.hpp" to convert the DOM tree into 'normal' XML
	file << doc;
	file.close();
}