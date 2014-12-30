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

#include "ACheckableMessageBox.h"

CheckableMessageBox::CheckableMessageBox(QWidget* parent) : QDialog(parent),ui(new Ui::CheckableMessageBox)
{
	ui->setupUi(this);
	setSizeGripEnabled(false);
}
CheckableMessageBox::~CheckableMessageBox()
{

}

CheckableMessageBox* CheckableMessageBox::newDoNotAskAgainQuestion(QString title, QString msg)
{
	CheckableMessageBox *msb = new CheckableMessageBox();
	msb->setTitle(title);
	msb->setMessage(msg);
	return msb;
}

void CheckableMessageBox::setTitle(QString title)
{
	setWindowTitle(title);
}
void CheckableMessageBox::setMessage(QString msg)
{
	ui->label->setText(msg);
	//ui->label->setMinimumSize(ui->label->minimumWidth(), ui->label->fontMetrics().boundingRect(ui->label->text()).height());
}
bool CheckableMessageBox::shouldAskAgain()
{
	return !ui->chkDoNotAsk->isChecked();
}

void CheckableMessageBox::yesBtn()
{
	emit result(true);
}
void CheckableMessageBox::noBtn()
{
	emit result(false);
}