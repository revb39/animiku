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

#include "AM_PerfMgr.h"
#include <sstream>
#include <qmessagebox.h>

void AMPerfMgr::prepAndPlay(int ind, bool fromPlaylist)
{
	if (ind < 0)
		return;

	ModelData* curMdl;
	for (int i=0;i<(int)_songList[ind].models.size();i++)
	{
		curMdl = &_songList[ind].models[i];

		char t[4];
		sprintf_s(t, "%i%i", ind, i);

		//make sure all the content is loaded
		if (!TheScreen.modelExists(t))
		{
			//if the model isn't loaded, stop
			continue;
		}
		//apply any offset to the model
		TheScreen.moveModel(t, curMdl->offset, 0, 0);

		if (TheScreen.motionExists((char*)curMdl->animFile.c_str()))
		{
			//only play the motion if it is loaded

			//assign the motion to the model
			TheScreen.assignMotionToModel(t, (char*)curMdl->animFile.c_str(), true);

			//set the model visible
			TheScreen.setModelVisible(t, true);

			if (!fromPlaylist || !TheScreen.getModelVisible(t))
				TheScreen.resetModelPhysics(t); //only reset physics if not auto from playlist or model is invisible

			//start playback
			TheScreen.playMotion(t);

			continue;
		}

		//set the model visible
		TheScreen.setModelVisible((char*)curMdl->mdlFile.c_str(), true);	
	}
}

char* AMPerfMgr::genModelId(int song, int model)
{
	char t[4];
	sprintf_s(t, "%i%i", song, model);
	return t;
}

void AMPerfMgr::startAnim(void)
{
	prepAndPlay();
	_delayTimer->stop();
}
void AMPerfMgr::startAudio(void)
{
	mciSendString("play song", NULL, 0, NULL);
	_delayTimer->stop();
}

AMPerfMgr::AMPerfMgr(Ui::MainWindow *ui)
{
	_ui = ui;
	_delayTimer = NULL;
	_delayTimer = new QTimer();
	_curSong = -1;
	_modelsToHide = 0;
	_playing = false;
	_reload = false;
	_rewind = true;
	_test = false;
}
AMPerfMgr::~AMPerfMgr()
{
	if (_delayTimer)
		delete _delayTimer;
}

void AMPerfMgr::songEnd()
{
	if (_test)
		return; //no playlist in test mode

	for (int i=0;i<_songList[_curPlayingSong].models.size();i++)
	{
		/*
		check if model is in next song

		yes:
			do nothing
		no OR there is no next song:
			fade out
		*/

		bool repeat = false;
		if (_curPlayingSong+1 < _songList.size()) //is there is a next song?
		{
			for (int j=0;j<_songList[_curPlayingSong+1].models.size();j++)
			{
				if (!strcmp(_songList[_curPlayingSong].models[i].mdlFile.c_str(), _songList[_curPlayingSong+1].models[j].mdlFile.c_str()))
				{
					//is this model also present in the next song?
					repeat = true;
					break;
				}
			}
		}

		if (!repeat)
			TheScreen.setModelVisible(genModelId(_curPlayingSong,i), false); //only hide if model is not in next song
	}

	if (_curPlayingSong+1 < _songList.size()) //is there is a next song?
	{
		_curPlayingSong++;
		playSong(_curPlayingSong, true);
	}
	else
	{
		_playing = false;
		stop();
	}

	/*
	if there is a next song:
		_curPlayingIndex++
		playSong(_curPlayingIndex)
	else:
		_playing = false
		stop()

	*/
}
void AMPerfMgr::fadedOut()
{
	//now what do?
}

