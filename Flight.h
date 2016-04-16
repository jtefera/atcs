/*
 * Flight.h
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
#include <string>
#include "GUI.h"
#ifndef FLIGHT_H_
#define FLIGHT_H_

#include "tiny_obj_loader.h"
#include "Position.h"
#include "Common.h"
#include <list>
#include <vector>

typedef struct {
	Position pos;
	float speed;
} Route;



typedef struct {
	Position pos;
	long timetoarrive;
} NextPositions;

class Flight {
public:
	Flight(std::string _id, Position _pos, float _bearing, float _inclination, float _speed);
	virtual ~Flight();

	void update(float delta_t);
	void draw();


	std::list<Route> *getRoute() { return &route;};
	std::list<NextPositions> *getNextRoutes() { return &nextroutes;};
	bool routed() { return !route.empty();};
	long TimeToPosIfInRoute(Position pos);
	void setFocused(bool state) { focused = state;};
	bool getFocused() { return focused;};
	Position getPosition() { return pos;};
	float getInclination() { return inclination;};
	float getBearing() { return bearing;};
	float getSpeed() { return speed;};
	float getPoints() {return points;};
	float getTimeToPoint(Route rte, float &velfin);
	//float timeBetweenTwoRoutes(Route r1, Route r2);
	long getTimeToCenter();
	std::string getId(){return id;};
	void calculateNextPos();
	void setTimeToCenter(long _timetocenter) {timetocenter = _timetocenter;};
	void setEmergency(bool state) { inEmergency = state;};
	void planEmergency();

private:
	std::string id;
	Position pos, last_pos;
	float bearing, inclination;
	float speed;
	float desspeed;
	std::list<Route> route;
	std::list<NextPositions> nextpositions;
	std::list<NextPositions> nextroutes;
	bool focused;
	bool test;
	float points;
	long timetocenter;
	bool firstbear;
	int accelerating;
	int beardir;
	bool rotating;
	bool inEmergency;
	bool emergencySetted;
	bool inStorm;
	Model_OBJ modelo;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
};

#endif /* FLIGHT_H_ */
