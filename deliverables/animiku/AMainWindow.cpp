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

#include "AMainWindow.h"
#include "defaults.h"
#include "ACheckableMessageBox.h"

#include <qfiledialog.h>
#include <qmessagebox.h>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
	_perfMgr = new AMPerfMgr(ui);
	TheScreen.registerManagerCallback(_perfMgr);
	offsetDialAmt = 0.5f;
	camXDialAmt = 0.5f;
	camYDialAmt = 0.5f;
	camZoomDialAmt = 0.01f;
	lastOffsetDialValue = 0;
	lastCamXDialValue = 0;
	lastCamYDialValue = 0;
	lastCamZoomDialValue = 0;

	OptionsDialog* d = new OptionsDialog();
	d->ok();
	delete d;

	ui->setupUi(this);
	ui->btnAddSong->setIcon(QIcon(":/icons/icons/add.png"));
	ui->btnAddSong->setIconSize(QSize(20, 20));
	ui->btnDelSong->setIcon(QIcon(":/icons/icons/del.png"));
	ui->btnDelSong->setIconSize(QSize(20, 20));
	ui->btnMoveUp->setIcon(QIcon(":/icons/icons/up.png"));
	ui->btnMoveUp->setIconSize(QSize(20, 20));
	ui->btnMoveDwn->setIcon(QIcon(":/icons/icons/down.png"));
	ui->btnMoveDwn->setIconSize(QSize(20, 20));
	ui->btnAddMdl->setIcon(QIcon(":/icons/icons/add.png"));
	ui->btnAddMdl->setIconSize(QSize(20, 20));
	ui->btnDelMdl->setIcon(QIcon(":/icons/icons/del.png"));
	ui->btnDelMdl->setIconSize(QSize(20, 20));

	//get camera settings from registry
	HKEY key = NULL;
	HRESULT hr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\AniMiku", NULL, KEY_WRITE, &key);
	if (key != NULL)
	{
		float x = atof(OptionsDialog::getValueFromRegistry(key, "cam x", DEF_CAM_X).c_str());
		float y = atof(OptionsDialog::getValueFromRegistry(key, "cam y", DEF_CAM_Y).c_str());
		float zoom = atof(OptionsDialog::getValueFromRegistry(key, "cam zoom", DEF_CAM_ZOOM).c_str());
		ui->nbrCamX->setValue(x);
		ui->nbrCamY->setValue(y);
		ui->nbrCamZoom->setValue(zoom);

		float *curTarget = TheScreen.cam->getTarget();
		float *curPos = TheScreen.cam->getPos();
		TheScreen.cam->lookAtFromPos((float)-x, y, curTarget[2], (float)-x, y, curPos[2]);
		TheScreen.cam->setZoom(zoom);

		RegCloseKey(key);
	}
	

	QObject::connect(ui->btnAddSong, SIGNAL(clicked()), this, SLOT(addSong()));
	QObject::connect(ui->btnDelSong, SIGNAL(clicked()), this, SLOT(deleteSong()));
	QObject::connect(ui->btnMoveUp, SIGNAL(clicked()), this, SLOT(moveSongUp()));
	QObject::connect(ui->btnMoveDwn, SIGNAL(clicked()), this, SLOT(moveSongDown()));
	QObject::connect(ui->listSetlist, SIGNAL(itemSelectionChanged()), this, SLOT(selectSong()));
	QObject::connect(ui->listSetlist, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(renameSong(QListWidgetItem*)));

	QObject::connect(ui->btnAddMdl, SIGNAL(clicked()), this, SLOT(addModel()));
	QObject::connect(ui->btnDelMdl, SIGNAL(clicked()), this, SLOT(deleteModel()));
	QObject::connect(ui->chkVisibleTest, SIGNAL(clicked()), this, SLOT(toggleModelVisibility()));
	QObject::connect(ui->cmbMdlList, SIGNAL(currentIndexChanged(int)), this, SLOT(changeActiveModel(int)));
	QObject::connect(ui->nbrMdlOffset, SIGNAL(valueChanged(double)), this, SLOT(changeModelOffset(double)));
	QObject::connect(ui->dialAnimOffset, SIGNAL(valueChanged(int)), this, SLOT(offsetDialMove(int)));

	QObject::connect(ui->btnAnimBrowse, SIGNAL(clicked()), this, SLOT(browseAnim()));

	QObject::connect(ui->btnSndBrowse, SIGNAL(clicked()), this, SLOT(browseAudio()));
	QObject::connect(ui->nbrSndDelay, SIGNAL(valueChanged(int)), this, SLOT(changeAudioDelay(int)));

	QObject::connect(ui->nbrCamX, SIGNAL(valueChanged(double)), this, SLOT(changeCameraX(double)));
	QObject::connect(ui->nbrCamY, SIGNAL(valueChanged(double)), this, SLOT(changeCameraY(double)));
	QObject::connect(ui->nbrCamZoom, SIGNAL(valueChanged(double)), this, SLOT(changeCameraZoom(double)));
	QObject::connect(ui->dialCamX, SIGNAL(valueChanged(int)), this, SLOT(camXDialMove(int)));
	QObject::connect(ui->dialCamY, SIGNAL(valueChanged(int)), this, SLOT(camYDialMove(int)));
	QObject::connect(ui->dialCamZoom, SIGNAL(valueChanged(int)), this, SLOT(camZoomDialMove(int)));

	QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(menuNew()));
	QObject::connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(menuOpen()));
	QObject::connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(menuSave()));
	QObject::connect(ui->actionSave_As, SIGNAL(triggered(bool)), this, SLOT(menuSaveAs()));
	QObject::connect(ui->actionImport, SIGNAL(triggered(bool)), this, SLOT(menuImport()));

	QObject::connect(ui->actionDocked, SIGNAL(toggled(bool)), this, SLOT(toggleDock(bool)));
	QObject::connect(ui->actionFull_Screen, SIGNAL(toggled(bool)), this, SLOT(toggleFullScreen(bool)));

	QObject::connect(ui->btnTestPlay, SIGNAL(clicked()), this, SLOT(playTest()));

	QObject::connect(ui->btnPlay, SIGNAL(clicked()), this, SLOT(play()));
	QObject::connect(ui->btnStop, SIGNAL(clicked()), this, SLOT(stop()));

	QObject::connect(ui->actionOptions, SIGNAL(triggered(bool)), this, SLOT(options(bool)));
}
MainWindow::~MainWindow()
{
	delete ui;
	delete _perfMgr;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	//make sure rendering window closes
	ui->amRenderContainer->close();

	//saves camera settings
	//
	//open the key
	HKEY key = NULL;
	HRESULT hr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\AniMiku", NULL, KEY_ALL_ACCESS, &key);
	if (key == NULL)
		return;

	bool saveCam = false;
	switch (atoi(OptionsDialog::getValueFromRegistry(key, "save cam on exit", DEF_CAM_EXIT).c_str()))
	{
	case 0: //never
		break;
	case 1: //always
		saveCam = true;
		break;
	case 2: //ask
		CheckableMessageBox* msb = CheckableMessageBox::newDoNotAskAgainQuestion("", "Would you like to save your camera position settings?");
		if (msb->exec() == QDialog::Accepted)
		{
			if (!msb->shouldAskAgain())
				OptionsDialog::setValueInRegistry(key, "save cam on exit", "1");
			saveCam = true;
		}
		else if (msb->exec() == QDialog::Rejected)
		{
			if (!msb->shouldAskAgain())
				OptionsDialog::setValueInRegistry(key, "save cam on exit", "0");
			saveCam = false;
		}
		break;
	}

	if (saveCam)
	{
		OptionsDialog::setValueInRegistry(key, "cam x", QString::number(ui->nbrCamX->value()).toStdString());
		OptionsDialog::setValueInRegistry(key, "cam y", QString::number(ui->nbrCamY->value()).toStdString());
		OptionsDialog::setValueInRegistry(key, "cam zoom", QString::number(ui->nbrCamZoom->value()).toStdString());
	}
}


