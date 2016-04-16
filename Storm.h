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

#ifndef STORM_H_
#define STORM_H_

#include <string>

#include "Position.h"
#include "Common.h"
#include <list>

class Storm{
 public:
	Storm(int _d);
	~Storm();

	// Funciones que actualizan datos de la tormenta y la dibujan;
	void update(float delta_t);
	void draw();

	// Posición del centro de la tormenta
	Position getPosition() { return pos;};

	// Dirección del movimiento
	float getBearing() { return bearing;};
	float getSpeed() { return speed;};
	int getId(){return id;};
	float getHeight(){return height;};
	float getAltitude(){return altitude;};
	float getRadius(){return radio;};

	//Debug info
	void info();



 private:
	int id;
	Position pos, last_pos;
	float bearing;
	float speed;
	float radio;
	float height;
	float altitude; //De la base
};









#endif /* STORM_H_ */
