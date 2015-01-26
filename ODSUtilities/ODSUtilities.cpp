// ODSUtilities.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "unzip.h"
#include "zip.h"
#include "..\tinyxml\tinyxml.h"

TCHAR mBuff[1024];		// Buffer for messages

bool any_errors = false; bool p_abort = false;
void msg(const TCHAR *s)
{
	if (s[0] == '*') any_errors = true;
#ifdef UNDER_CE
	int res = IDOK;
	if (s[0] == '*') res = MessageBox(0, s, _T("Zip error"), MB_ICONERROR | MB_OKCANCEL);
	else if (s[0] == '.') MessageBeep(0);
	else MessageBox(0, s, _T("Zip test"), MB_OKCANCEL);
	if (res == IDCANCEL) p_abort = true;
#else
	_tprintf(_T("%s\n"), s);
#endif
}

void Pause()
{
	std::cout << "Press Enter to continue...";
	std::cout.flush();
	std::cin.ignore(10000, '\n');
	return;
}

void ExtractToDisk(HZIP hz, int index, ZIPENTRY ze)
{
	wsprintf(mBuff, _T("Extract file: %s to disk"), ze.name);
	msg(mBuff);
	UnzipItem(hz, index, ze.name);
}

void ExtractToMemory(HZIP hz, int index, ZIPENTRY ze)
{
	char *ibuf = NULL;

	wsprintf(mBuff, _T("Extract file: %s to memory"), ze.name);
	msg(mBuff);
	ibuf = new char[ze.unc_size];
	UnzipItem(hz, index, ibuf, ze.unc_size);
	delete[] ibuf;
}

void processXMLFile()
{
	TiXmlDocument doc("content.xml");
	bool loadOkay = doc.LoadFile();

	if (!loadOkay)
	{
		printf("Could not load test file 'content.xml'. Error='%s'. Exiting.\n", doc.ErrorDesc());
		exit(1);
	}

	printf("** Demo doc read from disk: ** \n\n");
	printf("** Printing via doc.Print **\n");
	doc.Print(stdout);

	//{
	//	printf("** Printing via TiXmlPrinter **\n");
	//	TiXmlPrinter printer;
	//	doc.Accept(&printer);
	//	fprintf(stdout, "%s", printer.CStr());
	//}

	TiXmlNode* node = 0;
	TiXmlElement* todoElement = 0;
	TiXmlElement* itemElement = 0;

}


// ----------------------------------------------------------------------
// STDOUT dump and indenting utility functions
// ----------------------------------------------------------------------
const unsigned int NUM_INDENTS_PER_SPACE = 2;

const char * getIndent(unsigned int numIndents)
{
	static const char * pINDENT = "                                      + ";
	static const unsigned int LENGTH = strlen(pINDENT);
	unsigned int n = numIndents*NUM_INDENTS_PER_SPACE;
	if (n > LENGTH) n = LENGTH;

	return &pINDENT[LENGTH - n];
}

// same as getIndent but no "+" at the end
const char * getIndentAlt(unsigned int numIndents)
{
	static const char * pINDENT = "                                        ";
	static const unsigned int LENGTH = strlen(pINDENT);
	unsigned int n = numIndents*NUM_INDENTS_PER_SPACE;
	if (n > LENGTH) n = LENGTH;

	return &pINDENT[LENGTH - n];
}

int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if (!pElement) return 0;

	TiXmlAttribute* pAttrib = pElement->FirstAttribute();
	int i = 0;
	int ival;
	double dval;
	const char* pIndent = getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf("%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival) == TIXML_SUCCESS)    printf(" int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS) printf(" d=%1.1f", dval);
		printf("\n");
		i++;
		pAttrib = pAttrib->Next();
	}
	return i;
}

void dump_to_stdout(TiXmlNode* pParent, unsigned int indent = 0)
{
	if (!pParent) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf("%s", getIndent(indent));
	int num;

	switch (t)
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		printf("Document");
		break;

	case TiXmlNode::TINYXML_ELEMENT:
		printf("Element [%s]", pParent->Value());
		num = dump_attribs_to_stdout(pParent->ToElement(), indent + 1);
		switch (num)
		{
		case 0:  printf(" (No attributes)"); break;
		case 1:  printf("%s1 attribute", getIndentAlt(indent)); break;
		default: printf("%s%d attributes", getIndentAlt(indent), num); break;
		}
		break;

	case TiXmlNode::TINYXML_COMMENT:
		printf("Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		printf("Unknown");
		break;

	case TiXmlNode::TINYXML_TEXT:
		pText = pParent->ToText();
		printf("Text: [%s]", pText->Value());
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		printf("Declaration");
		break;
	default:
		break;
	}
	printf("\n");
	for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
	{
		dump_to_stdout(pChild, indent + 1);
	}
}

// load the named file and dump its structure to STDOUT
void dump_to_stdout(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		printf("\n%s:\n", pFilename);
		dump_to_stdout(&doc); // defined later in the tutorial
	}
	else
	{
		printf("Failed to load file \"%s\"\n", pFilename);
	}
}


int main(int argc, _TCHAR* argv[])
{
	#define BUFSIZE MAX_PATH
	TCHAR Buffer[BUFSIZE];

	printf("This program shows how to open an existing zipfile, display information about file.\n");
	SetCurrentDirectory(_T("..\\Input"));
	HZIP hz = OpenZip(_T("input.ods"), 0);
	if (hz == 0) 
		msg(_T("* Failed to open empty.zip"));
	GetCurrentDirectory(BUFSIZE, Buffer);
	_tprintf(TEXT("Current directory is: (%s)\n"), Buffer);

	ZIPENTRY ze; 
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int i=0; i<numitems; i++)
	{ 
		GetZipItem(hz,i,&ze);
		wsprintf(mBuff, _T("Index: %i\tCompSize: %i\tOrigSize: %i\tName: %s"), ze.index, ze.comp_size, ze.unc_size, ze.name);
		msg(mBuff);
		UnzipItem(hz,i,ze.name);
	}
	//   - unzip to a membuffer -
	int i; 
	char *ibuf = NULL;

	ZRESULT myResult = FindZipItem(hz, _T("content.xml"), true, &i, &ze);

	switch (myResult)
	{
		case ZR_OK:
			//ExtractToMemory(hz, i, ze);
			ExtractToDisk(hz, i, ze);
			processXMLFile();
			dump_to_stdout("content.xml");
			break;
		case ZR_NOTFOUND:
			msg(_T("* Failed to find xcontent.xml"));
			break;
		default:
			wsprintf(mBuff, _T("Untrapped Error Code: %i"), myResult);
			msg(mBuff);
			break;
	}
	
	CloseZip(hz);

	Pause();
	return 0;
}

