/************************************************************************
*	ACheckableMessageBox.h -- Qt checkable message box implmentation	*
*							  Copyright (c) 2014 Justin Byers			*
* ===================================================================== *
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
*************************************************************************/

#ifndef CHECKABLEMSB_H_
#define CHECKABLEMSB_H_

#include "ui_questionbox.h"

namespace Ui {
	class CheckableMessageBox;
}

class CheckableMessageBox : public QDialog
{
	Q_OBJECT
public:
	CheckableMessageBox(QWidget *parent = 0);
	~CheckableMessageBox();

	/*
		static CheckableMessageBox* newDoNotAskAgainQuestion()

		Parameters:			title		Dialog menu bar title
							msg			Message to display in dialog
		Returns:			A new "Yes/No/Do Not Ask Again" CheckableMessageBox
	*/
	static CheckableMessageBox* newDoNotAskAgainQuestion(QString title, QString msg);
	/*
		void setTitle()

		Set the dialog title

		Parameters:			title		Dialog menu bar title
	*/
	void setTitle(QString title);
	/*
		void setMessage()

		Set the dialog message

		Parameters:			msg			Message to display in dialog
	*/
	void setMessage(QString msg);
	/*
		bool shouldAskAgain()

		Was "Do Not Ask Again" checked?

		Returns:			bool		true=not checked,false=checked
	*/
	bool shouldAskAgain();
signals:
	void result(bool);
private slots:
	void yesBtn();
	void noBtn();
private:
	Ui::CheckableMessageBox *ui;
};

#endif