void MainWindow::addSong(void)
{
	QListWidgetItem *newItem = new QListWidgetItem("<New Song>");
	newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
	ui->listSetlist->addItem(newItem);
	_perfMgr->addSong();
	ui->listSetlist->setCurrentItem(newItem);
	ui->listSetlist->editItem(newItem);
}
void MainWindow::deleteSong(void)
{
	int rowToDelete = ui->listSetlist->currentRow();
	if (rowToDelete == -1)
		return;

	delete ui->listSetlist->takeItem(rowToDelete);
	_perfMgr->deleteSong(rowToDelete);
}
void MainWindow::moveSongUp(void)
{
	int rowMoveFrom = ui->listSetlist->currentRow();
	int rowMoveTo = rowMoveFrom-1;
	if (rowMoveTo < 0)
		return;
	QListWidgetItem* itmToMove = ui->listSetlist->takeItem(rowMoveFrom);
	ui->listSetlist->insertItem(rowMoveTo, itmToMove);
	ui->listSetlist->setCurrentItem(itmToMove);
	_perfMgr->moveSong(rowMoveFrom, rowMoveTo);
}
void MainWindow::moveSongDown(void)
{
	int rowMoveFrom = ui->listSetlist->currentRow();
	int rowMoveTo = rowMoveFrom+1;
	if (rowMoveFrom < 0 || rowMoveTo+1 > ui->listSetlist->count())
		return;
	QListWidgetItem* itmToMove = ui->listSetlist->takeItem(rowMoveFrom);
	ui->listSetlist->insertItem(rowMoveTo, itmToMove);
	ui->listSetlist->setCurrentItem(itmToMove);
	_perfMgr->moveSong(rowMoveFrom, rowMoveTo);
}
void MainWindow::selectSong(void)
{
	_perfMgr->selectSong(ui->listSetlist->currentRow());
	Song* selSong = _perfMgr->getSong();
	if (ui->listSetlist->currentRow() < 0)
	{
		disableModels();
		disableAnimation();
		disableAudio();
	}
	else
	{
		ui->frmMdlContainer->setEnabled(true);
		ui->frmSndContainer->setEnabled(true);
		
		ui->lblSndFileName->setText(QFileInfo(selSong->audioFile.c_str()).fileName());
		ui->edtSndFile->setText(selSong->audioFile.c_str());
		ui->nbrSndDelay->setValue(selSong->audioDelay);

		ui->cmbMdlList->clear();
		for (int i=0;i<selSong->models.size();i++)
		{
			QFileInfo fi(selSong->models[i].mdlFile.c_str());
			ui->cmbMdlList->addItem(fi.fileName());
		}


		if (selSong != NULL && selSong->models.size() > 0)
		{
			ui->cmbMdlList->setCurrentIndex(0);
			ui->frmAnimContainer->setEnabled(true);
		}
		else
		{
			ui->cmbMdlList->setCurrentIndex(-1);
			ui->edtAnimFile->setText("");
			ui->lblAnimFileName->setText("");
			ui->frmAnimContainer->setEnabled(false);
		}
	}
}
void MainWindow::renameSong(QListWidgetItem* item)
{
	int ind = ui->listSetlist->row(item);
	_perfMgr->renameSong(ind, item->text().toStdString());
}

