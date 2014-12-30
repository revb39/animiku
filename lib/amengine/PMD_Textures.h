/************************************************************************
*	PMD_Textures.h -- Texture & material info							*
*					  Copyright (c) 2014 Justin Byers					*
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

#ifndef _MATERIALS_H_
#define _MATERIALS_H_

#include "AMTypes.h"

/*An RGB defined color*/
struct Color3
{
	float R, G, B;
	static Color3 Zero()
	{
		Color3 o;
		o.R = 0;
		o.G = 0;
		o.B = 0;
		return o;
	}
	static Color3 FromArray(float *arr)
	{
		Color3 o;
		o.R = arr[0];
		o.G = arr[1];
		o.B = arr[2];
		return o;
	}
};
/*Material info structure*/
struct MaterialInfo
{
	Color3 Diffuse;
	Color3 Ambient;
	Color3 Specular;
	float Alpha;
	float Shininess;
	char* TextureFile;
	char* SphereFile;
	SphereMode SphereMode;
	int ApexCount;
};

/*
	Handles data storage for material and texture information of a single material
*/
class PMD_Material {
private:
	/*The material info structure*/
	MaterialInfo _info;
	/*
		bool tailcmp()

		Check if a string s1 has the tail s2

		Parameters:		s1			The source string
						s2			The tail to compare
		Returns:		bool		True if the tail matches
	*/
	bool tailcmp(const char *s1, const char *s2);

public:
	PMD_Material();
	~PMD_Material();

	/*
		MaterialInfo* getInfo()

		Get pointer to the material info structure

		Returns:		Pointer to the material info structure
	*/
	MaterialInfo* getInfo();
	/*
		bool hasTexture()

		Get if this material has a texture

		Returns:		bool		Material has a texture
	*/
	bool hasTexture();
	/*
		bool hasSphere()

		Get if this material has a sphere map

		Returns:		bool		Material has a sphere map
	*/
	bool hasSphere();
};
#endif