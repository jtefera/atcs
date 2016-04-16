/*
 * GUI.h
 *
 *  Created on: 16/07/2014
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

#ifndef GUI_H_
#define GUI_H_

#define KEY_ESCAPE 27

#include "TextDisplay.h"
#include "colours.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <iostream>



typedef struct {
    int width;
	int height;
	char* title;
	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

class Model_OBJ
{
  public:
	Model_OBJ();
    float* calculateNormal(float* coord1,float* coord2,float* coord3 );
    int Load(const char  *filename);	// Loads the model
	void Draw();	// Draws the model on the screen
	void Release();	// Release the model
	float* normals;	// Stores the normals
	float* Faces_Triangles;	// Stores the triangles
	float* vertexBuffer;	// Stores the points which make the object
	long TotalConnectedPoints;	// Stores the total number of connected verteces
	long TotalConnectedTriangles;	// Stores the total number of connected triangles
};





class GUI {
public:
	GUI(int argc, char *argv[]);
	virtual ~GUI();


	void init();
	void run();

	static const int win_width;
	static const int win_height;
	static int loadObject(const char* filename);


private:

	static const float field_of_view_angle;
	static const float x_near;
	static const float x_far;

	static void render();
	static void step();

	static void keyboard(unsigned char key, int mousePositionX, int mousePositionY);
	static void processMouseFunc(int button, int state, int x, int y);
	static void processMouseMotion(int x, int y);
	static int mousebutton;
	static int mousestate;
	static int mousex, mousey;
	static float cam_alpha, cam_beta;
	static float cam_x, cam_y, cam_z;



};

#endif /* GUI_H_ */