void MainWindow::addModel(void)
{
	std::string filter;
	std::string filterAll = "All Supported Types(";
	for (InhModelParsers::reverse_iterator it = AM_Model::ModelParsers().rbegin();it!= AM_Model::ModelParsers().rend();++it)
	{
		filter += std::string((*it).second.Description) + " (*." + (*it).first + ");;";
		if (it == AM_Model::ModelParsers().rbegin())
			filterAll += "*."+(*it).first;
		else
			filterAll += " *."+(*it).first;
	}
	filterAll += ");;"+filter;
	filterAll.erase(filterAll.end()-2, filterAll.end());
	QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(NULL, "Load Model", "", filterAll.c_str()));
	if (file.length() <= 0)
		return;

	QFileInfo fi(file);
	QByteArray ba = file.toLocal8Bit();
	char t[4];
	sprintf_s(t, "%i%i", ui->listSetlist->currentRow(), ui->cmbMdlList->count());
	std::string id(t);
	_perfMgr->addModel(id, ba.data());
	ui->cmbMdlList->addItem(fi.fileName());
	ui->cmbMdlList->setCurrentIndex(ui->cmbMdlList->count()-1);
}
void MainWindow::deleteModel(void)
{
	int indToDelete = ui->cmbMdlList->currentIndex();
	if (indToDelete == -1)
		return;
	_perfMgr->deleteModel(indToDelete);
	ui->cmbMdlList->removeItem(indToDelete);
}
void MainWindow::toggleModelVisibility(void)
{
	ModelData* mdlToToggle = &_perfMgr->getSong()->models[ui->cmbMdlList->currentIndex()];
	char t[4];
	sprintf_s(t, "%i%i", ui->listSetlist->currentRow(), ui->cmbMdlList->currentIndex());
	std::string id(t);
	TheScreen.moveModel((char*)id.c_str(), ui->nbrMdlOffset->value(), 0, 0);
	TheScreen.setModelVisible((char*)id.c_str(), ui->chkVisibleTest->isChecked());
	mdlToToggle->visibleTest = ui->chkVisibleTest->isChecked();
}
void MainWindow::changeActiveModel(int ind)
{
	if (ind < 0)
	{
		disableAnimation();
		ui->edtMdlLoc->setText("");
		ui->nbrMdlOffset->setValue(0);
		ui->chkVisibleTest->setEnabled(false);
		ui->nbrMdlOffset->setEnabled(false);
		ui->chkVisibleTest->setChecked(false);
		return;
	}
	else
	{
		QString mdlFile = _perfMgr->getSong()->models[ind].mdlFile.c_str();
		QString animFile = _perfMgr->getSong()->models[ind].animFile.c_str();
		float ofs = _perfMgr->getSong()->models[ind].offset;
		bool vis = _perfMgr->getSong()->models[ind].visibleTest;

		ui->edtMdlLoc->setText(mdlFile);
		ui->nbrMdlOffset->setValue(ofs);
		ui->chkVisibleTest->setChecked(vis);

		ui->lblAnimFileName->setText(QFileInfo(animFile).fileName());
		ui->edtAnimFile->setText(animFile);

		ui->frmAnimContainer->setEnabled(true);
		ui->chkVisibleTest->setEnabled(true);
		ui->nbrMdlOffset->setEnabled(true);
		ui->dialAnimOffset->setEnabled(true);
	}
}
void MainWindow::changeModelOffset(double d)
{
	int ci = ui->cmbMdlList->currentIndex();
	Song* a = _perfMgr->getSong(ui->cmbMdlList->currentIndex());
	if (ui->cmbMdlList->currentIndex() < 0 || _perfMgr->getSong(ui->listSetlist->currentRow()) == NULL)
		return;
	_perfMgr->setOffset(ui->cmbMdlList->currentIndex(), (float)d);
	char t[4];
	sprintf_s(t, "%i%i", ui->listSetlist->currentRow(), ui->cmbMdlList->currentIndex());
	std::string id(t);
	TheScreen.moveModel((char*)id.c_str(), d, 0, 0);
}
void MainWindow::offsetDialMove(int value)
{
	float nv = (float)value/(float)ui->dialAnimOffset->maximum();
	if (value == 100 || value == 0)
	{
		lastOffsetDialValue = nv;
		return;
	}
	
	float delta = nv-lastOffsetDialValue;
	if (delta < 0)
		ui->nbrMdlOffset->setValue(ui->nbrMdlOffset->value()-offsetDialAmt);
	else
		ui->nbrMdlOffset->setValue(ui->nbrMdlOffset->value()+offsetDialAmt);
	lastOffsetDialValue = nv;
}

