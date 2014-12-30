/************************************************************************
*	AM_TGALoader.cpp -- Adds missing support in D3D10 for TGA images	*
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

#include "AM_TGALoader.h"

HRESULT TGALoader::CreateShaderResourceViewFromTGAFile(ID3D10Device* pDevice, LPCSTR pSrcFile, D3DX10_IMAGE_LOAD_INFO *pLoadInfo, ID3D10ShaderResourceView **ppShaderResourceView)
{
	FILE *pSrc = NULL;
	size_t sz = 0;
	unsigned char *tmpBuf = NULL;
	unsigned char *imgBuf = NULL;
	TGAHeader header;

	//open the file
	fopen_s(&pSrc, pSrcFile, "rb");
	if (pSrc == NULL)
		return D3D10_ERROR_FILE_NOT_FOUND;
	//get size of the file
	fpos_t p;
	fseek(pSrc, 0, SEEK_END);
	fgetpos(pSrc, &p);
	fseek(pSrc, 0, SEEK_SET);
	sz = (size_t)p;
	
	//get the header
	if (fread_s(&header, sizeof(TGAHeader), 1, sizeof(TGAHeader), pSrc) < sizeof(TGAHeader))
		return E_INVALIDARG;

	//get image ID
	tmpBuf = new unsigned char[header.idLength];
	if (fread_s(tmpBuf, header.idLength, 1, header.idLength, pSrc) < header.idLength)
		return E_INVALIDARG;
	delete[] tmpBuf;
	tmpBuf = NULL;

	//ignore the color map, since true color TGAs don't use it

	//check for valid information
	//
	//only support for true-color images
	if (header.imageType != 2 && header.imageType != 10)
		return E_INVALIDARG;
	//other stuff
	if (header.imageSpec.imageWidth <= 0 || header.imageSpec.imageHeight <= 0 || (header.imageSpec.pixelDepth != 24 && header.imageSpec.pixelDepth != 32))
		return E_INVALIDARG;

	//calculate bytes per pixel and image memory size
	unsigned int bytesPerPixel = header.imageSpec.pixelDepth/8;
	unsigned int imageSize = bytesPerPixel*header.imageSpec.imageWidth*header.imageSpec.imageHeight;

	//allocate the image data buffer
	imgBuf = new unsigned char[imageSize];

	if (header.imageType == 2) //uncompressed
	{
		//read the image data into the buffer
		if (fread_s(imgBuf, imageSize, 1, imageSize, pSrc) < imageSize)
			return E_INVALIDARG;

		//flip BGR to RGB color spec
		for (int i=0;i<(int)imageSize;i+=bytesPerPixel)
		{
			imgBuf[i] ^= imgBuf[i+2] ^= imgBuf[i] ^= imgBuf[i+2];
		}
	}
	else //RLE compressed
	{
		unsigned int pixelCount = header.imageSpec.imageHeight*header.imageSpec.imageWidth;
		unsigned int curPixel = 0;
		unsigned int curByte = 0;
		unsigned char *colorBuffer = new unsigned char[bytesPerPixel];

		do
		{
			unsigned char chunkHeader = 0;
			if (fread_s(&chunkHeader, sizeof(unsigned char), 1, 1, pSrc) < 1) //read the chunk's header
				return E_INVALIDARG;

			if (chunkHeader < 128) //chunk is a raw header
			{
				chunkHeader++;
				//pixel reading loop
				for (short c=0;c<chunkHeader;c++)
				{
					if (fread_s(colorBuffer, bytesPerPixel, 1, bytesPerPixel, pSrc) < bytesPerPixel) //try to read 1 pixel
						return E_INVALIDARG;

					//swap BGR to RGB
					imgBuf[curByte] = colorBuffer[2]; //R
					imgBuf[curByte+1] = colorBuffer[1]; //G
					imgBuf[curByte+2] = colorBuffer[0]; //B
					if (bytesPerPixel == 4)
						imgBuf[curByte+3] = colorBuffer[3]; //write the alpha channel if this is a 32bit image

					//increment the current byte by bytes in a pixel
					curByte += bytesPerPixel;
					curPixel++;
				}
			}
			else //chunk is an RLE header
			{
				chunkHeader -= 127; //remove the id bit

				//try to read the next pixel
				if (fread_s(colorBuffer, bytesPerPixel, 1, bytesPerPixel, pSrc) < bytesPerPixel)
					return E_INVALIDARG;

				//duplicate the pixel a certain number of times
				//dictated by the chunk header
				for (short c=0;c<chunkHeader;c++)
				{
					//swap BGR to RGB
					imgBuf[curByte] = colorBuffer[2]; //R
					imgBuf[curByte+1] = colorBuffer[1]; //G
					imgBuf[curByte+2] = colorBuffer[0]; //B
					if (bytesPerPixel == 4)
						imgBuf[curByte+3] = colorBuffer[3]; //write the alpha channel if this is a 32bit image

					//increment the current byte by bytes in a pixel
					curByte += bytesPerPixel;
					curPixel++;
				}
			}
		}
		while (curPixel < pixelCount);

		delete[] colorBuffer;
	}
	fclose(pSrc);

	*ppShaderResourceView = createAndFillTexture(pDevice, imgBuf, &header);

	delete[] imgBuf;

	return S_OK;
}
ID3D10ShaderResourceView* TGALoader::createAndFillTexture(ID3D10Device *pDevice, unsigned char* pixelData, TGAHeader *header)
{
	HRESULT hr;
	ID3D10Texture2D *pTex = NULL; //the texture to hold the TGA pixel data
	ID3D10ShaderResourceView *pShaderResView = NULL;

	//create the texture
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width = header->imageSpec.imageWidth;
	desc.Height = header->imageSpec.imageHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	hr = pDevice->CreateTexture2D( &desc, NULL, &pTex );
	if (FAILED(hr))
		printf("FAIL! 1\n");

	// Create the shader-resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView( pTex, &srDesc, &pShaderResView );
	if (FAILED(hr))
		printf("FAIL! 2\n");

	//map the texture so we can fill it with the pixels from the TGA
	D3D10_MAPPED_TEXTURE2D mappedTex;
	hr = pTex->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );
	if (FAILED(hr))
		printf("FAIL! 3\n");
	unsigned char* pTexBuf = (unsigned char*)mappedTex.pData;
	unsigned int tgaBytesPerPixel = header->imageSpec.pixelDepth/8;
	unsigned int tgaRowPitch = header->imageSpec.imageWidth*tgaBytesPerPixel;

	//loop througheach pixel, and fill with the value from the TGA
	for (unsigned int r=0;r<header->imageSpec.imageHeight;r++)
	{
		unsigned int rs = r*mappedTex.RowPitch;
		for (unsigned int c=0;c<header->imageSpec.imageWidth;c++)
		{
			int tgaCurRow = header->imageSpec.imageHeight-r-1;
			int tgarowfirstpixel = tgaCurRow*tgaRowPitch;

			unsigned int cs = c*4;
			unsigned int cstga = c*tgaBytesPerPixel;
			pTexBuf[rs+cs+0] = pixelData[tgarowfirstpixel+cstga+0];
			pTexBuf[rs+cs+1] = pixelData[tgarowfirstpixel+cstga+1];
			pTexBuf[rs+cs+2] = pixelData[tgarowfirstpixel+cstga+2];
			if (header->imageSpec.pixelDepth == 32)
			{			
				pTexBuf[rs+cs+3] = pixelData[tgarowfirstpixel+cstga+3]; //write the alpha channel if this is a 32bit tga image
			}
			else
			{
				pTexBuf[rs+cs+3] = 255;
			}
		}
	}

	//unmap the texture
	pTex->Unmap(D3D10CalcSubresource(0, 0, 1));

	return pShaderResView;
}