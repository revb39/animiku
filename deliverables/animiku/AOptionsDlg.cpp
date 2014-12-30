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

#include "AOptionsDlg.h"
#include "defaults.h"
#include "AM_Screen.h"
#include <Windows.h>
#include <sstream>
#include <qmessagebox>

OptionsDialog::OptionsDialog(QWidget *parent) : QDialog(parent),ui(new Ui::OptionsDialog)
{
	ui->setupUi(this);
	loadFromRegistry();

	QObject* ok = (QObject*)ui->btnWndButtons->button(QDialogButtonBox::Ok);
	QObject* restoreDefaults = (QObject*)ui->btnWndButtons->button(QDialogButtonBox::RestoreDefaults);
	QObject::connect(ok, SIGNAL(clicked()), this, SLOT(ok()));
	QObject::connect(restoreDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
}

OptionsDialog::~OptionsDialog()
{
	delete ui;
}

void OptionsDialog::ok()
{
	//open the key
	HKEY key = NULL;
	RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\AniMiku", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL);
	HRESULT hr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\AniMiku", NULL, KEY_ALL_ACCESS, &key);
	if (key == NULL)
		return;

	setValueInRegistry(key, "fade time", ui->nbrFadeTime->text().toStdString());
	setValueInRegistry(key, "gravity", ui->nbrGravity->text().toStdString());
	setValueInRegistry(key, "fps", ui->nbrFps->text().toStdString());
	setValueInRegistry(key, "save cam on exit", QString::number(ui->cmbCamExit->currentIndex()).toStdString());

	setValueInRegistry(key, "cam move spd", ui->nbrCamMoveSpd->text().toStdString());
	setValueInRegistry(key, "cam zoom spd", ui->nbrCamZoomSpd->text().toStdString());
	setValueInRegistry(key, "ssao", QString::number(ui->chkSSAO->isChecked()).toStdString());
	setValueInRegistry(key, "ssao int", ui->nbrSSAOInt->text().toStdString());
	setValueInRegistry(key, "shadows", QString::number(ui->chkShadows->isChecked()).toStdString());

	setValueInRegistry(key, "resolution", ui->cmbResolution->currentText().toStdString());
	setValueInRegistry(key, "aa mode", ui->cmbAAMode->currentText().toStdString());

	setValueInRegistry(key, "ssao rad", ui->nbrSSAORad->text().toStdString());
	setValueInRegistry(key, "ssao scl", ui->nbrSSAOScl->text().toStdString());
	setValueInRegistry(key, "ssao bias", ui->nbrSSAOBias->text().toStdString());
	setValueInRegistry(key, "ssao samples", ui->nbrSSAOSamp->text().toStdString());
	setValueInRegistry(key, "minimal", QString::number(ui->chkMinimal->isChecked()).toStdString());
	setValueInRegistry(key, "console", QString::number(ui->chkConsole->isChecked()).toStdString());

	RegCloseKey(key);

	//put new settings into engine
	AMSettings *s = TheScreen.getSettings();
	s->Sets.FadeLength = ui->nbrFadeTime->value();
	s->Sets.Gravity = ui->nbrGravity->value();
	s->Sets.FPS = ui->nbrFps->value();

	s->Sets.SSAO = ui->chkSSAO->isChecked();
	s->Sets.SSAOInten = ui->nbrSSAOInt->value()/100.0f;
	s->Sets.Shadows = ui->chkShadows->isChecked();

	std::string res = ui->cmbResolution->currentText().toStdString();
	s->Sets.Width = atoi(res.substr(0, res.find('x')).c_str());
	s->Sets.Height = atoi(res.substr(res.find('x')+1, res.size()-res.find('x')).c_str());

	std::string aa = ui->cmbAAMode->currentText().toStdString();
	if (aa == "None")
	{
		s->Sets.Samples = 1;
		s->Sets.EnableMSAA = false;
	}
	else
	{
		s->Sets.Samples = atoi(aa.substr(0,aa.find('x')).c_str());
		s->Sets.EnableMSAA = true;
	}

	s->Sets.SSAORad = ui->nbrSSAORad->value();
	s->Sets.SSAOBias = ui->nbrSSAOBias->value();
	s->Sets.SSAOScl = ui->nbrSSAOScl->value();
	s->Sets.SSAOSamples = ui->nbrSSAOSamp->value();
	s->Sets.Minimal = ui->chkMinimal->isChecked();
	s->Sets.DevConsole = ui->chkConsole->isChecked();
}
void OptionsDialog::restoreDefaults()
{
	loadDefaults();
	QMessageBox::information(this, "Notice", "All settings returned to default.");
}


void OptionsDialog::getResolutions()
{
	ui->cmbResolution->clear();

	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);
	for( int iModeNum = 0; EnumDisplaySettings( NULL, iModeNum, &dm ) != 0; iModeNum++ ) 
	{
		std::stringstream ss;
		ss << dm.dmPelsWidth;
		ss << "x";
		ss << dm.dmPelsHeight;
		if (ui->cmbResolution->findText(QString::fromStdString(ss.str())) != -1)
			continue;
		ui->cmbResolution->addItem(QString::fromStdString(ss.str()));
	}
}

void OptionsDialog::setValueInRegistry(HKEY key, std::string id, std::string value)
{
	RegSetValueEx(key, id.c_str(), NULL, REG_SZ, (BYTE*)value.c_str(), value.size());
}

