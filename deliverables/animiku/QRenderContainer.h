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

#ifndef RNDRCONT_H_
#define RNDRCONT_H_

#include <qframe.h>
#include "QD3DRenderWidget.h"

class QRenderContainer : public QFrame
{
	Q_OBJECT
	Q_DISABLE_COPY(QRenderContainer)
private:
	QD3DRenderWidget *_rndrW;
	QWidget* _parent;
	QString _bgndColor;
	float _aspect;
public:
	QRenderContainer(QFrame *parent = 0);
	void release();
	void reParent();
	virtual ~QRenderContainer();
protected:
	virtual void resizeEvent(QResizeEvent *evt);
};

#endif