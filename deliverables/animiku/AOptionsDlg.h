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

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <Windows.h>
#include <qdialog.h>
#include "ui_options.h"

namespace Ui {
	class OptionsDialog;
}

class OptionsDialog : public QDialog
{
	Q_OBJECT
public:
	OptionsDialog(QWidget *parent = 0);
	~OptionsDialog();

	static std::string getValueFromRegistry(HKEY key, std::string value, std::string def);
	static void setValueInRegistry(HKEY key, std::string id, std::string value);
public slots:
	void ok();
	void restoreDefaults();
private:
	void getResolutions();
	void loadDefaults();
	void loadFromRegistry();

	Ui::OptionsDialog *ui;
};

#endif