/************************************************************************
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
*																		*
* AniMiku is Copyright (c) 2014 Justin Byers							*
*************************************************************************/

#include "AM_PhysicsDebug.h"

void AMPhysicsDebug::bufInit()
{
	//vb description
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(MMDVERTEX)*2;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	//vertex buffer data
	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = _v;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	_dev->CreateBuffer(&bd, &initData, &_vBuf);

	UINT stride = sizeof(MMDVERTEX);
	UINT offset = 0;
	_dev->IASetVertexBuffers(0, 1, &_vBuf, &stride, &offset);
	_dev->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
}


AMPhysicsDebug::AMPhysicsDebug(ID3D10Device *dev, ID3D10EffectTechnique *tech)
{
	_dev = dev;
	_tech = tech;
	_vBuf = NULL;

	_v = new MMDVERTEX[2];
	_v[0].pos.x = 0;
	_v[0].pos.y = 0;
	_v[0].pos.z = 0;
	_v[1].pos.x = 0;
	_v[1].pos.y = 0;
	_v[1].pos.z = 0;

	bufInit();
}
AMPhysicsDebug::~AMPhysicsDebug()
{
	if (_v)
		delete[] _v;
	if (_vBuf)
		_vBuf->Release();
}

void AMPhysicsDebug::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	//set the positions of the line ends
	_v[0].pos.x = from.getX();
	_v[0].pos.y = from.getY();
	_v[0].pos.z = from.getZ();
	_v[1].pos.x = to.getX();
	_v[1].pos.y = to.getY();
	_v[1].pos.z = to.getZ();

	//set the colors of the line ends
	_v[0].normal.x = color.getX();
	_v[0].normal.y = color.getY();
	_v[0].normal.z = color.getZ();
	_v[1].normal.x = color.getX();
	_v[1].normal.y = color.getY();
	_v[1].normal.z = color.getZ();

	//map the vertex buffer so we can edit it and write the new vertices to it
	void* pVoid;
	_vBuf->Map(D3D10_MAP_WRITE_DISCARD, 0, &pVoid);
	memcpy(pVoid, _v, sizeof(_v[0])*2);
	_vBuf->Unmap();
	
	//make sure device is set up to render
	UINT stride = sizeof(MMDVERTEX);
	UINT offset = 0;
	_dev->IASetVertexBuffers(0, 1, &_vBuf, &stride, &offset);
	_dev->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	//render the line
	_tech->GetPassByIndex(0)->Apply(0);
	_dev->Draw(2, 0);
}
void AMPhysicsDebug::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{

}
void AMPhysicsDebug::reportErrorWarning (const char *warningString)
{

}
void AMPhysicsDebug::draw3dText (const btVector3 &location, const char *textString)
{

}
void AMPhysicsDebug::setDebugMode (int debugMode)
{
	_dbgMode = (DebugDrawModes)debugMode;
}
int AMPhysicsDebug::getDebugMode() const
{
	return (int)_dbgMode;
}