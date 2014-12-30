/************************************************************************
*	AM_PMD.h -- File parsing for PMD format								*
*				Copyright (c) 2014 Justin Byers							*
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

#ifndef PMD_H_
#define PMD_H_

#include "AM_Model.h"

#define COMMENT_SZ 256

/*
	Extends AM_Model to parse PMD models
*/
class AM_Pmd : public AM_Model {
private:
	/*Types of bones supported in PMD format*/
	typedef enum BoneType {Rotate=0,RotateMove,IKType,None,IKFollow,InfRo,IKFollowTail,Tail,ArmX,ArmInv};
	/*
		virtual bool parse()

		See definition in AM_Model
	*/
	virtual bool parse(char *file, char *dir, bool ext);

public:
	AM_Pmd();
	virtual ~AM_Pmd();
};

#endif