void MainWindow::browseAnim(void)
{
	std::string filter;
	std::string filterAll = "All Supported Types(";
	for (InhMotionParsers::reverse_iterator it = AM_Motion::MotionParsers().rbegin();it!= AM_Motion::MotionParsers().rend();++it)
	{
		filter += std::string((*it).second.Description) + " (*." + (*it).first + ");;";
		if (it == AM_Motion::MotionParsers().rbegin())
			filterAll += "*."+(*it).first;
		else
			filterAll += " *."+(*it).first;
	}
	filterAll += ");;"+filter;
	filterAll.erase(filterAll.end()-2, filterAll.end());
	QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(NULL, "Load Motion", "", filterAll.c_str()));
	if (file.length() <= 0)
		return;

	QFileInfo fi(file);
	QByteArray ba = file.toLocal8Bit();
	_perfMgr->setAnimation(ui->cmbMdlList->currentIndex(), ba.data());

	ui->lblAnimFileName->setText(fi.fileName());
	ui->edtAnimFile->setText(file);
}

void MainWindow::browseAudio(void)
{
	QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(NULL, "Load Audio", "", "MP3 (*.mp3);;WAV (*.wav);;"));
	if (file.length() <= 0)
		return;

	QFileInfo fi(file);
	QByteArray ba = file.toLocal8Bit();

	_perfMgr->setAudioFile(ba.data());

	ui->lblSndFileName->setText(fi.fileName());
	ui->edtSndFile->setText(file);
}
void MainWindow::changeAudioDelay(int d)
{
	_perfMgr->setAudioDelay(d);
}

