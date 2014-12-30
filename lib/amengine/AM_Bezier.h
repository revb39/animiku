/************************************************************************
*	AM_Bezier.h -- Bezier curve solver for motion interpolation			*
*				   Copyright (c) 2014 Justin Byers						*
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

#ifndef _BEZIER_H_
#define _BEZIER_H_
#include "AMTypes.h"

/*Tolerance to end newton's method iterations*/
#define TOLERANCE 0.0001f
/*Max # of iterations to use in calculation*/
#define MAX_ITERATION 150

/*
	Solver for bezier curve keyframe interpolation
	Uses Newton's Method to estimate solution to curve
*/
class AMBezier {
private:
	/*
		static float cubeX()

		Solves a cubic polynomial ax^3+bx^2+cx+d

		Parameters:		x		x value to solve poly for
						a-d		coefficients in poly

		Returns:		float	y value of equation
			
	*/
	static float cubeX (float x, float a, float b, float c, float d);

	/*
		static float cubeDerivX()

		Solves the derivative of a cubic polynomial 3ax^2+2bx+c

		Parameters:		x		x value to solve deriv for
						a-c		coefficients in deriv

		Returns:		float	y value of equation
	*/
	static float cubeDerivX(float x, float a, float b, float c);

public:
	/*
		static float solveForY()

		Solves a bezier curve for the y value at an x provided points 1 & 2.
		Points 0 & 3 are defined as (0,0) and (1,1) respectively by the MMD spec.

		Parameters:		x		x value to solve for
						x1,y1	coordinates scaled 0.0-1.0 of curve point 1
						x2,y2	coordinates scaled 0.0-1.0 of curve point 2

		Returns:		float	estimated y value of curve at x
	*/
	static float solveForY(float x, float x1, float y1, float x2, float y2);
};
#endif