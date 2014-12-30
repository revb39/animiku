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

#ifndef _PERFMGR_H_
#define _PERFMGR_H_

#include <cstdlib>
#include <string>
#include <vector>
#include <qtimer.h>
#include "AM_Screen.h"
#include "AM_Manager.h"
#include "ui_mainwindow.h"

struct ModelData
{
	std::string mdlFile;
	std::string animFile;
	bool visibleTest;
	float offset;
};
struct Song
{
	std::string name;
	std::vector<ModelData> models;

	std::string audioFile;
	int audioDelay;
};

class AMPerfMgr : public QObject, public AM_Manager
{
	Q_OBJECT
private:
	std::vector<Song> _songList;
	QTimer *_delayTimer;
	Ui::MainWindow* _ui;
	int _curSong;
	int _curPlayingSong;
	int _modelsToHide;
	bool _playing;
	bool _test;
	bool _reload;
	bool _rewind;

	void prepAndPlay(int ind, bool fromPlaylist);
	void prepAndPlay()
	{
		prepAndPlay(_curSong, false);
	}

	char* genModelId(int song, int model);
public slots:
	void startAudio(void);
	void startAnim(void);
public:
	AMPerfMgr(Ui::MainWindow* ui);
	~AMPerfMgr();

	//inherited from AM_Manager
	void songEnd();
	void fadedOut();

	void setReloadModels(bool reload);
	void setRewind(bool rewind);

	void clearEverything();

	void addSong(std::string name="<New Song>");
	void deleteSong(int ind);
	void deleteSong()
	{
		deleteSong(_curSong);
	}
	void renameSong(int ind, std::string newName);
	void renameSong(std::string newName)
	{
		renameSong(_curSong, newName);
	}
	void moveSong(int ind, int newInd);
	void moveSong(int newInd)
	{
		moveSong(_curSong, newInd);
	}
	void selectSong(int ind);
	Song* getSong(int ind);
	Song* getSong()
	{
		return getSong(_curSong);
	}
	void playSong(int ind, bool fromPlaylist);
	void playSong()
	{
		playSong(_curSong, false);
	}
	void stopSong(int ind);
	void stopSong()
	{
		stopSong(_curSong);
	}

	void addModel(int songInd, std::string id, std::string file);
	void addModel(std::string id, std::string file)
	{
		addModel(_curSong, id, file);
	}
	void deleteModel(int songInd, int mdlInd);
	void deleteModel(int mdlInd)
	{
		deleteModel(_curSong, mdlInd);
	}
	void setAnimation(int songInd, int mdlInd, std::string file);
	void setAnimation(int mdlInd, std::string file)
	{
		setAnimation(_curSong, mdlInd, file);
	}
	void setOffset(int songInd, int mdlInd, float offset);
	void setOffset(int mdlInd, float offset)
	{
		setOffset(_curSong, mdlInd, offset);
	}
	void setAudioFile(int ind, std::string file);
	void setAudioFile(std::string file)
	{
		setAudioFile(_curSong, file);
	}
	void setAudioDelay(int ind, int delay);
	void setAudioDelay(int delay)
	{
		setAudioDelay(_curSong, delay);
	}

	void play(bool test);
	void stop();

	void loadFromFile(std::string file, Ui::MainWindow *ui);
	void saveToFile(std::string file);
};

#endif