std::string OptionsDialog::getValueFromRegistry(HKEY key, std::string value, std::string def)
{
	DWORD type = REG_SZ;
	DWORD sz = 0;
	char* data;
	RegQueryValueEx(key, value.c_str(), NULL, &type, NULL, &sz);
	data = new char[sz];
	HRESULT hr = RegQueryValueEx(key, value.c_str(), NULL, &type, (LPBYTE)data, &sz);

	if (hr != ERROR_SUCCESS)
	{
		delete[] data;
		return def;
	}

	std::string o (data);
	delete[] data;

	return o;
}

void OptionsDialog::loadFromRegistry()
{
	getResolutions();
	loadDefaults();

	//open the key
	HKEY key = NULL;
	HRESULT hr = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\AniMiku", NULL, KEY_READ, &key);
	if (key == NULL)
		return;

	//general settings
	ui->nbrFadeTime->setValue(atof(getValueFromRegistry(key, "fade time", DEF_FADE_TIME).c_str()));
	ui->nbrGravity->setValue(atof(getValueFromRegistry(key, "gravity", DEF_GRAVITY).c_str()));
	ui->nbrFps->setValue(atof(getValueFromRegistry(key, "fps", DEF_FPS).c_str()));
	ui->cmbCamExit->setCurrentIndex(atoi(getValueFromRegistry(key, "save cam on exit", DEF_CAM_EXIT).c_str()));

	ui->nbrCamMoveSpd->setValue(atof(getValueFromRegistry(key, "cam move spd", DEF_CAM_MOVE_SPD).c_str()));
	ui->nbrCamZoomSpd->setValue(atof(getValueFromRegistry(key, "cam zoom spd", DEF_CAM_ZOOM_SPD).c_str()));
	ui->chkSSAO->setChecked(atoi(getValueFromRegistry(key, "ssao", DEF_SSAO).c_str()));
	ui->nbrSSAOInt->setValue(atof(getValueFromRegistry(key, "ssao int", DEF_SSAO_INT).c_str()));
	ui->chkShadows->setChecked(atoi(getValueFromRegistry(key, "shadows", DEF_SHADOWS).c_str()));

	//graphics settings
	std::string res = getValueFromRegistry(key, "resolution", DEF_RESOLUTION);
	if (ui->cmbResolution->findText(QString::fromStdString(res)) != -1)
		ui->cmbResolution->setCurrentText(QString::fromStdString(res));
	else
		ui->cmbResolution->setCurrentIndex(0);

	std::string aa = getValueFromRegistry(key, "aa mode", DEF_AA);
	if (ui->cmbAAMode->findText(QString::fromStdString(aa)) != -1)
		ui->cmbAAMode->setCurrentText(QString::fromStdString(aa));
	else
		ui->cmbAAMode->setCurrentText(DEF_AA);

	//advanced settings
	ui->nbrSSAORad->setValue(atof(getValueFromRegistry(key, "ssao rad", DEF_SSAO_RAD).c_str()));
	ui->nbrSSAOScl->setValue(atof(getValueFromRegistry(key, "ssao scl", DEF_SSAO_SCL).c_str()));
	ui->nbrSSAOBias->setValue(atof(getValueFromRegistry(key, "ssao bias", DEF_SSAO_BIAS).c_str()));
	ui->nbrSSAOSamp->setValue(atoi(getValueFromRegistry(key, "ssao samples", DEF_SSAO_SAMPLES).c_str()));
	ui->chkMinimal->setChecked(atoi(getValueFromRegistry(key, "minimal", DEF_MINIMAL).c_str()));
	ui->chkConsole->setChecked(atoi(getValueFromRegistry(key, "console", DEF_CONSOLE).c_str()));

	RegCloseKey(key);
}

void OptionsDialog::loadDefaults()
{
	ui->nbrFadeTime->setValue(atof(DEF_FADE_TIME));
	ui->nbrGravity->setValue(atof(DEF_GRAVITY));
	ui->nbrFps->setValue(atof(DEF_FPS));
	ui->cmbCamExit->setCurrentIndex(atoi(DEF_CAM_EXIT));

	ui->nbrCamMoveSpd->setValue(atof(DEF_CAM_MOVE_SPD));
	ui->nbrCamZoomSpd->setValue(atof(DEF_CAM_ZOOM_SPD));
	ui->chkSSAO->setChecked(atoi(DEF_SSAO));
	ui->nbrSSAOInt->setValue(atof(DEF_SSAO));
	ui->chkShadows->setChecked(atoi(DEF_SHADOWS));
	
	if (ui->cmbResolution->findText(DEF_RESOLUTION) != -1)
		ui->cmbResolution->setCurrentText(DEF_RESOLUTION);
	else
		ui->cmbResolution->setCurrentIndex(0);

	ui->cmbAAMode->setCurrentText(DEF_AA);

	ui->nbrSSAORad->setValue(atof(DEF_SSAO_RAD));
	ui->nbrSSAOScl->setValue(atof(DEF_SSAO_SCL));
	ui->nbrSSAOBias->setValue(atof(DEF_SSAO_BIAS));
	ui->nbrSSAOSamp->setValue(atof(DEF_SSAO_SAMPLES));
	ui->chkMinimal->setChecked(atoi(DEF_MINIMAL));
	ui->chkConsole->setChecked(atoi(DEF_CONSOLE));
}