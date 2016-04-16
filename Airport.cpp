/*
 * Airport.cpp
 *
 *  Created on: 17/07/2014
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

#include "Airport.h"

#include <sys/time.h>
#include <stdio.h>

#include "GUI.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "Storm.h"
#include "Common.h"
#include <cstdlib>




Airport::Airport() {

	flights.clear();

	gettimeofday(&last_ts, NULL);
	crono = last_ts.tv_sec*1000000 + last_ts.tv_usec;

	final_pos.set_x(LANDING_POS_X);
	final_pos.set_y(LANDING_POS_Y);
	final_pos.set_z(LANDING_POS_Z);

	srand (time(NULL));
	sec = 0;
	points = INIT_POINTS;
	max_flights = INIT_MAX_FLIGHTS;
    SimTimeMod = 1.0;
    Storm *teststorm = new Storm(1);
    teststorms.push_back(teststorm);
    cronotot = 0;
    stormid = 0;
}

Airport::~Airport() {
	std::list<Flight*>::iterator it;
	for(it = flights.begin(); it!=flights.end(); ++it)
		delete(*it);
	flights.clear();
}

void
Airport::generate_flight()
{
	//std::cout <<"chorno----------------------"<<crono<<std::endl;
	float angle, x, y, z;
	float bear, inc;
	char id[6];

	angle = toRadians((float)(rand() % 360 - 180));

	x = AIRPORT_DISTANCE_MAX * cos(angle);
	y = AIRPORT_DISTANCE_MAX * sin(angle);
	z = FLIGHT_HEIGHT + (float)(rand() % 2000);

	Position ipos(x, y, z);
	Position pos0(0.0, 0.0, 0.0);

	pos0.angles(ipos, bear, inc);

	sprintf(id, "IB%4.4d", sec++);
	Flight *aux;
	aux = new Flight(id, ipos, bear, 0.0, 500.0);
	flights.push_back(aux);

	if(flights.size() == 1)
		NextFocus();
}

void
Airport::generate_storm()
{
	Storm *aux;
	aux = new Storm(stormid);
	stormid++;
	teststorms.push_back(aux);

}

void
Airport::NextFocus()
{
	if(DEBUG_MODE && DEBUG_STORMS)
	{
//		Storm *teststorm = new Storm(1);
//		teststorms.push_back(teststorm);
//		std::cout	<<"10,10, 5: "<<normalDist(10, 10, 0.2)<<std::endl
//					<<"0.1, 10, 5: "<<normalDist(9.9, 10, 0.2)<<std::endl
//					<<"0, 10, 5: "<<normalDist(10.1, 10, 0.2)<<std::endl
//					<<"........................"<<std::endl;
	}


	if(flights.empty()) return;

	if(flights.size() == 1)
	{
		focus = flights.begin();
		(*focus)->setFocused(true);
	}

	if(flights.size() > 1)
	{
		(*focus)->setFocused(false);

		if(++focus == flights.end())
			focus =  flights.begin();
	}else
		focus =  flights.begin();

	(*focus)->setFocused(true);
}
void Airport::Emergency()
{
	if(flights.empty()) return;
	else {
		(*focus)->setEmergency(true);
		std::cout<<"Emergencia Emergencia!!"<<std::endl;
	}
}

void
Airport::step()
{

	float delta_t;
	struct timeval tv;
	std::list<Flight*>::iterator it;
	std::list<Storm*>::iterator stormIt;
	long ta, tb;

	gettimeofday(&tv, NULL);

	ta = tv.tv_sec*1000000 + tv.tv_usec;
	tb = last_ts.tv_sec*1000000 + last_ts.tv_usec;

	delta_t = ((float)(ta-tb)) /1000000.0;
	last_ts = tv;

	//Crono que toma en cuenta las variaciones en la velocidad del tiempo
	cronotot = cronotot + delta_t*SimTimeMod*1000000.0;

	if((ta-crono)>INC_DIFF)
	{
		max_flights += INC_PEN;
		std::cerr<<"Increase flights in "<<INC_PEN<<" to "<<max_flights<<std::endl;
		crono = ta;
	}

	/*if((ta-cronoStorm)>nextStormTime)
	{


		nextStormTime=normalDist;
		cronoStorm = ta;
	}*/
	if(teststorms.size()<1)
	{
		if(abs(cronotot/1000000.0-cronoStorm)>200)
		{
			cronoStorm = cronotot/1000000.0;
			generate_storm();

		}
	}

	if(!flights.empty())
		for(it = flights.begin(); it!=flights.end(); ++it)
		{
			(*it)->update(SimTimeMod * delta_t);
			(*it)->draw();
		}

	checkLandings();
	checkCollisions();
	checkCrashes();
	removeOutStorm();

	if(!teststorms.empty())
	{
		for(stormIt = teststorms.begin(); stormIt!=teststorms.end(); ++stormIt)
		{
			(*stormIt)->update(SimTimeMod * delta_t);
		}

	}

	if(flights.size()<max_flights)
		generate_flight();
}
void
Airport::removeOutStorm()
{
	std::list<Storm*>::iterator stormit;
	std::list<Storm*>::iterator aux;
	Position center(0,0,0);
	for(stormit = teststorms.begin();stormit!=teststorms.end();++stormit)
	{
		if(abs((*stormit)->getPosition().get_x())>25000 || abs((*stormit)->getPosition().get_y())>25000)
		{
			std::cout<<"Removing stooooooooooooooorm"<<std::endl;
			delete(*stormit);
			stormit = teststorms.erase(stormit);
		}
	}

}