void MainWindow::changeCameraX(double d)
{
	float *curTarget = TheScreen.cam->getTarget();
	float *curPos = TheScreen.cam->getPos();
	TheScreen.cam->lookAtFromPos((float)-d, curTarget[1], curTarget[2], (float)-d, curPos[1], curPos[2]);
}
void MainWindow::changeCameraY(double d)
{
	float *curTarget = TheScreen.cam->getTarget();
	float *curPos = TheScreen.cam->getPos();
	TheScreen.cam->lookAtFromPos(curTarget[0], (float)d, curTarget[2], curPos[0], (float)d, curPos[2]);
}
void MainWindow::changeCameraZoom(double d)
{
	TheScreen.cam->setZoom((float)d);
}
void MainWindow::camXDialMove(int v)
{
	float nv = (float)v/(float)ui->dialCamX->maximum();
	if (v == 100 || v == 0)
	{
		lastCamXDialValue = nv;
		return;
	}
	
	float delta = nv-lastCamXDialValue;
	if (delta < 0)
		ui->nbrCamX->setValue(ui->nbrCamX->value()+camXDialAmt);
	else
		ui->nbrCamX->setValue(ui->nbrCamX->value()-camXDialAmt);
	lastCamXDialValue = nv;
}
void MainWindow::camYDialMove(int v)
{
	float nv = (float)v/(float)ui->dialCamY->maximum();
	if (v == 100 || v == 0)
	{
		lastCamYDialValue = nv;
		return;
	}
	
	float delta = nv-lastCamYDialValue;
	if (delta < 0)
		ui->nbrCamY->setValue(ui->nbrCamY->value()+camYDialAmt);
	else
		ui->nbrCamY->setValue(ui->nbrCamY->value()-camYDialAmt);
	lastCamYDialValue = nv;
}
void MainWindow::camZoomDialMove(int v)
{
	float nv = (float)v/(float)ui->dialCamZoom->maximum();
	if (v == 100 || v == 0)
	{
		lastCamZoomDialValue = nv;
		return;
	}
	
	float delta = nv-lastCamZoomDialValue;
	if (delta < 0)
		ui->nbrCamZoom->setValue(ui->nbrCamZoom->value()-camZoomDialAmt);
	else
		ui->nbrCamZoom->setValue(ui->nbrCamZoom->value()+camZoomDialAmt);
	lastCamZoomDialValue = nv;
}

void MainWindow::menuNew()
{
	//update the UI
	ui->listSetlist->clear();
	ui->cmbMdlList->clear();
	ui->edtMdlLoc->setText("");
	ui->nbrMdlOffset->setValue(0);
	ui->chkVisibleTest->setChecked(false);
	ui->lblAnimFileName->setText("");
	ui->edtAnimFile->setText("");
	ui->lblSndFileName->setText("");
	ui->edtSndFile->setText("");
	ui->nbrSndDelay->setValue(0);

	//clear everything in the engine
	_perfMgr->clearEverything();
}
void MainWindow::menuOpen()
{
	QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(NULL, "Load Save", "", "AniMiku Performance (*.amp);;"));
	if (file == NULL)
		return;
	QByteArray ba = file.toLatin1();
	_perfMgr->loadFromFile(ba.data(), ui);
}
void MainWindow::menuSave()
{

}
void MainWindow::menuSaveAs()
{

}
void MainWindow::menuImport()
{

}
void MainWindow::menuExit()
{

}

