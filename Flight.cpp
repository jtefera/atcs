/*
 * Flight.cpp
 *
 *  Created on: 15/07/2014
 *      Author: paco
 *
 *  Copyright 2014 Francisco Martín
 *
 *  This file is part of ATCSim.
 *
 *  ATCSim is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ATCSim is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ATCSim.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Flight.h"

#include "GUI.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "Common.h"

#include <iostream>
#include <string>
#include <math.h>
#include "Airport.h"
#include "Storm.h"
#include "tiny_obj_loader.h"
#include <cassert>

//#include <vector>

Flight::~Flight() {
	// TODO Auto-generated destructor stub
}

Flight::Flight(std::string _id, Position _pos, float _bearing, float _inclination, float _speed)
{
	id = _id;
	pos = _pos;
	bearing = _bearing;
	inclination = _inclination;
	speed = _speed;
	route.clear();
	test = true;
	points = INIT_FLIGHT_POINTS;
	focused = false;
	timetocenter = 0;
	firstbear = true;
	accelerating = 0;
	rotating = false;
	inEmergency = false;
	emergencySetted = false;
	desspeed = _speed;
	inStorm = false;
	modelo.Load("../cessna.obj");

}

void
Flight::update(float delta_t)
{
	float trans;
	Position CPpos;
	float distanceneeded;
	float differenceOnSpeed;
	float desiredbearing;
	float errorbearing;
	float simtimemod = Airport::getInstance()->getSimTime();

	std::list<Storm*> storms = Airport::getInstance()->getStorms();
	std::list<Storm*>::iterator stIt;

	float changeinbearing;
	float beardenormalized;
	float desbeardenormalized;
	float timeneededtoacc;
	int signacc;
	float distanceneededtoacc;
	if(routed())
	{
		CPpos = route.front().pos;
		differenceOnSpeed = route.front().speed-speed;
		signacc = sign(differenceOnSpeed);
		float desbear;
		bool status;
		if(inEmergency && !emergencySetted)
		{
			planEmergency();
		}


		if(routed()&&firstbear)
		{
			CPpos = route.front().pos;
			pos.angles(CPpos, bearing, inclination);
			bearing = normalizePi(bearing + M_PI);
			firstbear = false;
		}
		if(routed())
		{
			CPpos = route.front().pos;
			pos.angles(CPpos, desbear, inclination);
			desbear = normalizePi(desbear + M_PI);
			status = (bearing<desbear) && (bearing > normalizePi(desbear-M_PI));

			if(abs(desbear-bearing)<0.01)
			{
				bearing = desbear;
				rotating = false;
			} else{
				bearing = normalizePi(bearing + dirofrotation(bearing,desbear)*delta_t*simtimemod);

			}

		} else {
			inclination = 0.0;
		}

		if(abs(desspeed - speed)>10)
			speed = speed + sign(desspeed - speed)*MAXACC*delta_t*simtimemod;
		else
			speed = desspeed;

		if(test) {
			calculateNextPos();
			desspeed = route.front().speed;
		}
	}else
		inclination = 0.0;

	last_pos = pos;

	trans = speed * delta_t * simtimemod  ;


	pos.set_x(pos.get_x() + trans * cos(bearing) * cos(inclination));
	pos.set_y(pos.get_y() + trans * sin(bearing) * cos(inclination));
	pos.set_z(pos.get_z() + ( trans * sin(inclination)));

	if(pos.distance(last_pos) > pos.distance(CPpos))
	{
		route.pop_front();
	}

	Position planarPos(pos.get_x(),pos.get_y(),0.0);
	Position planarStormPos;
	float distanceInZ;
	//Check if in storm
	inStorm = false;
	for(stIt = storms.begin();stIt!=storms.end();++stIt)
	{
		planarStormPos.set_x((*stIt)->getPosition().get_x());
		planarStormPos.set_y((*stIt)->getPosition().get_y());
		planarStormPos.set_z(0.0);
		distanceInZ = pos.get_z()-(*stIt)->getPosition().get_z();
		if(planarPos.distance(planarStormPos)<(*stIt)->getRadius() &&  distanceInZ < (*stIt)->getAltitude())
		{
			inStorm = true;
			break;
		}
	}



	points = points - (1+inStorm+inEmergency)*delta_t*simtimemod;

}




void
Flight::planEmergency()
{
	//Plan de emergencia: Borra la ruta actual del avion y crea una solo con los de landing
	//Reasigna a todos los aviones que puedan entrar en conflicto una nueva ruta.
	std::list<Flight*> flights = Airport::getInstance()->getFlights();
	std::list<Flight*>::iterator it,it2,otherflight;
	std::cout<<"Recibido!"<<std::endl;

	route.clear();
	Position pos0(4000.0, 0.0, 680.0); //Posición antes de procedimiento de bajada
	Position pos1(2000.0, 0.0, 300.0); //Mitad de la bajada
	Position pos2(0.0, 0.0, 25.0); //Avión en suelo, principio de pista
	Position pos3(-750.0, 0.0, 25.0); //Final de pista
	Route r0, r1, r2, r3;
	r0.pos = pos0;
	r0.speed = 200;
	r1.pos = pos1;
	r1.speed = 100.0;
	r2.pos = pos2;
	r2.speed = 10.0;
	r3.pos = pos3;
	r3.speed = 15.0;
	route.push_back(r3);
	route.push_front(r2);
	route.push_front(r1);
	route.push_front(r0);

	float sectime = SECTIME;
	float timetoland = getTimeToCenter();
	for(it = flights.begin(); it != flights.end();++it)
	{
		if(abs((*it)->getTimeToCenter()-timetoland)<sectime && (*it)->getId()!=id)
		{
			(*it)->getRoute()->clear();
		}
	}
	emergencySetted = true;



}

long
Flight::TimeToPosIfInRoute(Position pos)
{
	std::list<NextPositions>::iterator nxposIt;
	for(nxposIt = nextroutes.begin();nxposIt != nextroutes.end(); ++nxposIt)
	{
		//std::cout <<"Flg.cpp isPosInRoute distance-> "<<pos.distance((*nxposIt).pos)<<std::endl;
		if(pos.distance((*nxposIt).pos)<1)
		{
			return (*nxposIt).timetoarrive;
		}
	}
	return 0;

}


long
Flight::getTimeToCenter()
{
	//Calcula el tiempo al centro
	std::list<Route>::iterator nxroute;

	long timetocen;
	timetocen = 0;
	Route pvroute;
	pvroute.pos = pos;
	pvroute.speed = speed;
	int numruta = 1;
	float distance,v0,vf;
	float velFinTramo; 	//En caso de que en el tramo calculado al avión no le de tiempo a desacelerar
						//en esta variable se guarda la velocidad al final del tramo. En caso de que
						//si de, guarda la velocidad final del punto
	float timebeforelanding;

	if(routed())
	{
		for(nxroute = route.begin();nxroute != route.end(); ++nxroute)
		{
			distance = pvroute.pos.distance((*nxroute).pos);
			v0 = pvroute.speed;
			vf = (*nxroute).speed;
			timetocen = timetocen + timeBetweenTwoRoutes(distance,v0,vf,MAXACC,velFinTramo);
			//std::cout <<"velfin y ruta fin "<<velFinTramo<<" "<<vf<<std::endl;
			pvroute.pos = (*nxroute).pos;
			//pvroute.speed = (*nxroute).speed;
			pvroute.speed = velFinTramo;
			timetocenter = timetocen;
			if(route.size()-numruta == 4)
			{
				timebeforelanding = timetocenter;
			}

			numruta++;
		}
		//std::cout <<"Landing Time fun "<< timetocenter-timebeforelanding<<std::endl;
		return timetocen;

	} else {
		//std::cout <<"Devolviendo " <<-1<<std::endl;
		return -1;
	}

}




void
Flight::calculateNextPos()
{
	if(route.size()>2)
	{
		nextpositions.clear();
		std::list<Route>::iterator routeIt;
		Route previousRoute;
		previousRoute.pos = pos;
		previousRoute.speed = speed;
		float distance;
		float dx,dy,dz;
		float newx,newy,newz;
		float nextSpeed = speed;
		float dbtwpoints = 1000;
		float nextBearing, nextInclination;
		NextPositions nextpos;
		Route newRoute;
		int npoints;
		int i;
		float firstdistance, ratiofirstdistance;
		for(routeIt = route.begin(); routeIt != route.end(); ++routeIt)
		{
			distance = previousRoute.pos.distance((*routeIt).pos);
			previousRoute.pos.angles((*routeIt).pos,nextBearing,nextInclination);
			dx=dbtwpoints*cos(nextBearing)*cos(nextInclination);
			dy=dbtwpoints*sin(nextBearing)*cos(nextInclination);
			dz=dbtwpoints*sin(nextInclination);

			npoints = int(distance/dbtwpoints);
			for(i=0;i<=npoints;i++)
			{
				//std::cout <<i<<std::endl;
				if(i==0)
				{
					firstdistance = distance-npoints*dbtwpoints;
					ratiofirstdistance = firstdistance/dbtwpoints;
					newx = previousRoute.pos.get_x()-ratiofirstdistance*dx;
					newy = previousRoute.pos.get_y()-ratiofirstdistance*dy;
					newz = previousRoute.pos.get_z()+ratiofirstdistance*dz;
				} else {
					newx = previousRoute.pos.get_x()-dx;
					newy = previousRoute.pos.get_y()-dy;
					newz = previousRoute.pos.get_z()+dz;
				}
				previousRoute.pos.set_x(newx);
				previousRoute.pos.set_y(newy);
				previousRoute.pos.set_z(newz);
				nextpos.pos = previousRoute.pos;
				nextpos.timetoarrive = i;
				nextpositions.push_back(nextpos);

			}
		}

	}
}

float
Flight::getTimeToPoint(Route rte, float &velfin)
{
	//std::cout 	<<"Fl.cpp getTimeTopoint,xpoint ->"<< point.distance(pos)/speed<<" x-> "<<point.get_x() <<std::endl;

	return timeBetweenTwoRoutes(rte.pos.distance(pos),speed,rte.speed,MAXACC,velfin);
}




void
Flight::draw()
{



	glPushMatrix();

	//Draw Aeroplane
	glTranslatef(pos.get_x(), pos.get_y(), pos.get_z());
	glRotatef(toDegrees(bearing), 0.0f, 0.0f, 1.0f);
	glRotatef(toDegrees(-inclination)-15, 0.0f, 1.0f, 0.0f);
	//CARGANDO EL CESSNA
	glPushMatrix();
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		glRotatef(180, 0.0f, 1.0f, 0.0f);
		glScalef(3,3,3);
		modelo.Draw();
	glPopMatrix();

	//Draw Collision sphere

	if(focused && !inEmergency)
		glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
	else if(inEmergency)
		glColor4f(1.0f,0.0f,0.0f, 0.1f);
	else
		glColor4f(1.0f,1.0f,1.0f, 0.1f);


	GLUquadric *quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);
	gluSphere( quadratic, COLLISION_DISTANCE/2.0f, 32, 32);
	glPopMatrix();

	if(focused)
	{
		std::list<Route>::iterator it;

		TextDisplay *textDisplay = TextDisplay::getInstance();
		char pos_str[255];

		textDisplay->displayText((char*)id.c_str(), 15, 75, GUI::win_width, GUI::win_height, LIGHTBLUE, GLUT_BITMAP_HELVETICA_18);
		snprintf(pos_str, 255, "Points: %5.5lf ", points);
		textDisplay->displayText(pos_str, 15, 95, GUI::win_width, GUI::win_height, YELLOW, GLUT_BITMAP_HELVETICA_12);

		snprintf(pos_str, 255, "Position: (%lfaprobar ultimo examen , %lf, %lf) m", pos.get_x(), pos.get_y(), pos.get_z());
		textDisplay->displayText(pos_str, 15, 115, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
		snprintf(pos_str, 255, "Bearing: %lf deg", toDegrees(bearing));
		textDisplay->displayText(pos_str, 15, 135, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
		snprintf(pos_str, 255, "Inclination: %lf deg", toDegrees(inclination));
		textDisplay->displayText(pos_str, 15, 155, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
		snprintf(pos_str, 255, "Speed: %lf m/sec", speed);
		textDisplay->displayText(pos_str, 15, 175, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
		snprintf(pos_str, 255, "Timetocenter: %li sec", getTimeToCenter());
		textDisplay->displayText(pos_str, 15, 195, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);


		if(!route.empty())
		{

			glColor4f(0.0f,0.0f,1.0f, 1.0f);
			glBegin(GL_LINES);

			glVertex3f(pos.get_x(), pos.get_y(), pos.get_z());
			for(it = route.begin(); it!=route.end(); ++it)
			{
				glVertex3f((*it).pos.get_x(), (*it).pos.get_y(), (*it).pos.get_z());
				glVertex3f((*it).pos.get_x(), (*it).pos.get_y(), (*it).pos.get_z());
			}
			glEnd();

			for(it = route.begin(); it!=route.end(); ++it)
			{
				glPushMatrix();
				glTranslatef((*it).pos.get_x(), (*it).pos.get_y(),(*it).pos.get_z());
				GLUquadric *quadratic = gluNewQuadric();
				gluQuadricNormals(quadratic, GLU_SMOOTH);
				gluQuadricTexture(quadratic, GL_TRUE);
				gluSphere( quadratic, 5.0f, 32, 32);
				glPopMatrix();
			}


			textDisplay->displayText((char *)"Route", 15, 230, GUI::win_width, GUI::win_height, BLUE, GLUT_BITMAP_HELVETICA_12);

			int c = 0;
			for(it = route.begin(); it!=route.end(); ++it)
			{
				snprintf(pos_str, 255, "Position: (%lf, %lf, %lf) %lf m", (*it).pos.get_x(), (*it).pos.get_y(), (*it).pos.get_z(), (*it).speed);
				textDisplay->displayText(pos_str, 25, 250+(20*c), GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
				c++;
			}

		}
		if(!nextpositions.empty())
				{
					std::list<NextPositions>::iterator itNextPos;
					glColor4f(0.0f,1.0f,0.0f, 1.0f);
					glBegin(GL_LINES);

					glVertex3f(pos.get_x(), pos.get_y(), pos.get_z());
					for(itNextPos = nextpositions.begin(); itNextPos!=nextpositions.end(); ++itNextPos)
					{
						glVertex3f((*itNextPos).pos.get_x(), (*itNextPos).pos.get_y(), (*itNextPos).pos.get_z());
						glVertex3f((*itNextPos).pos.get_x(), (*itNextPos).pos.get_y(), (*itNextPos).pos.get_z());
					}
					glEnd();
					float colr = 0;
					float colg = 0;
					float colb = 1;
					for(itNextPos = nextpositions.begin(); itNextPos!=nextpositions.end(); ++itNextPos)
					{
						glColor4f(colr,colg,colb, 1.0f);
						glPushMatrix();
						glTranslatef((*itNextPos).pos.get_x(), (*itNextPos).pos.get_y(),(*itNextPos).pos.get_z());
						GLUquadric *quadratic = gluNewQuadric();
						gluQuadricNormals(quadratic, GLU_SMOOTH);
						gluQuadricTexture(quadratic, GL_TRUE);
						gluSphere( quadratic, 100.0f, 32, 32);
						glPopMatrix();
						colr = colr+0.1;
						colg = colg+0.1;
						colb = colb-0.1;
					}

				}

	}




}
