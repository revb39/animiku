/************************************************************************
*	AM_PMX.h -- File parsing for PMX format								*
*				Copyright (c) 2014 Justin Byers							*
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

#ifndef _PMX_H_
#define _PMX_H_

#include "AMTypes.h"
#include "AM_Model.h"
#include "AM_IK.h"

/*Encoding type used in this file*/
typedef enum Encoding{UTF16=0,UTF8};

/*
	Extends AM_Model to parse PMX files
*/
class AM_Pmx : public AM_Model {
private:
	/*Encoding used in this PMX file*/
	Encoding _enc;
	/*Honestly no idea what this is*/
	int _uvCount;
	/*Number of bytes to read indices*/
	int _indexSize;
	/*Number of bytes to read for texture IDs*/
	int _texIdSize;
	/*Number of bytes to read for material IDs*/
	int _matIdSize;
	/*Number of bytes to read for bone IDs*/
	int _boneIdSize;
	/*Number of bytes to read for morph IDs*/
	int _faceIdSize;
	/*Number of bytes to read for rigid body IDs*/
	int _rigidBodyIdSize;
	/*Number of textures in the model*/
	unsigned long _numTextures;
	/*Array of texture filenames*/
	char** _textureList;
	/*Array of IK chains in the model*/
	IK *ikl;

	/*
		void getString()

		Read a string from the file data at the current location

		Parameters:		str			Pointer to the char array to receive the string
	*/
	void getString(char** str);
	/*
		void getVertexBoneLinks()

		Get the bone links and weights for skinning of a vertex

		Parameters:		v			Index of vertex to get links and weights for
	*/
	void getVertexBoneLinks(int v);
	/*
		int indexConverter()

		Read an index from the file at current position

		Parameters:		nb			Size in bytes to read
		Returns:		int			Index read, or -1 if failed to read
	*/
	int indexConverter(unsigned int nb);
	/*
		char* UTF16ToANSI()

		Convert a UTF16 string to ANSI

		Parameters:		pChar		Pointer to UTF16 string to convert
		Returns:		char*		Converted ANSI string
	*/
	char* UTF16ToANSI(WCHAR *pChar);
	/*
		char* UTF8ToANSI()

		Convert a UTF8 string to ANSI

		Parameters:		pChar		Pointer to UTF8 string to convert
		Returns:		char*		Converted ANSI string
	*/
	char* UTF8ToANSI(char *pChar);

	/*
		virtual bool parse()

		See definition in AM_Model
	*/
	virtual bool parse(char *file, char *dir, bool ext);

public:
	AM_Pmx();
	~AM_Pmx();
};
#endif