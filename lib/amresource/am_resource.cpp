/************************************************************************
*	AM_Resource.cpp -- App resource management							*
*					   Copyright (c) 2014 Justin Byers					*
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

#include "am_resource.h"
#include <QString>
#include <QFile>

AMResource::AMResource(char* file)
{
	m_path = new QString(file);
	m_file = 0;
	m_ba = 0;
}

AMResource::~AMResource()
{
	Free();
}

bool AMResource::Open()
{
	m_file = new QFile(*m_path);
	return m_file->open(QFile::ReadOnly);
}

void AMResource::Close()
{
	m_file->close();
}

void AMResource::Read()
{
	m_ba = new QByteArray(m_file->readAll());
}

void AMResource::Free()
{
	if (m_ba)
		delete m_ba;
	if (m_path)
		delete m_path;
	if (m_file)
		delete m_file;

	m_ba = 0;
	m_path = 0;
	m_file = 0;
}

size_t AMResource::GetSize()
{
	return m_ba->size();
}

char* AMResource::GetData()
{
	return m_ba->data();
}