/************************************************************************
*	AM_Bezier.cpp -- Bezier curve solver for motion interpolation		*
*					 Copyright (c) 2014 Justin Byers					*
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

#include "AM_Bezier.h"

float AMBezier::cubeX (float x, float a, float b, float c, float d)
{
	return (a*(x*x*x))+(b*(x*x))+(c*x)+d;
}
float AMBezier::cubeDerivX(float x, float a, float b, float c)
{
	return (3.0f*a*(x*x))+(2.0f*b*x)+c;
}
float AMBezier::solveForY(float x, float x1, float y1, float x2, float y2)
{
	D3DXVECTOR2 point0, point1, point2, point3;

	//equation is linear with slope=1 if x & y values match
	if (x1 == y1 && x2 == y2)
		return x;

	//cannot estimate solution if invalid coordinates provided
	//just return x value as if the equation was linear
	if (x1 > 1 || y1 > 1 || x2 > 1 || y2 > 1 || x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0)
		return x;

	//set up coordinates
	point1.x = x1;
	point1.y = y1;
	point2.x = x2;
	point2.y = y2;

	//create point 0 & 3 based on MMD spec
	point0 = D3DXVECTOR2(0, 0);
	point3 = D3DXVECTOR2(1, 1);

	//calculate the coefficients to use when solving the x cubic poly
	//cx = 3(x1-x0)
	float cx = 3.0f*(point1.x-point0.x);
	//bx = 3(x2-x1) - cx
	float bx = 3.0f*(point2.x-point1.x) - cx;
	//ax = x3 - x0 - cx - bx
	float ax = point3.x-point0.x-cx-bx;
	//dx = -x
	float dx = -x;

	//estimate the time constant to use to solve for y using newton's method
	float t = x; //guess the time constant at the provided x value
	int i=0;
	while (i <= MAX_ITERATION)
	{
		float n;
		float ddx = cubeDerivX(t, ax, bx, cx);
		if (ddx == 0)
			n = 0;
		else
			n = t - (cubeX(t, ax, bx, cx, dx)/ddx);

		//end if the solved value is within the tolerance
		if (fabsf(n-t) < TOLERANCE)
			break;

		t = n;
		i++;
	}

	//make sure the time constant is within the range 0.0-1.0
	if (t > 1)
		t = 1;
	else if (t < 0)
		t = -t;

	//calculate the coefficients to use when solving the y cubic poly
	//cy = 3(y1-y0)
	float cy = 3.0f*(point1.y-point0.y);
	//by = 3(y2-y2)-cy
	float by = 3.0f*(point2.y-point1.y)-cy;
	//ay = y3 - y0 - cy - by
	float ay = point3.y - point0.y - cy - by;

	//finally solve the curve for y at the estimated t
	float y = ay*(t*t*t) + by*(t*t) + cy*t + point0.y;

	//make sure the y is within the range 0.0-1.0
	if (y > 1)
		y = 1;
	else if (y < 0)
		y = 0;
	return y;
}