void AMPerfMgr::setReloadModels(bool reload)
{
	_reload = reload;
}
void AMPerfMgr::setRewind(bool rewind)
{
	_rewind = rewind;
}
void AMPerfMgr::clearEverything()
{
	for (vector<Song>::iterator i = _songList.begin();i != _songList.end();i++)
	{
		//for each song in the set
		//clear model list
		(*i).models.clear();
	}
	//now we can clear the song list
	_songList.clear();

	TheScreen.clearMotions();
	TheScreen.clearModels();
}
void AMPerfMgr::addSong(std::string name)
{
	Song newSong;
	newSong.audioDelay = 0;
	newSong.audioFile = "";
	newSong.name = name;

	_songList.push_back(newSong);
}
void AMPerfMgr::deleteSong(int ind)
{
	_songList.erase(_songList.begin() + ind);
}
void AMPerfMgr::renameSong(int ind, std::string newName)
{
	_songList[ind].name = newName;
}
void AMPerfMgr::moveSong(int ind, int newInd)
{
	std::vector<Song>::iterator current = _songList.begin() + ind;
	std::vector<Song>::iterator result = _songList.begin() + newInd;

	Song toMove = _songList[ind];
	
	if (newInd < ind)
	{
		_songList.insert(_songList.begin() + newInd, toMove);
		_songList.erase(_songList.begin() + ind+1);
	}
	else if (newInd > ind)
	{
		_songList.insert(_songList.begin() + newInd+1, toMove);
		_songList.erase(_songList.begin() + ind);
	}
}
void AMPerfMgr::selectSong(int ind)
{
	if (ind < 0)
		_curSong = -1;
	else
		_curSong = ind;
}
Song* AMPerfMgr::getSong(int ind)
{
	if (ind >= _songList.size())
		return NULL;
	return &_songList[ind];
}
void AMPerfMgr::playSong(int ind, bool fromPlaylist)
{
	if (ind < 0)
		return;
	//start loading the audio file
	std::string mciCommand = "open \""+_songList[ind].audioFile+"\" alias song wait";
	mciSendString(mciCommand.c_str(), NULL, 0, NULL);
	mciSendString("seek song to start", NULL, 0, NULL);

	//update the list box, and name
	_ui->listSetlist->setCurrentRow(ind);
	_ui->lblCurSong->setText(QString::fromStdString(_songList[ind].name));

	if  (_songList[ind].audioDelay > 0)
	{
		//delay > 0 means to delay audio playback
		prepAndPlay(ind, fromPlaylist);
		_delayTimer->stop();
		_delayTimer->setInterval(_songList[ind].audioDelay);
		QObject::disconnect(this, SLOT(startAnim()));
		QObject::connect(_delayTimer, SIGNAL(timeout()), this, SLOT(startAudio()));
		_delayTimer->start();
	}
	else if (_songList[ind].audioDelay < 0)
	{
		//delay < 0 means to delat animation playback
		mciSendString("play song", NULL, 0, NULL);
		_delayTimer->stop();
		_delayTimer->setInterval(-_songList[ind].audioDelay);
		QObject::disconnect(this, SLOT(startAudio()));
		QObject::connect(_delayTimer, SIGNAL(timeout()), this, SLOT(startAnim()));
		_delayTimer->start();
	}
	else
	{
		//no delay means just start
		prepAndPlay(ind, fromPlaylist);
		mciSendString("play song", NULL, 0, NULL);
	}
}
void AMPerfMgr::stopSong(int ind)
{
	mciSendString("stop song", NULL, 0, NULL);

	for (int i=0;i<(int)_songList[ind].models.size();i++)
	{
		char t[4];
		sprintf_s(t, "%i%i", ind, i);

		if (!TheScreen.modelExists(t))
			continue;
		TheScreen.stopMotion(t);
	}
}

void AMPerfMgr::addModel(int songInd, std::string id, std::string file)
{
	ModelData newModel;
	newModel.animFile = "";
	newModel.mdlFile = file;
	newModel.offset = 0;

	_songList[songInd].models.push_back(newModel);
	TheScreen.preloadModel((char*)id.c_str(), (char*)file.c_str());
	TheScreen.rotateModel((char*)id.c_str(), 0, 180, 0);
}
void AMPerfMgr::deleteModel(int songInd, int mdlInd)
{
	char t[4];
	sprintf_s(t, "%i%i", songInd, mdlInd);

	TheScreen.clearModel(t);
	_songList[songInd].models.erase(_songList[songInd].models.begin()+mdlInd);
}
void AMPerfMgr::setAnimation(int songInd, int mdlInd, std::string file)
{
	char t[4];
	sprintf_s(t, "%i%i", songInd, mdlInd);

	TheScreen.clearMotion(t);
	_songList[songInd].models[mdlInd].animFile = file;

	TheScreen.preloadMotion((char*)file.c_str(), (char*)file.c_str());
}
void AMPerfMgr::setOffset(int songInd, int mdlInd, float offset)
{
	_songList[songInd].models[mdlInd].offset = offset;
}
void AMPerfMgr::setAudioFile(int ind, std::string file)
{
	_songList[ind].audioFile = file;
}
void AMPerfMgr::setAudioDelay(int ind, int delay)
{
	_songList[ind].audioDelay = delay;
}

