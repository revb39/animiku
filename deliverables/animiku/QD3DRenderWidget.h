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

#ifndef RNDRWIDGET_H_
#define RNDRWIDGET_H_

#include "AM_Screen.h"
#include <qwidget.h>
#include <qtimer.h>

class QD3DRenderWidget : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(QD3DRenderWidget)
private:
	QTimer *loopTmr;
public:
	QD3DRenderWidget(QWidget *parent = 0);
	virtual ~QD3DRenderWidget();
	virtual QPaintEngine* paintEngine() const {return NULL;}
public slots:
	void render(void);
};

#endif