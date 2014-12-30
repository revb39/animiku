/************************************************************************
*	AM_TGALoader.h -- Adds missing support in D3D10 for TGA images		*
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

#ifndef _TGALOADER_H_
#define _TGALOADER_H_

#include "AMTypes.h"

#pragma pack(push, 1)
struct TGAColorMap
{
	unsigned short offset;
	unsigned short length;
	unsigned char entrySize;
};
struct TGAImageSpec
{
	unsigned short xOrigin;
	unsigned short yOrigin;
	unsigned short imageWidth;
	unsigned short imageHeight;
	unsigned char pixelDepth; //bits per pixel
	unsigned char descriptor;
};
struct TGAHeader
{
	unsigned char idLength;
	unsigned char colorMapType;
	unsigned char imageType;
	TGAColorMap colorMap;
	TGAImageSpec imageSpec;
};
#pragma pack(pop)

/*
	Load and parse TGA images into D3D shader resource views
*/
class TGALoader
{
public:
	/*
		static HRESULT CreateShaderResourceViewFromTGAFile()

		Load a TGA image and create an ID3D10ShaderResourceView

		Parameters:			pDevice					Pointer to the D3D device
							pSrcFile				Path to TGA image file
							pLoadInfo				D3D image load info
							ppShaderResourceView	Pointer to ID3D10ShaderResourceView to receive data
		Returns:			HRESULT					Result of the shader resource view creation operation
	*/
	static HRESULT CreateShaderResourceViewFromTGAFile(ID3D10Device* pDevice, LPCSTR pSrcFile, D3DX10_IMAGE_LOAD_INFO *pLoadInfo, ID3D10ShaderResourceView **ppShaderResourceView);
private:
	/*
		static ID3D10ShaderResourceView* createAndFillTexture()

		Create the ID3D10ShaderResourceView with raw pixel data

		Parameters:			pDevice						Pointer to the D3D device
							pixelData					Raw pixel data of image
							header						Pointer to the TGA header structure
		Returns				ID3D10ShaderResourceView*	New shader resource view
	*/
	static ID3D10ShaderResourceView* createAndFillTexture(ID3D10Device *pDevice, unsigned char* pixelData, TGAHeader *header);
};
#endif