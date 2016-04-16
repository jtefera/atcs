/*
 * AirController.cpp
 *
 *  Created on: 21/07/2014
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

#include "AirController.h"
#include "Airport.h"
#include "Flight.h"
#include "Position.h"
#include <list>
#include <sys/time.h>

AirController::AirController() {
	// TODO Auto-generated constructor stub
	frame = 0;
	lastDangerCheckTime = 0;
}

AirController::~AirController() {
	// TODO Auto-generated destructor stub
}

void
AirController::clearDanger()
{
	//Segundo mecanismo de seguridad. controlador comprueba cada cierto tiempo si
	//la diferencia en el tiempo de llegada de dos aviones es mayor que la mininma de las minimas de seg
	std::list<Flight*> flights = Airport::getInstance()->getFlights();
	std::list<Flight*>::iterator flightIt,otherFlIt;
	//Itinera sobre cada vuelo y comprueba con el resto de vuelos que
	for(flightIt = flights.begin(); flightIt!=flights.end();++flightIt)
	{
		for(otherFlIt = flightIt;otherFlIt != flights.end(); ++otherFlIt)
		{
			if((*flightIt)->getId() != (*otherFlIt)->getId() && (*flightIt)->routed() && (*otherFlIt)->routed())
			{
				if(abs((*flightIt)->getTimeToCenter()-(*otherFlIt)->getTimeToCenter())<MAXSECTIME)
				{
					if((*flightIt)->getTimeToCenter()>(*otherFlIt)->getTimeToCenter())
					{
						(*flightIt)->getRoute()->clear();
						assignRoute(flightIt);
					}

					else
					{
						(*otherFlIt)->getRoute()->clear();
						assignRoute(otherFlIt);
					}

				}
			}
		}
	}

}

void
AirController::assignRoute(std::list<Flight*>::iterator flight)
{
	//Crea una ruta para cada vuelo sin rutas según la posición desde la que empiezan y la
	//cola existente.
	//Se coloca al avión en la cola según el tiempo que tardará en llegar a la pista
	//Si la diferencia de dicho tiempo según dicha ruta con respecto al resto de rutas ya
	//asignadas es mayor que cierto tiempo de seguridad, se le asigna dicha ruta
	//Instancia de todos los vuelos
	std::list<Flight*> flights = Airport::getInstance()->getFlights();
	std::list<Flight*>::iterator otherflight;

	//Definición últimos puntos de  ruta bajada y aterrizaje
	Route r0, r1, r2, r3;
	Position pos0(POS0_x, POS0_y, POS0_z); //Posición antes de procedimiento de bajada
	Position pos1(POS1_x, POS1_y, POS1_z); //Mitad de la bajada
	Position pos2(POS2_x, POS2_y, POS2_z); //Avión en suelo, principio de pista
	Position pos3(POS3_x, POS3_y, POS3_z); //Final de pista
	r0.pos = pos0;
	r0.speed = SPEED_0;
	r1.pos = pos1;
	r1.speed = SPEED_1;
	r2.pos = pos2;
	r2.speed = SPEED_2;
	r3.pos = pos3;
	r3.speed = SPEED_3;

	//Definición de variables que nos serán útiles
	int i,j,pointofroute,k;
	//float timetoarrive;
	//float speedf = 120; //Velocidad en la ruta
	long ta;
	float xd, yd,zd;
	bool freeroute = false; //Bool que al intinerar nos permitirá ver si un punto ruta está libre
	//gettimeofday(&tv, NULL);

	ta = Airport::getInstance()->getTotalCrono()/1000000.0; //Se ha escogido Crono para tener en cuenta las
															//aceleraciones en tiempo que pueden existir al aumentar
															//o disminuir el paso del tiemp

	//NextPositions nextp;



	float angle;
	long timetopoint = 0;
	long timetocenter;
	Position posbeforeland = pos0;
	Position newpoint = posbeforeland;
	Position newpointroute;

	float xnext,ynext,znext;
	float speednext;
	Route nextroute,newroute;
	float landingtime;
	int numeroderutasfun;
	float velFinTramo; //Guarda en cada tramo, la velocidad con la que acaba el avión
	float timebeforelanding;
	float landtimefun;

	//Iniciamos borrando cualquier ruta que pueda tener(util en caso de necesitar reininciar)
	//(*flight)->getRoute()->clear();
	//Aignamos

	//R1 R2 y R3 son las rutas de pos final de descenso, principio pista y final. Comun a todos
	(*flight)->getRoute()->push_back(r3);
	(*flight)->getRoute()->push_front(r2);
	(*flight)->getRoute()->push_front(r1);
	//Angulo de entrada del avion en grados
	angle = atan2((*flight)->getPosition().get_y(),(*flight)->getPosition().get_x())*180/3.1415;
	//Según angulo se asigna una ruta distinta
	if(abs(angle)<60){ //Ruta de la izq
		xd = DISROUTE;
		yd = 0;
	} else if(angle>60) { //Ruta de la der
		xd = -DISROUTE*cos(60*pi/180);
		yd = DISROUTE*sin(60*pi/180);
	} else { //Ruta del centro
		xd = -DISROUTE*cos(60*pi/180);
		yd = -DISROUTE*sin(60*pi/180);
	}



	//Cada punto de ruta es un punto a una distancia fija del anterior de cada una de las tres rutas
	pointofroute = 0;

	//Comprobar si una a una distancia pointfroute*DISROUTE de r0 cumple con la seguridad
	//ir aumentando pointofroute hasta que lo haga
	while(true)
	{
		//Punto (y ruta) de entrada a ruta a probar
		newpoint.set_x(posbeforeland.get_x()+xd*pointofroute);
		newpoint.set_y(posbeforeland.get_y()+yd*pointofroute);
		newpoint.set_y(posbeforeland.get_z()+HEIGHTDIFFERENCEPOINTS*pointofroute);
		newroute.pos = newpoint;
		newroute.speed = SPEED_0 + pointofroute*DIFFSPEED;

		//Tiempo que tarda el avión desde su pos ini hasta dicho punto
		timetopoint = (*flight)->getTimeToPoint(newroute,velFinTramo);
		//Tiempo que tardara en alcanzar al centro sumando el tiempo a dicho punto, el de aterrizaje y el de cada
		//franja
		timetocenter = timetopoint;
		if(pointofroute>0)
		{
			for(k=pointofroute;k>=0;k--)
			{
				timetocenter += timeBetweenTwoRoutes(DISROUTE,velFinTramo,SPEED_0+(k-1)*DIFFSPEED,MAXACC,velFinTramo);

			}
		}


		//Timpo que se tarda en recorrer los últimos cuatro puntos especificados arriba
		landingtime = timeBetweenTwoRoutes(r0.pos.distance(r1.pos),velFinTramo,r1.speed,MAXACC,velFinTramo);
		landingtime += timeBetweenTwoRoutes(r1.pos.distance(r2.pos),velFinTramo,r2.speed,MAXACC,velFinTramo);
		landingtime += timeBetweenTwoRoutes(r2.pos.distance(r3.pos),velFinTramo,r3.speed,MAXACC,velFinTramo);
		timebeforelanding = timetocenter;
		timetocenter +=  landingtime;
		//Comprobar si dicho punto cumple con seguridad con el resto de tiempos del resto de aviones
		//Si al pasar por todos los aviones, freeroute sigue verdadero, es que cumple con seg
		freeroute = true;
		for(otherflight = flights.begin();otherflight!=flights.end();++otherflight)
		{
			if((*otherflight)->getId()!=(*flight)->getId() && (*otherflight)->getTimeToCenter()>-1)
			{
				//En caso de que no cumpla con SECTIME, free route false
				if(abs((*otherflight)->getTimeToCenter()-timetocenter)<SECTIME){
					freeroute = false;
					pointofroute++;
					break;
				}
			}
		}
		//Caso que si cumpla, se asigna el nuevo punto de ruta y cada punto intermedio
		if(freeroute){
			for(i=0;i<=pointofroute;i++){
				xnext = posbeforeland.get_x() + i*xd;
				ynext = posbeforeland.get_y() + i*yd;
				znext = posbeforeland.get_z() + i*HEIGHTDIFFERENCEPOINTS;
				speednext = SPEED_0+i*20;
				newpointroute.set_x(xnext);
				newpointroute.set_y(ynext);
				newpointroute.set_z(znext);
				nextroute.pos = newpointroute;
				nextroute.speed = speednext;
				(*flight)->getRoute()->push_front(nextroute);
			}
			break;
		}
	}

}

void
AirController::doWork()
{
	std::list<Flight*> flights = Airport::getInstance()->getFlights();
	std::list<Flight*>::iterator it,it2,otherflight;
	long ta;
	ta = Airport::getInstance()->getTotalCrono()/1000000.0; //Se ha escogido Crono para tener en cuenta las
															//aceleraciones en tiempo que pueden existir al aumentar
															//o disminuir el paso del tiemp
	//El mecanismo de limpieza de peligro es pesado por lo que para no relentizar mucho, se activa cada
	//50 segundos del simulador
	if(ta-lastDangerCheckTime > 50.0)
	{
		clearDanger();
		lastDangerCheckTime = ta;
	}


	//Asignacion de rutas
	for(it = flights.begin(); it!=flights.end(); ++it)
	{
		if((*it)->getRoute()->empty())
		{
			assignRoute(it);

		}
	}

}
