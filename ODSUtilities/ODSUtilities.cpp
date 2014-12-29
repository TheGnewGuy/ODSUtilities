// ODSUtilities.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "unzip.h"
#include "zip.h"

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

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR mBuff[1024];		// Buffer for messages

	printf("This program shows how to open an existing zipfile, display information about file.\n");
	SetCurrentDirectory(_T("..\\Input"));
	HZIP hz = OpenZip(_T("input.ods"), 0);
	if (hz == 0) msg(_T("* Failed to open empty.zip"));
	ZIPENTRY ze; 
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int i=0; i<numitems; i++)
	{ 
		GetZipItem(hz,i,&ze);
		wsprintf(mBuff, _T("Index: %i\tCompSize: %i\tOrigSize: %i\tName: %s"), ze.index, ze.comp_size, ze.unc_size, ze.name);
		msg(mBuff);
		//UnzipItem(hz,i,ze.name);
	}
	CloseZip(hz);

	Pause();
	return 0;
}

