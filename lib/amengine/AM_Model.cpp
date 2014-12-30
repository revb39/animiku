/************************************************************************
*	AM_Model.cpp -- Model management									*
*					Copyright (c) 2014 Justin Byers						*
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

#include "AM_Model.h"
#include "AM_Settings.h"
#include "AM_TGALoader.h"

AM_Model::AM_Model()
{
	//initialize the transforms
	D3DXMatrixTranslation(&matTranslate, 0, 0, 0);
	D3DXMatrixRotationX(&matRotateX, 0);
	D3DXMatrixRotationY(&matRotateY, 0);
	D3DXMatrixRotationZ(&matRotateZ, 0);

	//initialize D3D stuff
	dev = NULL;
	v_buffer = NULL;
	i_buffer = NULL;
	textures = NULL;
	spheres = NULL;
	texResViews = NULL;
	sphereResViews = NULL;
	world = NULL;
	palette = NULL;
	amb = NULL;
	dif = NULL;
	spec = NULL;
	shine = NULL;
	alpha = NULL;
	tex = NULL;
	sph = NULL;
	toon = NULL;
	effect = NULL;
	effectTech = NULL;

	//initialize the raw parsing data
	_nameJ = NULL;
	_nameE = NULL;
	_commentJ = NULL;
	_commentE = NULL;
	_numMaterial = 0;
	_material = NULL;
	_numMorph = 0;
	_morphList = NULL;
	_baseMorph = NULL;
	numFaceVertex = 0;
	faceVertexIndices = NULL;
	_numRigidBody = 0;
	_rigidBodyList = NULL;
	_pPhysics = NULL;
	_numConstraint = 0;
	_constraintList = NULL;

	//initialize the rest of the stuff
	assignedMotion = NULL;
	alphaFade = false;
	useTex = NULL;
	useToon = NULL;
	sphType = NULL;
	boneMatrixPalette2 = NULL;
	cVertices = NULL;
}
AM_Model::~AM_Model()
{
	if (v_buffer)
		v_buffer->Release();
	if (i_buffer)
		i_buffer->Release();
	if (textures)
		textures->Release();
	if (spheres)
		spheres->Release();
	if (boneMatrixPalette2)
		delete[] boneMatrixPalette2;

	if (texResViews)
	{
		for(int i = 0;i<(sizeof(texResViews)/sizeof(ID3D10ShaderResourceView*));i++)
		{
			if (texResViews[i])
				texResViews[i]->Release();
		}
		delete[] texResViews;
	}
	if (sphereResViews)
	{
		for(int i = 0;i<(sizeof(sphereResViews)/sizeof(ID3D10ShaderResourceView*));i++)
		{
			if (sphereResViews[i])
				sphereResViews[i]->Release();
		}
		delete[] sphereResViews;
	}
}

bool AM_Model::load(char *file, ID3D10Device *mainDev, BulletPhysics *physics)
{
	char dir[MAX_PATH];
	char ind[4];
	fpos_t sz;
	bool ext;
	bool r;
	unsigned long start;

	//get the directory of the model file
	strcpy_s(dir, sizeof(dir), file);
	while (dir[strlen(dir)-1] != '\\' && strlen(dir) != 0)
	{
		dir[strlen(dir)-1] = '\0';
	}
	if (strlen(dir) == 0)
		return false;
	//set working directory to the directory the model resides in
	SetCurrentDirectory(dir);

	//assign Bullet and D3D device pointers
	_pPhysics = physics;
	dev = mainDev;

	//attempt to open the file for binary read
	f = NULL;
	fopen_s(&f, file, "rb");
	if (f == NULL)
		return false;

	//get size of file
	fseek(f, 0, SEEK_END);
	fgetpos(f, &sz);

	//check to see if AM extended format is appended
	fseek(f, -3, SEEK_END);
	fread(ind, 3, 1, f);
	ind[3] = '\0';
	ext = false;
	start = (unsigned long)sz;
	if (!strcmp(ind, "END"))
	{
		//found key, does have extended format
		ext = true;
		fseek(f, -7, SEEK_END);
		fread(&start, sizeof(unsigned long), 1, f);
	}
	//update size to not include extended model data & seek to start of file
	size = (size_t) start;
	fseek(f, 0, SEEK_SET);
	//attempt to parse the data
	r =  parse(file, dir, ext);
	fclose(f);

	return r;
}

bool AM_Model::parse(char *file, char *dir, bool ext)
{
	//nope, this needs to be implemented by the inheriting class
	return false;
}

void AM_Model::prepareD3D()
{
	boneMatrixPalette2 = new D3DXMATRIX[_numBone];
	texResViews = new ID3D10ShaderResourceView*[_numMaterial];
	sphereResViews = new ID3D10ShaderResourceView*[_numMaterial];
	ZeroMemory(texResViews, sizeof(texResViews));
	ZeroMemory(sphereResViews, sizeof(sphereResViews));
}

void AM_Model::updateDevice(ID3D10Device *dev, ID3D10Effect *effect, ID3D10EffectTechnique *t)
{
	D3D10_BUFFER_DESC bd;
	D3D10_SUBRESOURCE_DATA initData;
	D3DX10_IMAGE_LOAD_INFO loadInfo;
	char* n;
	bool alreadyLoaded;
	MaterialInfo *ci;

	//vertex buffer description
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(MMDVERTEX) * _numVertex;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	//vertex buffer data
	initData.pSysMem = cVertices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&bd, &initData, &v_buffer); //create the vertex buffer

	//index buffer description
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned short) * _numSurface;
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = _surfaceList;
	dev->CreateBuffer(&bd, &initData, &i_buffer); //create the index buffer

	//texture load info (for textures and sphere maps)
	ZeroMemory(&loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO));
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	loadInfo.Format = DXGI_FORMAT_BC1_UNORM;

	//load all textures and prepare them for D3D
	for (int i=0;i<(int)_numMaterial;i++)
	{
		alreadyLoaded = false;
		ci = _material[i].getInfo();

		HRESULT h;
		if (_material[i].hasTexture())
		{
			//if the material has a texture with it
			n = ci->TextureFile; //get the texture's name
			if (i != 0)
			{
				//only run this check if this is not the first material
				for (int j=i-1; j>=0;j--)
				{
					//check to see if this texture was already loaded
					if (_material[j].hasTexture() && !strcmp(n, _material[j].getInfo()->TextureFile))
					{
						//if it was already loaded, copy the memory address
						//this saves memory usage
						texResViews[i] = texResViews[j];
						alreadyLoaded = true;
						break;
					}
				}
			}
			if (!alreadyLoaded)
			{
				//D3D doesn't support loading TGA natively
				//if TGA texture, use our own TGA parser to prepare the texture for D3D
				std::string ext = std::string(ci->TextureFile).substr(std::string(ci->TextureFile).find_last_of('.')+1);
				if (!strcmp(ext.c_str(), "tga"))
					h = TGALoader::CreateShaderResourceViewFromTGAFile(dev, ci->TextureFile, &loadInfo, &(texResViews[i]));
				else
					h = D3DX10CreateShaderResourceViewFromFile(dev, ci->TextureFile, &loadInfo, NULL, &(texResViews[i]), NULL);
				if (FAILED(h))
				{
					printf("load %s failed\n", ci->TextureFile);
					texResViews[i] = NULL;
				}
			}
			alreadyLoaded = false;
		}
		if (_material[i].hasSphere())
		{
			//if the material has a sphere map with it
			n = ci->SphereFile; //get the sphere map's name
			if (i != 0)
			{
				//only run this check if this is not the first material
				for (int j=i-1; j>=0;j--)
				{
					//check to see if this sphere map texture was already loaded
					if (_material[j].hasSphere() && !strcmp(n, _material[j].getInfo()->SphereFile))
					{
						//if it was already loaded, copy the memory address
						//this saves memory usage
						sphereResViews[i] = sphereResViews[j];
						alreadyLoaded = true;
						break;
					}
				}
			}
			if (!alreadyLoaded)
			{
				//create the shader resource view
				h = D3DX10CreateShaderResourceViewFromFile(dev,ci->SphereFile, &loadInfo, NULL, &(sphereResViews[i]), NULL);
				if (FAILED(h))
					sphereResViews[i] = NULL;
			}
		}
	}

	
	this->dev = dev;
	this->effect = effect;
	this->effectTech = t;

	//get pointers to variables in shader
	amb = effect->GetVariableByName("materialAmbient")->AsVector();
	dif = effect->GetVariableByName("materialDiffuse")->AsVector();
	spec = effect->GetVariableByName("materialSpecular")->AsVector();
	shine = effect->GetVariableByName("shine")->AsScalar();
	alpha = effect->GetVariableByName("alpha")->AsScalar();
	useTex = effect->GetVariableByName("useTexture")->AsScalar();
	useToon = effect->GetVariableByName("useToon")->AsScalar();
	sphType = effect->GetVariableByName("sphereType")->AsScalar();
	tex = effect->GetVariableByName("texDiffuse")->AsShaderResource();
	sph = effect->GetVariableByName("sphTexture")->AsShaderResource();
	toon = effect->GetVariableByName("toonTexture")->AsShaderResource();
	palette = effect->GetVariableByName("MatrixPalette")->AsMatrix();
	world = effect->GetVariableByName("worldMatrix")->AsMatrix();
	lightsOff = effect->GetVariableByName("lightsOff")->AsScalar();
	extraBright = effect->GetVariableByName("extraBright")->AsScalar();
	fadeCoef = effect->GetVariableByName("fadeCoef")->AsScalar();

	//set up D3D to use the new vertex & index buffers
	UINT stride = sizeof(MMDVERTEX);
	UINT offset = 0;
	dev->IASetVertexBuffers(0, 1, &v_buffer, &stride, &offset);
	dev->IASetIndexBuffer(i_buffer, DXGI_FORMAT_R16_UINT, 0);
	dev->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void AM_Model::translate(float x, float y, float z)
{
	D3DXMatrixTranslation(&matTranslate, x, y, z);
}
void AM_Model::rotate (float x, float y, float z)
{
	D3DXMatrixRotationX(&matRotateX, x);
	D3DXMatrixRotationY(&matRotateY, y);
	D3DXMatrixRotationZ(&matRotateZ, z);
}
void AM_Model::updateMotion()
{
	if (assignedMotion->getIsPlaying())
	{
		//update everything from the motion, but only if it is playing
		assignedMotion->update();
		boneController.updateBonesFromMotion(assignedMotion, false);
		updateMorphVertices(false);
		boneController.updateTransformsFromRoot(false);
		boneController.solveIKs();
	}
}

void AM_Model::render(AMSettings *sets, ID3D10EffectTechnique *t)
{
	int finSurfaces;
	MaterialInfo *ci;
	D3DXVECTOR4 d;
	D3DXVECTOR3 s;
	D3DXVECTOR3 a;

	//update from the motion
	if (hasMotionStarted())
		updateMotion();
		
	//update all rigid bodies
	updateFromPhysics();

	//rebuild the bone matrix palette with the new bone positions
	//these are used to skin vertices in the shader
	for (int i=0;i<(int)_numBone;i++)
	{
		boneMatrixPalette2[i] = boneController.getBoneList()[i].getSkinningTrans();
	}
	palette->SetMatrixArray((float*)&(boneMatrixPalette2[0]), 0, _numBone);

	//update the world transform matrix in the shader
	world->SetMatrix((float*)&(matRotateX*matRotateY*matRotateZ*matTranslate));

	//render each material separately
	finSurfaces = 0;
	for (int i=0;i<(int)_numMaterial;i++)
	{
		ci = _material[i].getInfo();

		//get diffuse color from the material & blend with fader
		//if material is self-lit, do not apply fader
		d = D3DXVECTOR4(ci->Diffuse.R, ci->Diffuse.G, ci->Diffuse.B, 1);
		if (!alphaFade && !extendedData.getSource(i))
			d *= fader.GetCurAlphaScalar();
		d.w = ci->Alpha;
		if (alphaFade && !extendedData.getSource(i))
			d.w *= fader.GetCurAlphaScalar();
		//get ambient color from the material & blend with fader
		a = D3DXVECTOR3(ci->Ambient.R, ci->Ambient.G, ci->Ambient.B);
		if (!alphaFade && !extendedData.getSource(i))
			a *= fader.GetCurAlphaScalar();
		//get specular color from the material & blend with fader
		s = D3DXVECTOR3(ci->Specular.R, ci->Specular.G, ci->Specular.B);
		if (!alphaFade && !extendedData.getSource(i))
			s *= fader.GetCurAlphaScalar();

		if (extendedData.getSource(i))
		{
			//if the extended data says this is material is a light source
			//update the brightness coefficient in the shader
			extraBright->SetFloat(sets->getExtraBright());
		}
		else
		{
			//otherwise, set brightness coefficient to 1 (multiplying by 1 does nothing)
			extraBright->SetFloat(1.0f);

			//since this is not a light source, it must be affected by the fader
			//update the fader coefficient to the current coefficient from the fader
			fadeCoef->SetFloat(fader.GetCurAlphaScalar());
		}

		//update all lighting params in shader
		dif->SetFloatVector((float*)&d);
		amb->SetFloatVector((float*)&a);
		spec->SetFloatVector((float*)&s);
		shine->SetFloat(ci->Shininess);

		//not using toon shading
		useToon->SetBool(false);
		toon->SetResource(NULL);

		//tell the shader if material has a texture
		useTex->SetBool((_material[i].hasTexture() && texResViews[i] != NULL));
		if (extendedData.getAnimated(i))
		{
			//if it is animated, get the current texture from the extended data
			//calculated automatically based on time and settings in extended data
			useTex->SetBool(true);
			tex->SetResource(extendedData.getTexture(i));
		}
		else
		{
			//not animated, so we get the static texture from the material
			//if the material has a texture and it was sucessfully loaded into memory
			if (_material[i].hasTexture() && texResViews[i] != NULL)
				tex->SetResource(texResViews[i]);
		}

		//tell the shader what sphere mapping technique to use
		sphType->SetInt(ci->SphereMode);

		//get the sphere map and pass it to the shader
		//if the material has a sphere map and it was sucessfully loaded into memory
		if (_material[i].hasSphere() && sphereResViews[i] != NULL)
			sph->SetResource(sphereResViews[i]);

		//lights off mode not implemented yet
		lightsOff->SetBool(false);

		//update the shader with all the new params we passed to it
		t->GetPassByIndex(0)->Apply(0);

		//make sure D3D is set to use the vertex and index buffer
		UINT stride = sizeof(MMDVERTEX);
		UINT offset = 0;
		dev->IASetVertexBuffers(0, 1, &v_buffer, &stride, &offset);
		dev->IASetIndexBuffer(i_buffer, DXGI_FORMAT_R16_UINT, 0);
		dev->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//draw only the number of indices in this material
		dev->DrawIndexed(ci->ApexCount, finSurfaces, 0);
		finSurfaces += ci->ApexCount;
	}
}

void AM_Model::startMotion()
{
	if (assignedMotion != NULL)
		assignedMotion->start();
}
void AM_Model::stopMotion()
{
	if (assignedMotion != NULL)
		assignedMotion->stop();
}
void AM_Model::assignMotion(AM_Motion *motion)
{
	assignedMotion = motion;
}

bool AM_Model::hasMotionStarted()
{
	return assignedMotion != NULL && assignedMotion->getIsPlaying();
}

void AM_Model::updateMorphVertices(bool isDefPos)
{
	if (assignedMotion)
	{
		MorphInfo* ci;
		//calculate all the group morphs first so all morphs effected are up to date
		for (int i=0;i<(int)_numMorph;i++)
		{
			ci = _morphList[i].getInfo();
			float weight = 0;

			if (ci->Type == MorphDefs::Group)
			{
				//get bind pose weight or current pose weight
				if (isDefPos)
					assignedMotion->getDefFace(ci->Name, i, &weight);
				else
					assignedMotion->getWeightAtTimeFromFace(ci->Name, i, &weight);
				//calculate the group morph based on weight
				_morphList[i].calculate(weight);
			}
			else
			{
				//otherwise clear the transform 
				_morphList[i].clearVertTransforms();
			}
		}
		for (int i=0;i<(int)_numMorph;i++)
		{
			ci = _morphList[i].getInfo();
			float weight = 0;
			//get bind pose weight or current pose weight
			if (isDefPos)
				assignedMotion->getDefFace(ci->Name, i, &weight);
			else
				assignedMotion->getWeightAtTimeFromFace(ci->Name, i, &weight);
			//calculate the morph based on weight
			_morphList[i].calculate(weight);
		}

		//map the vertex buffer so we can edit it and write the new vertices to it
		void* pVoid;
		v_buffer->Map(D3D10_MAP_WRITE_NO_OVERWRITE, 0, &pVoid);
		//copy the vertex buffer in
		memcpy(pVoid, cVertices, sizeof(cVertices[0])*_numVertex);
		v_buffer->Unmap();
	}
}

void AM_Model::setPhysicsEnabled(bool e)
{
	for (int i = 0; i<(int)_numConstraint; i++)
	{
		_constraintList[i].setEnabled(e);
	}
	for (int i=0;i<(int)_numRigidBody;i++)
	{
		_rigidBodyList[i].setEnabled(e);
	}
}
void AM_Model::updateFromPhysics()
{
	//update all rigid bodies and set their bones' transforms
	for (int i=0;i<(int)_numRigidBody;i++)
	{
		_rigidBodyList[i].applyTransformToBone();
	}
}

AMFader* AM_Model::getFader()
{
    return &fader;
}

void AM_Model::moveToDefMotionPos()
{
	if (!assignedMotion)
		return;
	//update everything, but tell them to pull data of bind pose
	boneController.updateBonesFromMotion(assignedMotion, true);
	updateMorphVertices(true);
	boneController.updateTransformsFromRoot(true);
	boneController.solveIKs();
}
void AM_Model::resetPhysics()
{
	//force all rigid bodies to move to the transforms of their bones
	for (int i=0;i<(int)_numRigidBody;i++)
	{
		_rigidBodyList[i].updatePosition();
	}
}
void AM_Model::setAlphaFade(bool t)
{
	alphaFade = t;
}

AM_Model* AM_Model::CreateFromType(char* type)
{
	//create an AM_Model based on the file types
	if (ModelParsers().find(type) == ModelParsers().end())
		return NULL;
	else
		return ((AM_Model*(*)(void))ModelParsers()[type].Constructor)();
}