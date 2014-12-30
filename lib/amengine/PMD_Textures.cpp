/************************************************************************
*	PMD_Textures.cpp -- Texture & material info							*
*						Copyright (c) 2014 Justin Byers					*
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

#include "PMD_Textures.h"
#include <stdio.h>
#include <iostream>

PMD_Material::PMD_Material()
{
	ZeroMemory(&_info, sizeof(MaterialInfo));
	_info.Ambient.R = 1;
	_info.Ambient.G = 1;
	_info.Ambient.B = 1;
	_info.Diffuse.R = 1;
	_info.Diffuse.G = 1;
	_info.Diffuse.B = 1;
	_info.Specular.R = 1;
	_info.Specular.G = 1;
	_info.Specular.B = 1;
	_info.Alpha = 1;
	_info.ApexCount = 0;
	_info.Shininess = 0;
	_info.TextureFile = NULL;
	_info.SphereFile = NULL;
}
PMD_Material::~PMD_Material()
{
	
}
bool PMD_Material::tailcmp(const char *s1, const char *s2)
{
	if (strcmp(&s1[strlen(s1)-strlen(s2)], s2) == 0)
		return true;
	else
		return false;
}
MaterialInfo* PMD_Material::getInfo()
{
	return &_info;
}

bool PMD_Material::hasTexture()
{
	return _info.TextureFile != NULL;
}
bool PMD_Material::hasSphere()
{
	return _info.SphereMode != NoSphere && _info.SphereFile != NULL;
}