void MainWindow::toggleDock(bool checked)
{
	if (checked)
	{
		ui->amRenderContainer->setWindowFlags((ui->amRenderContainer->windowFlags() & ~Qt::Window) | Qt::Widget);
		ui->amRenderContainer->reParent();
		ui->actionFull_Screen->setEnabled(false);
	}
	else
	{
		ui->amRenderContainer->setWindowFlags((ui->amRenderContainer->windowFlags() & ~Qt::Widget & ~Qt::WindowStaysOnTopHint) | Qt::Window);
		ui->amRenderContainer->release();
		ui->actionFull_Screen->setEnabled(true);
	}

	ui->amRenderContainer->show();
}

void MainWindow::toggleFullScreen(bool checked)
{
	if (checked)
	{
		ui->amRenderContainer->setWindowState(Qt::WindowState::WindowMaximized);
		ui->amRenderContainer->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
		ui->amRenderContainer->show();
	}
	else
	{
		ui->amRenderContainer->setWindowState(Qt::WindowState::WindowNoState);
		ui->amRenderContainer->setWindowFlags(Qt::Window);
		ui->amRenderContainer->show();
	}
}

void MainWindow::playTest(void)
{
	if (ui->btnTestPlay->isChecked())
	{
		_perfMgr->play(true);
		ui->layoutSetlist->setEnabled(false);
		ui->frmMdlContainer->setEnabled(false);
		ui->frmAnimContainer->setEnabled(false);
		ui->frmSndContainer->setEnabled(false);
		ui->frmPlayback->setEnabled(false);
	}
	else
	{
		_perfMgr->stop();
		ui->layoutSetlist->setEnabled(true);
		ui->frmMdlContainer->setEnabled(true);
		ui->frmAnimContainer->setEnabled(true);
		ui->frmSndContainer->setEnabled(true);
		ui->frmPlayback->setEnabled(true);
	}
}

void MainWindow::play()
{
	_perfMgr->play(false);

	ui->layoutSetlist->setEnabled(false);
	ui->frmMdlContainer->setEnabled(false);
	ui->frmAnimContainer->setEnabled(false);
	ui->frmSndContainer->setEnabled(false);

	ui->frmAnimTest->setEnabled(false);
	ui->btnPlay->setEnabled(false);
	ui->btnStop->setEnabled(true);
}
void MainWindow::stop()
{
	_perfMgr->stop();

	ui->layoutSetlist->setEnabled(true);
	ui->frmMdlContainer->setEnabled(true);
	ui->frmAnimContainer->setEnabled(true);
	ui->frmSndContainer->setEnabled(true);

	ui->frmAnimTest->setEnabled(true);
	ui->btnPlay->setEnabled(true);
	ui->btnStop->setEnabled(false);
}

void MainWindow::options(bool checked)
{
	OptionsDialog *o = new OptionsDialog(this);
	o->show();
}

void MainWindow::disableModels()
{
	ui->cmbMdlList->clear();
	ui->edtMdlLoc->setText("");
	ui->nbrMdlOffset->setValue(0);
	ui->chkVisibleTest->setChecked(false);
	ui->chkVisibleTest->setEnabled(false);
	ui->nbrMdlOffset->setEnabled(false);
	ui->dialAnimOffset->setEnabled(false);
	ui->frmMdlContainer->setEnabled(false);
}
void MainWindow::disableAnimation()
{
	ui->lblAnimFileName->setText("");
	ui->edtAnimFile->setText("");
	ui->frmAnimContainer->setEnabled(false);
}
void MainWindow::disableAudio()
{
	ui->lblSndFileName->setText("");
	ui->edtSndFile->setText("");
	ui->nbrSndDelay->setValue(0);
	ui->frmSndContainer->setEnabled(false);
}