std::list<Flight*>::iterator
Airport::removeFlight(std::string id)
{
	std::list<Flight*>::iterator it;

	it=flights.begin();

	while(it !=  flights.end())
	{
		if((*it)->getId().compare(id) == 0)
		{
			if((*it)==(*focus))
			{
				delete(*it);
				focus = flights.erase(it);
				(*focus)->setFocused(true);
				return focus;
			}else
			{
				delete(*it);
				it = flights.erase(it);
				return it;
			}
		}
			it++;
	}
	return flights.begin();

}

void
Airport::checkCollisions()
{
	std::list<Flight*>::iterator i,j;
	bool removed = false;


	i =  flights.begin();

	while(i != flights.end())
	{
		j = i;

		if(j!=flights.end()) j++;

		while(j != flights.end())
		{
			if( (*i)->getPosition().distance((*j)->getPosition()) < COLLISION_DISTANCE)
			{
				std::cerr<<"Collision between "<<(*i)->getId()<<" and "<<(*j)->getId()<<std::endl;
				i = removeFlight((*i)->getId());
				j = removeFlight((*j)->getId());
				points += COLLISION_POINTS;
				return; //Avoid not valid iterator. Only one collision per cycle
			}
			j++;
		}
		i++;
	}
}


void
Airport::checkCrashes()
{
	std::list<Flight*>::iterator it;

	it = flights.begin();
	while(it != flights.end())
	{
		if((*it)->getPosition().get_z()<CRASH_Z)
		{
			std::cerr<<"[PoZ]Crash of "<<(*it)->getId()<<std::endl;
			it=removeFlight((*it)->getId());
			points += CRASH_HEIGHT_POINTS;
		}else if(toDegrees(fabs((*it)->getInclination())) > CRASH_INC)
		{
			std::cerr<<"[Inc] Crash of "<<(*it)->getId()<<std::endl;
			it = removeFlight((*it)->getId());
			points += CRASH_INC_POINTS;
		}else if( (*it)->getSpeed()<CRASH_SPEED)
		{
			std::cerr<<"[Spd] Crash of "<<(*it)->getId()<<std::endl;
			it = removeFlight((*it)->getId());
			points += CRASH_SPEED_POINTS;
		}else
			it++;
	}
}

void
Airport::checkLandings()
{
	std::list<Flight*>::iterator it;

	it = flights.begin();

	while(it != flights.end())
	{

		if((final_pos.distance((*it)->getPosition()) < LANDING_DIST) &&
				(toDegrees(normalizePi(fabs((*it)->getBearing() - toRadians(LANDING_BEAR))))<LANDING_BEAR_MAX_ERROR) &&
				((*it)->getSpeed()<LANDING_SPEED))
		{

			std::cerr<<"Flight "<<(*it)->getId()<<" landed"<<std::endl;
			it = removeFlight((*it)->getId());

			points += (int)(*it)->getPoints();
		}else
			it++;
	}

}

void
Airport::UpdateSimTime(float inc)
{
    SimTimeMod = SimTimeMod + inc;
    
    if(SimTimeMod < 0) SimTimeMod = 0;
}

