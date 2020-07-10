#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotGeometry.h"

using namespace EsChPlotGeometry;

double _2D::Cartesian::distanceGet(double x0, double y0, double x1, double y1)
{
	double dX = x1-x0;
	double dY = y1-y0;

	return sqrt(dX*dX+dY*dY);
}

bool _2D::Cartesian::pointsInVicinity(double x0, double y0, double x1, double y1, 
				double toleranceX, double toleranceY)
{
	return (abs(x0-x1) <= abs(toleranceX)) && 
		(abs(y0-y1) <= abs(toleranceY));
}