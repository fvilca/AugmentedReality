#pragma once

#include "ContourFinder.h"

class ElipseFinder:public ContourFinder{

public:
	
	Distance<T_DOUBLE> initialBoxes;
	Distance<T_DOUBLE> finalBoxes;

	ElipseFinder():ContourFinder() { }
	~ElipseFinder() {}
		
	//esta parte es de acuerdo a la forma del contorno

	bool VerifyContourShape(vector<Point> contour)
	{
		double dist1, dist2;
		Point2f pini,
			pfin;
		size_t count = contour.size();

		pini = contour[count*0.25];	pfin = contour[count*0.75];
		dist1 = norm(pini- pfin);

		pini = contour[0];	pfin = contour[count*0.5];
		dist2 = norm(pini- pfin);

		if (dist1 < 4 || dist2 < 4)
			return false; // se descarta
		return true; // se procesa contorno
	}
	
	// ajustar contornos a elipses
	RotatedRect FitContourToEllipse(vector<Point> contour)
	{
		Mat pointsf;
		Mat(contour).convertTo(pointsf, CV_32F);
		return fitEllipse(pointsf);
	}


	//Verifica que la elipse detectada no sea muy achatada
	bool VerifyEllipseNoEnlarged(RotatedRect box)
	{
		if (MIN(box.size.width, box.size.height) / MAX(box.size.width, box.size.height) < 0.25)
			return false; // se descarta
		return true;// se procesa contorno
	}

	// Verifica si se un contorno se asemeja a una elipse
	bool  VerifyEllipseShaped(RotatedRect box, Mat &cimage)
	{

		Point2f vtx[4];
		box.points(vtx);
		Point2f vtxq[4];
		int  v;
		for (v = 0; v < 3; v++)
		{
			vtxq[v].x = (vtx[v].x + vtx[v + 1].x) / 2;
			vtxq[v].y = (vtx[v].y + vtx[v + 1].y) / 2;
		}
		vtxq[v].x = (vtx[0].x + vtx[v].x) / 2;
		vtxq[v].y = (vtx[0].y + vtx[v].y) / 2;

		//verificar si alguno de los puntos se intersectan con el rectangulo
		int  nc = 5, // se busca en una grilla 3 x 3
			fnc, // badera que indica que encontro un punto blanco
			cn = 0,
			cm = nc / 2;
		for (register int  j = 0; j < 4; j++)
		{
			//buscar un punto blanco en el contorno del punto que intersecta
			fnc = 0;

			if (vtxq[j].y - cm >= 0 &&
				vtxq[j].y + cm < cimage.rows &&
				vtxq[j].x - cm >= 0 &&
				vtxq[j].x + cm < cimage.cols) // comprobar qu esta dentro dela imagen
			{
				for (int k = 0; k < nc; k++)
					for (int l = 0; l < nc; l++)
						if (cimage.at<Vec3b>(vtxq[j].y + k - cm, vtxq[j].x + l - cm)[0] == 255)
						{
							fnc = 1;
							goto AQUI;
						}
			AQUI:
				if (fnc)
					cn++;
			}
			else
				break;
			//imshow("...", cimage);
			//cv::waitKey(10);
		}

		if (cn < 4)
			return false; // se desctarta
		return true;
	}
	
	// Elegir las Elipses finales, rpimer algoritmo, ademas se eligen posisiones unicas
	
	vector<RotatedRect> ElegirElipsesFinales(vector< RotatedRect > boxElegidos, h_Matriz<double> distancias, T_DOUBLE &dmax)
	{
		//vector< RotatedRect > boxFinal;
		//T_INT nbelegidos = boxElegidos.size();
				
		T_INT insertable = 0;
		for (int i = 0; i < initialBoxes.nPoints; i++)
		{
			insertable = 0;
			for (int j = 0; j < initialBoxes.nPoints; j++){
				if (i != j)
					if (distancias.Get(i, j) <= dmax*3.5)// && distancias.Get(i, j) > dmax*1.5)
						insertable++;
				if (insertable == 3)
					break;
			}
			if (insertable == 3)
				boxFinal.push_back(boxElegidos[i]);
		}
		
		//[2]
		int varCenter = 3;
		vector< RotatedRect > box;
		double distance = 0, x, y, cont;
		for (register int i = 0; i < boxFinal.size(); ++i) {
			Point2f nuevo = boxFinal[i].center;
			x = 0;
			y = 0;
			cont = 1;
			for (register int j = 0; j < boxFinal.size(); ++j) {
				if (i != j) {
					Point2f temp = boxFinal[j].center;
					distance = norm(nuevo- temp);

					if (distance <= varCenter &&  boxFinal[j].center.x != 0){						
						x += boxFinal[j].center.x;
						y += boxFinal[j].center.y;
						boxFinal[j].center = Point2f(0, 0);
						cont++;
					}
				}
			}
			
			x += boxFinal[i].center.x;
			y += boxFinal[i].center.y;
			//cout <<"x: "<< x <<" y: "<< y <<" cont: "<< cont<<endl;
			boxFinal[i].center.x = x / cont;
			boxFinal[i].center.y = y / cont;
		}

		for (register int i = 0; i < boxFinal.size(); i++)
			(boxFinal[i].center.x != 0) ? box.push_back(boxFinal[i]) : 0;
		
		//cout << "tamaño del box final jojojojojo.... "<<box.size()<<endl;		
		//[2]!
		
		return boxFinal=box;
	}

