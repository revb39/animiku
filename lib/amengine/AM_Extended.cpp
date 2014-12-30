/************************************************************************
*	AM_Extended.cpp -- Extended model format							*
*					   Copyright (c) 2014 Justin Byers					*
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

#include "AM_Extended.h"
AMExtended::AMExtended()
{
	matList = NULL;
	_numMaterial = 0;
	dev = NULL;
}
AMExtended::~AMExtended()
{
	if (matList)
	{
		for (int i=0;i<_numMaterial;i++)
		{
			if (matList[i].frameData)
			{
				for (int j=0;j<(int)matList[i].numFrames;j++)
				{
					if (matList[i].frameData[j].imageData)
						delete[] matList[i].frameData[j].imageData;
				}
				delete[] matList[i].frameData;
			}
		}
		delete[] matList;
	}
}
void AMExtended::loadAMT(ExtMaterial *mat, char* amtName)
{
	//open the AMT file
	FILE* amt;
	fopen_s(&amt, amtName, "rb");
	
	//get the frame delay
	fread(&(mat->frameDelay), sizeof(unsigned long), 1, amt);
	
	//get the number of frames
	unsigned long nf;
	fread(&nf, sizeof(unsigned long), 1, amt);
	
	//load each frame image 
	for (int f=0;f<(int)nf;f++)
	{
		//get the size of this frame's image and allocate the data array
		fread(&(mat->frameData[f].blockSize), sizeof(unsigned long), 1, amt);
		mat->frameData[f].imageData = new char[mat->frameData[f].blockSize];

		//read the image data to the data array
		fread(mat->frameData[f].imageData, mat->frameData[f].blockSize, 1, amt);

		//load the image data into direct3d
		mat->frameData[f].texture = NULL;
		D3DX10_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO));
		loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_BC1_UNORM;
		D3DX10CreateShaderResourceViewFromMemory(dev, mat->frameData[f].imageData, mat->frameData[f].blockSize, &loadInfo, NULL, &(mat->frameData[f].texture), NULL);
	}
	fclose(amt);
}
void AMExtended::loadAndParse(FILE* model, int numMaterial, ID3D10Device *mainDev)
{
	//assign material count and D3D device
	_numMaterial = numMaterial;
	dev = mainDev;

	//allocate memory for and initialize each material
	matList = new ExtMaterial[numMaterial];
	for (int i=0;i<numMaterial;i++)
	{
		matList[i].selfLit = false;
		matList[i].animated = false;
		matList[i].glow = false;
		matList[i].startedAnimating = false;

		matList[i].numFrames = 0;
		matList[i].frameData = NULL;

		matList[i].frameDelay = 0;
	}
	//don't do anything if already at end of file
	if (feof(model))
		return;

	char flags[3];
	char amtName[100];
	//parse each material in extended model data
	for (int i=0;i<numMaterial;i++)
	{
		//get flags
		fread(flags, 3, 1, model);
		matList[i].selfLit = flags[0] ? true : false;
		matList[i].animated = flags[1] ? true : false;
		matList[i].glow = flags[2] ? true : false;

		//load the frames if this material is animated
		if (matList[i].animated)
		{
			//get number of frames in animation & allocate memory for frame blocks
			fread(&(matList[i].numFrames), sizeof(unsigned long), 1, model);
			matList[i].frameData = new FrameBlock[matList[i].numFrames];
			//initialize each frame block
			for (int f=0;f<(int)matList[i].numFrames;f++)
			{
				matList[i].frameData[f].imageData = NULL;
				matList[i].frameData[f].texture = NULL;
			}

			//get filename of the amt
			fread(amtName, 100, 1, model);
			//load the AMT
			loadAMT(&(matList[i]), amtName);

			//skip original texture file paths (part of AMT spec, not used in renderer)
			fseek(model, 256*matList[i].numFrames, SEEK_CUR);
		}
	}
}
ID3D10ShaderResourceView* AMExtended::getTexture(int matIndex)
{
	//update animation if already started animating
	if (matList[matIndex].startedAnimating)
	{
		//calculate time delta from last update
		DWORD deltaTime = timeGetTime()-matList[matIndex].startTime;

		//if delta >= frame delay move to next frame
		if (deltaTime >= matList[matIndex].frameDelay)
		{
			matList[matIndex].curFrame++;
			//loop to first frame if hit last frame
			if (matList[matIndex].curFrame >= (int)matList[matIndex].numFrames)
				matList[matIndex].curFrame = 0;
			//update start time to current time
			matList[matIndex].startTime = timeGetTime();			
		}
		//return the texture
		return matList[matIndex].frameData[matList[matIndex].curFrame].texture;
	}
	else
	{
		//otherwise start the animation at frame 0
		matList[matIndex].startTime = timeGetTime();
		matList[matIndex].curFrame = 0;
		matList[matIndex].startedAnimating = true;
		return matList[matIndex].frameData[0].texture;
	}
}
bool AMExtended::getAnimated(int matInd)
{
	if (matList == NULL)
		return false;

	return matList[matInd].animated;
}
bool AMExtended::getSource(int matInd)
{
	if (matList == NULL)
		return false;

	return matList[matInd].selfLit;
}