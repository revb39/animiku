/************************************************************************
*	AM_Utils.cpp -- Various utilities used in AMENGINE					*
*					Copyright (c) 2014 Justin Byers						*
* ===================================================================== *
* This file is part of AniMiku.											*
*																		*
* AniMiku is free software : you can redistribute it and / or modify	*
* it under the terms of the GNU General Public License as published by	*
* the Free Software Foundation, either version 3 of the License, or		*
* (at your option) any later version.									*
*																		*
* AniMiku is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of		*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the			*
* GNU General Public License for more details.							*
*																		*
* You should have received a copy of the GNU General Public License		*
* along with AniMiku. If not, see <http://www.gnu.org/licenses/>.		*
*************************************************************************/

#include "AM_Utils.h"

char* AMUtils::UTF16ToANSI(WCHAR *win)
{
	char* ansi = NULL;
	int n = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, win, -1, NULL, 0, 0, 0);
	if (n > 1)
	{
		ansi = new char[n];
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, win, -1, ansi, n, 0, 0);
		return ansi;
	}

	return NULL;
}
char* AMUtils::UTF8ToANSI(char *cin)
{
	char* ansi = NULL;
	int n = MultiByteToWideChar(CP_UTF8, 0, cin, -1, NULL, 0);
	if (n > 0)
	{
		wchar_t *wcs = new wchar_t[n];
		MultiByteToWideChar(CP_UTF8, 0, cin, -1, wcs, n);

		ansi = UTF16ToANSI(wcs);
		delete[] wcs;
	}
	return ansi;
}
WCHAR* AMUtils::ANSIToUTF16(char* pChar)
{
	WCHAR* utf16 = NULL;
	int n = MultiByteToWideChar(CP_ACP, 0, pChar, -1, NULL, 0);
	if (n <= 0)
		return NULL;
	utf16 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, pChar, -1, utf16, n);
	return utf16;
}
char* AMUtils::ANSIToUTF8(char* pChar)
{
	WCHAR* w = ANSIToUTF16(pChar);
	int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, 0, 0);
	if (n <= 0)
		return NULL;
	char* utf8 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, w, -1, utf8, n, 0, 0);
	delete[] w;
	return utf8;
}

char* AMUtils::getDir(char* path)
{
	int i=0;
	for (i=strlen(path);i>=0;i--)
	{
		if (path[i] == '\\')
			break;
	}
	i++;
	char *d = new char[i+1];
	strncpy_s(d, i+1, path, i);

	return d;
}
char* AMUtils::getFile(char* path)
{
	int i=0;
	for (i=strlen(path);i>=0;i--)
	{
		if (path[i] == '\\')
			break;
	}
	i++;
	return path+i;
}
char* AMUtils::getExtension(char* file)
{
	int l = strlen(file);
	if (l == 0)
		return "";
	int i=l;
	for (i=l;i>=0;i--)
	{
		if (file[i] == '.')
			break;
	}
	i++;

	return file+i;
}