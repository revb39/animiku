/************************************************************************
*	AM_Utils.h -- Various utilities used in AMENGINE					*
*				  Copyright (c) 2014 Justin Byers						*
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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <Windows.h>

class AMUtils {
public:
	/*
		static char* UTF16ToANSI()

		Convert a UTF16 string to ANSI

		Parameters:			win			UTF16 string to convert
		Returns:			char*		Converted string
	*/
	static char* UTF16ToANSI(WCHAR *win);
	/*
		static char* UTF8ToANSI()

		Convert a UTF8 string to ANSI

		Parameters:			cin			UTF8 string to convert
		Returns:			char*		Converted string
	*/
	static char* UTF8ToANSI(char *cin);
	/*
		static WCHAR* ANSIToUTF16()

		Convert an ANSI string to UTF16

		Parameters:			pChar		ANSI string to convert
		Returns:			WCHAR*		Converted string
	*/
	static WCHAR* ANSIToUTF16(char* pChar);
	/*
		static WCHAR* ANSIToUTF8()

		Convert an ANSI string to UTF8

		Parameters:			pChar		ANSI string to convert
		Returns:			WCHAR*		Converted string
	*/
	static char* ANSIToUTF8(char* pChar);

	/*
		static char* getDir()

		Truncate a file path to get its directory

		Parameters:			path		File path
		Returns:			char*		Directory of the file
	*/
	static char* getDir(char* path);
	/*
		static char* getFile()

		Get filename from a ful file path

		Parameters:			path		File path
		Returns:			char*		The filename
	*/
	static char* getFile(char* path);
	/*
		static char* getExtension

		Get the extension of a file

		Parameters:			file		File to get extension from
		Returns:			char*		Extension of the file
	*/
	static char* getExtension(char* file);
};
#endif