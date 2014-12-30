/************************************************************************
*	AM_Extended.h -- Extended model format								*
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

#ifndef EXTENDED_H_
#define EXTENDED_H_

#include "AMTypes.h"
#include "PMD_Textures.h"

/*Single frame in the animated texture*/
struct FrameBlock
{
	unsigned long blockSize;
	char* imageData;
	ID3D10ShaderResourceView *texture;
};
/*Extended material supporting animated textures*/
struct ExtMaterial
{
	bool selfLit;
	bool animated;
	bool glow;
	bool startedAnimating;
	DWORD startTime;
	unsigned long numFrames;
	unsigned long frameDelay;
	int curFrame;
	FrameBlock *frameData;
};

/*
	Manages the parsing of the extended model format
*/
class AMExtended
{
private:
	/*List of extended materials*/
	ExtMaterial* matList;
	/*Number of extended materials*/
	int _numMaterial;
	/*Pointer to the D3D device*/
	ID3D10Device *dev;
	/*
		void loadAMT()

		Load and parse an AMT (animated texture) file

		Parameters:			mat			Pointer to the extended material structure for the material
							amtName		Filename of the AMT file
	*/
	void loadAMT(ExtMaterial *mat, char* amtName);
public:
	AMExtended();
	~AMExtended();
	/*
		void loadAndParse()

		Load and parse the extended model data in a model file

		Parameters:			model			Pointer to FILE object, must be at start of extended data
							numMaterial		Number of extended materials in the model
	*/
	void loadAndParse(FILE* model, int numMaterial, ID3D10Device *mainDev);
	/*
		ID3D10ShaderResourceView* getTexture()

		Get shader resource view for the animated texture at current time delta

		Parameters:			matIndex		Index of material to get texture from
	*/
	ID3D10ShaderResourceView* getTexture(int matIndex);

	/*
		bool getAnimated()

		Get if a material is animated

		Parameters:			matInd			Index of material to get texture from
		Returns:			bool			Material is animated
	*/
	bool getAnimated(int matInd);
	/*
		bool getSource()

		Get if a material is a self-lit material (light source)

		Parameters:			matInd			Index of material to get texture from
		Returns:			bool			Material is self-lit
	*/
	bool getSource(int matInd);
};
#endif