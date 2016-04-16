/*
 * Storm.h
 *
 *  Creado el 27 de Febrero 2015
 *  Autor: Jonathan Tefera (jonahum)
 *  Parte de ATCSim
 *  Se encarga de definir el objeto tormenta
 * 	Se crea cada cierto tiempo.
 * 	Viene definido por la posición en la que empieza
 * 	(en los bordes del plano), la velocidad y
 * 	la dirección(bearing, se supone inclinación 0) con la que se mueve,
 * 	el radio y la altura que tiene, La altura sobre la que vuela.
 *
 * 	Rango de Alturas que afecta: desde minh hasta minh+h
 *
 * 	Cuando un avión pasa por dentro, la puntuación
 * 	de este decrece al doble de velocidad
 *
 * 	Pueden haber varios a la vez.
 * 	Van a estar dibujados como un cilindro.
 *
 *
 */

#include "Storm.h"
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


Storm::Storm(int _id)
{
	float x,y,z;
	/*	La creación de una tormenta viene dada por la definición de su posición de
	 * 	inicio, la velocidad incial(constante), la dirección(bearing, constante),
	 * 	la altura(cte), el radio(cte) y la altitud a la que vuela(distancia del
	 * 	suelo a la base, cte).
	 *
	 */
	id = _id;

	//Min altitud de base: 1000m; Max Altitud de Base: 2000m;
	altitude = randomBetween(MIN_ALTITUD_BASE_STORM , MAX_ALTITUD_BASE_STORM);
	//Max altura de la tormenta: 2000m
	height = random(MAX_HEIGHT_STORM);
	//Rango de velocidades: entre 200 y 1000 m/s
	speed = randomBetween(MIN_SPEED_STORM , MAX_SPEED_STORM);
	//Rango de radios de la tormenta: Entre 100 y 2000
	radio = randomBetween(MIN_RADIO_STORM , MAX_RADIO_STORM);
	//Empezamos en el borde horizontal o vertical?
	bool startInHorBorder = randomDouble()>0.5;

	//La pos de inicio es un punto del borde de vista del escenario. Aleatorio.
	//La pos es la del centro de la tormenta:
	//	-x e y es un punto del borde. Cambian.
	//	-z viene dado por altitud + altura/2. Constante.

	//¿En la parte derecha(o superior) o izquierda(o inferior?

	bool startInPositiveSide = randomDouble()>0.5;
	int side = -1;
	if(startInPositiveSide)
		side = 1;

	if(startInHorBorder)
	{
		x = side * AIRPORT_DISTANCE_MAX;
		y = randomBetween(-AIRPORT_DISTANCE_MAX,AIRPORT_DISTANCE_MAX);
	} else {
		x = randomBetween(-AIRPORT_DISTANCE_MAX,AIRPORT_DISTANCE_MAX);
		y = side * AIRPORT_DISTANCE_MAX;
	}
	z = altitude + height*0.5;

	pos.set_x(x);
	pos.set_y(y);
	pos.set_z(z);

	//Bearing: entre 0 y 2pi
	// Bearing según el lado desde el que empieza.
	//
	/* Si empieza en el lado:
	 * 		superior(-20000,y) 	-> 	bearing de -90 a 90º
	 * 		derecha (x,20000) 	->	bearing de -180 a 0
	 * 		inferior (20000,y)	-> 	bearing de -90 a -180 y de 90 a 180
	 * 											de 90 a 270 y normalizar
	 * 		izq(x,-20000)		->	bearing de 0 a 180
	 *
	 */
	if(startInHorBorder && startInPositiveSide) //Inferior
	{
		bearing = randomBetweenAnglesDeg(90,270);
	} else if(startInHorBorder)	//Superior
	{
		bearing = randomBetweenAnglesDeg(-90,90);
	} else if(startInPositiveSide)	//Derecha
	{
		bearing = randomBetweenAnglesDeg(-180,0);
	} else { //Izquierda
		bearing = randomBetweenAnglesDeg(0,180);

	}
	info();

}

Storm::~Storm()
{

}

void Storm::info()
{
	//Para DebugMode
	if(DEBUG_MODE && DEBUG_STORMS){
		float x = pos.get_x();
		float y = pos.get_y();
		float z = pos.get_z();
		/*std::cout	<< "\n Tormenta nº"<<id<<std::endl
					<<"x="<<x<<" y="<<y<<" z="<<z<<std::endl
					<<"altitude="<<altitude<<" height="<<height<<std::endl
					<<"radio="<<radio<<" speed="<<speed<<"bearing"<<toDegrees(bearing)<<std::endl;*/
		draw();
	}
}


void Storm::update(float delta_t)
{
	float trans;
	float x,y,z;
	trans = speed*delta_t;

	last_pos = pos;

	x = last_pos.get_x()+ trans * cos(bearing);
	y = last_pos.get_y() + trans * sin(bearing);
	pos.set_x(x);
	pos.set_y(y);

	draw();
}




void Storm::draw()
{

	glPushMatrix();
		//Color of the storm: white with transparency
		glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
			//Position of the center of the base of the cylinder
		glTranslatef(pos.get_x(), pos.get_y(), altitude);
			//Variable that it is going to hold the storm
		GLUquadric *quadratic = gluNewQuadric();
			//Set textures
		gluQuadricNormals(quadratic, GLU_SMOOTH);
			//GLUquadratic colorcilinder[2,2,2,1]
		gluQuadricTexture(quadratic, GL_TRUE);
			//gluSphere( quadratic, COLLISION_DISTANCE/2.0f, 32, 32);

		gluCylinder(quadratic, radio,radio,height,32,1);

		//La tapa
		glTranslatef(0, 0,height);
		glBegin(GL_POLYGON);
			for(double i = 0; i < 2 * pi; i += pi / 32) //<-- Change this Value
		 			glVertex3f(cos(i) * radio, sin(i) * radio, 0.0);

		glEnd();

	glPopMatrix();

}