	//Segundo algoritmo	, elegir los puntos mas cercanos al ca, que es el centroide del patron anterior
	vector<RotatedRect> ElegirElipsesFinales2(vector< RotatedRect > boxElegidos, Point2f &ca, T_DOUBLE &dmax)
	{
		vector< RotatedRect > boxFinal;
		T_INT nbelegidos = boxElegidos.size();
		T_INT insertable = 0;
		//cout << "dmax:" << dmax * 8<<endl;
		
		for (int i = 0; i < nbelegidos; i++){
			//cout << norm(boxElegidos[i].center- ca);
			if(norm(boxElegidos[i].center- ca)<dmax*1.15){
				// para mejorar	
				boxFinal.push_back(boxElegidos[i]);
				//cout << "  ok ";
			}
			//cout << endl;
		}

		//cout << boxFinal.size()<<endl<<endl;
		//if (boxFinal.size()!=30)
		//	getchar();
		
		return boxFinal;
	}
	
	//Obtiene las elipses necesarias
	bool processFrame(cv::Mat& in, cv::Mat& out, int &frame_counter, Point2f &ca, double &distMaxPatron, vector< vector<Point> > &contours, vector< RotatedRect > &boxElegidos, vector< RotatedRect > &boxFinal, int tipo, int &NroPuntos, double  &framesPerdidos, double &dmax) {
		// Matriz para obtener los contornos semjantes a 
		Mat cimage = Mat::zeros(in.size(), CV_8UC3); 
		out= Mat::zeros(in.size(), CV_8UC3);
		vector< RotatedRect > boxFinaltmp;

		// Rectangulo que contine a las elipses ajustadas a los contronos que mas se asemejan a una elipse		
		for (size_t i = 0; i < contours.size(); i++){
			size_t count = contours[i].size();
			// Contornos muy pequenos, se eliminan
			if (count < 10)	continue;// se descarta

			//esta parte es de acuerdo a la forma del contorno, si son muy achatados			
			if (tipo == SIMETRICO)
				if (!VerifyContourShape(contours[i])) continue; // se descarta
			 
			//Ajustar Contorno a Elipse se obtiene u  box que circunscribe a la elipse
			RotatedRect box = FitContourToEllipse(contours[i]);

			if (!VerifyEllipseNoEnlarged(box)) continue; // se descarta

			//dibujar el contorno i
			drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);

			// debo encontrar los puntos que se intersectan las elipses con los rectangulos
			if (!VerifyEllipseShaped(box, cimage)) continue; // se descarta

			// Se inserta box elegido
			boxElegidos.push_back(box);
		}// hasta aca se tiene todos los contornos que mas se asemejan a una elipse
		
		

		//Dibujar las elipses elegidas
		for (int i = 0;i < boxElegidos.size();i++)
			circle(out, boxElegidos[i].center, boxElegidos[i].size.width/2, Scalar(255, 125, 0), 1);
		if (frame_counter > 1)
			circle(out, ca, distMaxPatron*1.15, Scalar(125, 125, 255), 1);
		
		//imshow("Elipses detectadas", out);
		//imshow("bordes detectados", cimage);


		int nbelegidos = boxElegidos.size();

		// Solo si hay la cantidad de puntos necesarios, si son menos no se hace nada, se pierde el frame
		if (nbelegidos >= NroPuntos) {
			double  dmin;
			Distance<T_DOUBLE> distancias(boxElegidos);
								
			distancias.Calculate();
			dmax = distancias.maxRadio;
			
			// Aca es donde descarto a los puntos que no me son utiles
			boxFinaltmp = ElegirElipsesFinales(boxElegidos, distancias.distances, dmax);
			// luego si hay muchos puntos entonces solo me quedo con los necesarios
			
			//cout << "llegueeeeeeeeee......." << frame_counter<<" - "<<boxFinaltmp.size() << endl;
			
			if (frame_counter > 1 && boxFinaltmp.size() >= NroPuntos)			
			{				
				boxFinal = ElegirElipsesFinales2(boxFinaltmp, ca, distMaxPatron);			
			}
			else
				boxFinal = boxFinaltmp;
						//std::cout << " dmax: " << dmax << " tam: " << boxFinal.size() << std::endl;
			
			if (boxFinal.size() == NroPuntos)
				return true;
			else
				return false;
		}
		else 
			return false;		
	}
};