void AMPerfMgr::play(bool test)
{
	_test = test;
	_playing = true;
	_curPlayingSong = _curSong;
	playSong();
}
void AMPerfMgr::stop()
{
	_ui->lblCurSong->setText("Not Playing");

	_playing = false;
	if (!_rewind)
		_curSong = _curPlayingSong;
	stopSong(_curPlayingSong);

	ModelData *curMdl;
	for (int i=0;i<(int)_songList[_curPlayingSong].models.size();i++)
	{
		curMdl = &_songList[_curPlayingSong].models[i];

		//make sure all the content is loaded
		if (!TheScreen.modelExists((char*)curMdl->mdlFile.c_str()))
		{
			//if the model isn't loaded, skip it
			continue;
		}

		TheScreen.setModelVisible((char*)curMdl->mdlFile.c_str(), false);
	}
}

void AMPerfMgr::loadFromFile(std::string file, Ui::MainWindow *ui)
{
	FILE *fp;
	char str[6];
	fopen_s(&fp, file.c_str(), "r");
	fgets(str,6,fp);
	fclose(fp);

	if (strstr(str, "AMPV2") == NULL && strstr(str, "AMSV2") == NULL)
	{
		MessageBox(NULL, "Not a valid Save File! If this is an old save, please convert to V2", "Error", MB_OK);
		return;
	}

	int numSongs = GetPrivateProfileInt("PERF", "num", 0, file.c_str());
	for (int i=0;i<numSongs;i++)
	{
		std::stringstream ss;
		ss << "CHP-";
		ss << i;
		std::string app = ss.str();
		char name[MAX_PATH];
		char vmd1[MAX_PATH];
		char vmd2[MAX_PATH];
		char sound[MAX_PATH];
		int delay = 0;
		char model1[MAX_PATH];
		char model2[MAX_PATH];
		float offset1 = 0;
		float offset2 = 0;

		GetPrivateProfileString(app.c_str(), "name", "<Error loading name>", name, MAX_PATH, file.c_str());
		GetPrivateProfileString(app.c_str(), "vmd1", "", vmd1, MAX_PATH, file.c_str());
		GetPrivateProfileString(app.c_str(), "vmd2", "", vmd2, MAX_PATH, file.c_str());
		GetPrivateProfileString(app.c_str(), "sound", "", sound, MAX_PATH, file.c_str());
		delay = GetPrivateProfileInt(app.c_str(), "delay", 0, file.c_str());
		GetPrivateProfileString(app.c_str(), "model1", "", model1, MAX_PATH, file.c_str());
		GetPrivateProfileString(app.c_str(), "model2", "", model2, MAX_PATH, file.c_str());
		char offset[MAX_PATH];
		GetPrivateProfileString(app.c_str(), "offset1", "", offset, MAX_PATH, file.c_str());
		offset1 = atof(offset);
		GetPrivateProfileString(app.c_str(), "offset2", "", offset, MAX_PATH, file.c_str());
		offset2 - atof(offset);

		if (strlen(model1) > 2 && model1[1] != ':')
		{
			QString error;
			error = "Model\n\n\""+QString::fromStdString(model1)+"\"\n\nof song #"+QString::number(i+1)+":\n\n\""+QString::fromStdString(name)+"\"\n\nis a predefined model in previous AniMiku versions. You will need to load this model again.";
			QMessageBox::information(NULL, "Error", error);
			strcpy_s(model1, sizeof(model1), "");
		}
		if (strlen(model2) > 2 && model2[1] != ':')
		{
			QString error;
			error = "Model\n\n\""+QString::fromStdString(model1)+"\"\n\nof song #"+QString::number(i+1)+":\n\n\""+QString::fromStdString(name)+"\"\n\nis a predefined model in previous AniMiku versions. You will need to load this model again.";
			QMessageBox::information(NULL, "Error", error);
			strcpy_s(model2, sizeof(model2), "");
		}

		addSong(name);
		ui->listSetlist->addItem(QString::fromStdString(name));
		setAudioFile(i, sound);
		setAudioDelay(i, delay);
		if(strlen(model1) > 0)
		{
			char t[4];
			sprintf_s(t, "%i%i", i, 0);
			addModel(i, t, model1);
			setOffset(i, 0, offset1);
			if (strlen(vmd1) > 0)
			{
				setAnimation(i, 0, vmd1);
			}
		}
		if(strlen(model2) > 0)
		{
			char t[4];
			sprintf_s(t, "%i%i", i, 1);
			addModel(i, t, model2);
			setOffset(i, 0, offset2);
			if (strlen(vmd2) > 0)
			{
				setAnimation(i, 1, vmd2);
			}
		}
	}
	ui->listSetlist->setCurrentRow(0);

	/*char chNumC[MAX_PATH];
	sprintf_s(chNumC, sizeof(chNumC), "CHP-%i", chNum);
	*/
}
void saveToFile(std::string file)
{

}