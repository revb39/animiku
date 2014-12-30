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

#include "QD3DRenderWidget.h"
#include <qevent.h>

QD3DRenderWidget::QD3DRenderWidget(QWidget* parent) : QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);

	TheScreen.init(0, 0, (HWND)winId());

	loopTmr = new QTimer(this);
	loopTmr->setInterval(0);
	loopTmr->start();
	QObject::connect(loopTmr, SIGNAL(timeout()), this, SLOT(render()));
}
QD3DRenderWidget::~QD3DRenderWidget()
{
	
}

void QD3DRenderWidget::render(void)
{
	TheScreen.render();
}