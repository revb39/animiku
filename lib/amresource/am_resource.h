/************************************************************************
*	AM_Resource.h -- App resource management							*
*					 Copyright (c) 2014 Justin Byers					*
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

#ifndef RESOURCE_H_
#define RESOURCE_H_

class QString;
class QFile;
class QByteArray;

/*
	Handles loading and reading Qt app resources
*/
class AMResource
{
public:
	AMResource(char* res);
	~AMResource();

	/*
		bool Open()

		Open the resource

		Returns:		bool		Sucessfully opened
	*/
	bool Open();
	/*
		void Close()

		Close the resource
	*/
	void Close();
	/*
		void Read()

		Read the resource data
	*/
	void Read();
	/*
		void Free()

		Free the resource
	*/
	void Free();
	/*
		size_t GetSize()

		Get the size in bytes of the resource data

		Returns:		size_t		Byte size of the resource data
	*/
	size_t GetSize();
	/*
		char* GetData()

		Get the raw data from the resource

		Returns:		char*		Raw resource data
	*/
	char* GetData();
	
private:
	/*Path of the resource*/
	QString *m_path;
	/*File handler for the resource*/
	QFile *m_file;
	/*Byte array data read from resource*/
	QByteArray *m_ba;
};

#endif
