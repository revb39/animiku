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

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <qmainwindow.h>
#include "AM_PerfMgr.h"
#include "ui_mainwindow.h"
#include "AOptionsDlg.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent* event);

public slots:
	void addSong(void);
	void deleteSong(void);
	void moveSongUp(void);
	void moveSongDown(void);
	void selectSong(void);
	void renameSong(QListWidgetItem* item);

	void addModel(void);
	void deleteModel(void);
	void toggleModelVisibility(void);
	void changeActiveModel(int ind);
	void changeModelOffset(double d);
	void offsetDialMove(int value);

	void browseAnim(void);

	void browseAudio(void);
	void changeAudioDelay(int d);

	void changeCameraX(double d);
	void changeCameraY(double d);
	void changeCameraZoom(double d);
	void camXDialMove(int v);
	void camYDialMove(int v);

	void menuNew();
	void menuOpen();
	void menuSave();
	void menuSaveAs();
	void menuImport();
	void menuExit();

	void toggleDock(bool checked);
	void toggleFullScreen(bool checked);

	void camZoomDialMove(int v);

	void playTest(void);

	void play(void);
	void stop(void);

	void options(bool checked);

private:
	AMPerfMgr *_perfMgr;
	Ui::MainWindow *ui;

	float offsetDialAmt;
	float camXDialAmt;
	float camYDialAmt;
	float camZoomDialAmt;

	float lastOffsetDialValue;
	float lastCamXDialValue;
	float lastCamYDialValue;
	float lastCamZoomDialValue;

	void disableModels();
	void disableAnimation();
	void disableAudio();
};

#endif