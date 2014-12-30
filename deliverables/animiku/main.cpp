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

#include "AM_Screen.h"
#include "AMainWindow.h"
#include <QApplication>

#pragma comment (lib, "d3d10.lib")
#pragma comment (lib, "d3dx10.lib")


int main (int argc, char *argv[])
{
	QApplication a(argc, argv);
    MainWindow wndw;
	wndw.show();

    return a.exec();
}

