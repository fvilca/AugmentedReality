#ifndef PATTERNPOINT_CPP
#define PATTERNPOINT_CPP

#include "PatternPoint.h"

template <class T>
PatternPoint<T>::PatternPoint()
{
}

template <class T>
PatternPoint<T>::PatternPoint(T_INT n)
{
	Init(n);
}

template <class T>
PatternPoint<T>::PatternPoint(vector< RotatedRect > box)
{
	Point2f point;
	Size radio;

	for (int i = 0; i < box.size(); i++) {
		point = box.center;
		radio = box.size;
		points.push_back(point);
		radios.push_back(radio);
	}
	Init(points.size());
}

template <class T>
PatternPoint<T>::PatternPoint(vector<Point2f> p)
{
	points = p;	
	Init(points.size());
}


template <class T>
void PatternPoint<T>::Init(T_INT n)
{	
	nPoints=n;
	distances.Init(n,n);
	minDistances.Init(n,1);
	maxRadio=0;
	maxDistance=0;
	if (radios.size() == 0)
		for (T_INT i = 0; i < nPoints; i++)
			radios.push_back(Size(0, 0));
}

// Calcular distancias de todos los puntos
template <class T>
void PatternPoint<T>::Calculate()
{
	Point2f p1, p2;	
	T_DOUBLE	dmin = 10000,// variable para la distancia minima, las distancias siempre son positivas
				dist1, dist2;
	maxRadio = 0;
	maxDistance = 0;

	FindCentroid();

	for (T_INT i = 0; i < nPoints; i++){
		
		dist2 = norm(centroid - points[i]);
		if (dist2 > maxDistance)
			maxDistance = dist2;
				
		dmin = 10000;		
		for (T_INT j = i; j < nPoints; j++){
			if (i == j)
				distances.Set(i, j, 0.0);
			else{
				p1 = points[i];
				p2 = points[j];
				dist1 = norm(p1 - p2);// sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
				distances.Set(i, j, dist1);
				distances.Set(j, i, dist1);
				if (dist1 < dmin)
					dmin = dist1;
			}
		}
		maxRadio += MAX(radios[i].width, radios[i].height);
		minDistances.Set(i, 0, dmin);
	}
	maxRadio /= nPoints; // es el grosor promedio de las esferas
}

template <class T>
Point2f PatternPoint<T>::FindCentroid()
{	
	T_DOUBLE mediax = 0, mediay = 0;
	mediax = 0, mediay = 0;
	for (T_INT i = 0; i < nPoints; i++){
		centroid = points[i];
		mediax += centroid.x;
		mediay += centroid.y;
	}
	mediax /= nPoints;
	mediay /= nPoints;
	centroid.x = mediax;
	centroid.y = mediay;
	return centroid;
}

template <class T>
Point2f PatternPoint<T>::FindCentroid(vector<Point2f> pts)
{	
	Point2f ctr;
	T_DOUBLE mediax = 0, mediay = 0;
	mediax = 0, mediay = 0;
	for (T_INT i = 0; i < pts.size(); i++) {
		ctr = pts[i];
		mediax += ctr.x;
		mediay += ctr.y;
	}
	mediax /= pts.size();
	mediay /= pts.size();
	ctr.x = mediax;
	ctr.y = mediay;
	return ctr;
}

template <class T>
PatternPoint<T>::~PatternPoint()
{
}

#endif // !PATTERNPOINT_CPP