void
Airport::draw()
{
	long ta, tb;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	ta = tv.tv_sec;

	glBegin(GL_QUADS);
	glColor3f(0.2f,0.4f,0.2f);
	glVertex3f( -AIRPORT_DISTANCE_MAX, AIRPORT_DISTANCE_MAX, 0.0f);
	glVertex3f(  AIRPORT_DISTANCE_MAX, AIRPORT_DISTANCE_MAX, 0.0f);
	glVertex3f(  AIRPORT_DISTANCE_MAX,-AIRPORT_DISTANCE_MAX, 0.0f);
	glVertex3f( -AIRPORT_DISTANCE_MAX,-AIRPORT_DISTANCE_MAX, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f,1.0f,0.0f);
	glVertex3f(  0.0f, -(LANDING_STRIP_W/2.0), 0.01f);
	glVertex3f(  0.0f,  LANDING_STRIP_W, 0.01f);
	glVertex3f( -LANDING_STRIP_L, LANDING_STRIP_W, 0.01f);
	glVertex3f( -LANDING_STRIP_L, -LANDING_STRIP_W, 0.01f);
	glEnd();

	glPushMatrix();
	glTranslatef(1.0f, 0.0f, 25.0f);
	GLUquadric *quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);
	gluSphere( quadratic, 25.0f, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0f,1.0f,0.0f);
	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	      // Top face (y = 1.0f)
	      // Define vertices in counter-clockwise (CCW) order with normal pointing out
	      glColor3f(0.0f, 1.0f, 0.0f);     // Green
	      glVertex3f( 1.0f, 1.0f, -1.0f);
	      glVertex3f(-1.0f, 1.0f, -1.0f);
	      glVertex3f(-1.0f, 1.0f,  1.0f);
	      glVertex3f( 1.0f, 1.0f,  1.0f);

	      // Bottom face (y = -1.0f)
	      glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	      glVertex3f( 1.0f, -1.0f,  1.0f);
	      glVertex3f(-1.0f, -1.0f,  1.0f);
	      glVertex3f(-1.0f, -1.0f, -1.0f);
	      glVertex3f( 1.0f, -1.0f, -1.0f);

	      // Front face  (z = 1.0f)
	      glColor3f(1.0f, 0.0f, 0.0f);     // Red
	      glVertex3f( 1.0f,  1.0f, 1.0f);
	      glVertex3f(-1.0f,  1.0f, 1.0f);
	      glVertex3f(-1.0f, -1.0f, 1.0f);
	      glVertex3f( 1.0f, -1.0f, 1.0f);

	      // Back face (z = -1.0f)
	      glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	      glVertex3f( 1.0f, -1.0f, -1.0f);
	      glVertex3f(-1.0f, -1.0f, -1.0f);
	      glVertex3f(-1.0f,  1.0f, -1.0f);
	      glVertex3f( 1.0f,  1.0f, -1.0f);

	      // Left face (x = -1.0f)
	      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	      glVertex3f(-1.0f,  1.0f,  1.0f);
	      glVertex3f(-1.0f,  1.0f, -1.0f);
	      glVertex3f(-1.0f, -1.0f, -1.0f);
	      glVertex3f(-1.0f, -1.0f,  1.0f);

	      // Right face (x = 1.0f)
	      glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	      glVertex3f(1.0f,  1.0f, -1.0f);
	      glVertex3f(1.0f,  1.0f,  1.0f);
	      glVertex3f(1.0f, -1.0f,  1.0f);
	      glVertex3f(1.0f, -1.0f, -1.0f);
	   glEnd();  // End of drawing color-cube
	glPopMatrix();

	TextDisplay *textDisplay = TextDisplay::getInstance();
	char points_txt[255];
	snprintf(points_txt, 255, "Level: %d\t\tPoints: %d ",max_flights, points);
	textDisplay->displayText(points_txt, GUI::win_width-300, 25, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_TIMES_ROMAN_24);
	snprintf(points_txt, 255, "Time: %li Cronotot: %f ",ta, cronotot/1000000.0);
	textDisplay->displayText(points_txt, GUI::win_width-500, 55, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_TIMES_ROMAN_24);
	snprintf(points_txt, 255, "Time speed: x%3.1f", SimTimeMod);
	textDisplay->displayText(points_txt, 10, GUI::win_height-5, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	char help_txt[255];
	strcpy(help_txt, "Press Mouse3 and move mouse to change orientation");
	textDisplay->displayText(help_txt, GUI::win_width-310, GUI::win_height-5, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<Tab> Change flight info");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-200, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<w>Move camera forward");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-180, GUI::win_width,
GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<+>Speed up time");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-160, GUI::win_width,
GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<->Slow down time");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-140, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<s>Move camera backward");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-120, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<a>Move camera side left");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-100, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<d>Move camera side right");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-80, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<c>Reset camera position");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-60, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<f>Center camera in flight");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-40, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);
	strcpy(help_txt, "<esc>Finish");
	textDisplay->displayText(help_txt, GUI::win_width-160, GUI::win_height-20, GUI::win_width, GUI::win_height, WHITE, GLUT_BITMAP_HELVETICA_12);

}
