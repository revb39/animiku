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

#include "QRenderContainer.h"

QRenderContainer::QRenderContainer(QFrame* parent) : QFrame(parent)
{
	_rndrW = new QD3DRenderWidget(this);
	this->resize(800, height());
	_aspect = (float)TheScreen.width / (float)TheScreen.height;
	_parent = parent;
	_bgndColor = this->styleSheet();
}
QRenderContainer::~QRenderContainer()
{
	delete _rndrW;
}

void QRenderContainer::release()
{
	setParent(0);
	this->setStyleSheet("\nbackground-color: #000000;\n");
}
void QRenderContainer::reParent()
{
	setParent(_parent);
	this->setStyleSheet(_bgndColor);
}

void QRenderContainer::resizeEvent(QResizeEvent *evt)
{
	float newAspect = (float)width() / (float)height();
	int w = width();
	int h = height();
	int nw = width();
	int nh = height();;
	if(newAspect > _aspect)
		nw = (float)height()*_aspect;
	else if (newAspect < _aspect)
		nh = (float)width()/_aspect;

	_rndrW->resize(nw, nh);

	float cx = (float)width()*0.5f;
	float cy = (float)height()*0.5f;
	_rndrW->move(cx-(nw*0.5f), cy-(nh*0.5f));
}