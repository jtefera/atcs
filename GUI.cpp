/*
 * GUI.cpp
 *
 *  Created on: 16/07/2014
 *      Author: paco
 */

#include "GUI.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <iostream>
#include <math.h>
#include "Common.h"
#include "Airport.h"
#include "AirController.h"
#include "tiny_obj_loader.h"




//#define INIT_CAM_X -20000.0f;
//#define INIT_CAM_Y 0.0f;
//#define INIT_CAM_Z 5000.0f;
//#define INIT_CAM_A M_PI;
//#define INIT_CAM_B -M_PI/2.0f;
#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9

#define INIT_CAM_X 0.0f;
#define INIT_CAM_Y 0.0f;
#define INIT_CAM_Z 20000.0f;
#define INIT_CAM_A M_PI;
#define INIT_CAM_B -M_PI;

int GUI::mousebutton = 0;
int GUI::mousestate = 0;
int GUI::mousex = 0;
int GUI::mousey = 0;
float GUI::cam_alpha = INIT_CAM_A;
float GUI::cam_beta = INIT_CAM_B;
float GUI::cam_x = INIT_CAM_X;
float GUI::cam_y = INIT_CAM_Y;
float GUI::cam_z = INIT_CAM_Z;
const int GUI::win_width  = 1024;
const int GUI::win_height = 768;
const float GUI::field_of_view_angle = 60;
const float GUI::x_near = 1.0f;
const float GUI::x_far = 40000.0f;

GUI::GUI(int argc, char *argv[]) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win_width,win_height);					// set window size
	glutCreateWindow("ATCSim");									// create Window
	glutDisplayFunc(GUI::step);									// register Display Function
	glutIdleFunc( GUI::render );									// register Idle Function
	glutMouseFunc(GUI::processMouseFunc);
	glutMotionFunc(GUI::processMouseMotion);
	glutKeyboardFunc( GUI::keyboard );								// register Keyboard Handler

}

GUI::~GUI() {
	// TODO Auto-generated destructor stub
}

void
GUI::render()
{
	float vcam_x, vcam_y, vcam_z;
	TextDisplay *textDisplay;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	vcam_x = cam_x + (cos(cam_alpha) * sin (cam_beta));
	vcam_y = cam_y + (sin(cam_alpha) * sin (cam_beta));
	vcam_z = cam_z + cos(cam_beta);

	gluLookAt(cam_x, cam_y, cam_z, vcam_x, vcam_y, vcam_z, 0.0, 0.0, 1.0);

	Airport::getInstance()->draw();
	Airport::getInstance()->step();

	AirController::getInstance()->doWork();

	textDisplay = TextDisplay::getInstance();

	textDisplay->calculateFramesPerSecond();
	textDisplay->displayFPS(30, win_height - 30, win_width, win_height, YELLOW);

	textDisplay->displayText((char*)(new std::string("ATCSim"))->c_str(), 15, 25, win_width, win_height, ORANGE);

	glutSwapBuffers();



}

int
GUI::loadObject(const char* filename)
{


}

#define POINTS_PER_VERTEX 3

Model_OBJ::Model_OBJ()
{
	this->TotalConnectedTriangles = 0;
	this->TotalConnectedPoints = 0;
}

float* Model_OBJ::calculateNormal( float *coord1, float *coord2, float *coord3 )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = coord1[0] - coord2[0];
   va[1] = coord1[1] - coord2[1];
   va[2] = coord1[2] - coord2[2];
   vb[0] = coord1[0] - coord3[0];
   vb[1] = coord1[1] - coord3[1];
   vb[2] = coord1[2] - coord3[2];
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
	float norm[3];
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
	return norm;
}

int Model_OBJ::Load(const char* filename)
{
	std::string line;
	std::ifstream objFile (filename);
	if (objFile.is_open())	// If obj file is open, continue
	{
		objFile.seekg (0, std::ios::end);	// Go to end of the file,
		long fileSize = objFile.tellg();	// get file size
		objFile.seekg (0, std::ios::beg);	// we'll use this to register memory for our 3d model
		vertexBuffer = (float*) malloc (fileSize);	// Allocate memory for the verteces
		Faces_Triangles = (float*) malloc(fileSize*sizeof(float));	// Allocate memory for the triangles
		normals  = (float*) malloc(fileSize*sizeof(float));	// Allocate memory for the normals
		int triangle_index = 0;	// Set triangle index to zero
		int normal_index = 0;	// Set normal index to zero
		while (! objFile.eof() )	// Start reading file data
		{
			getline (objFile,line);	// Get line from file
			if (line.c_str()[0] == 'v')	// The first character is a v: on this line is a vertex stored.
			{
				line[0] = ' ';	// Set first character to 0. This will allow us to use sscanf
				sscanf(line.c_str(),"%f %f %f ",	// Read floats from the line: v X Y Z
				&vertexBuffer[TotalConnectedPoints],
				&vertexBuffer[TotalConnectedPoints+1],
				&vertexBuffer[TotalConnectedPoints+2]);
				TotalConnectedPoints += POINTS_PER_VERTEX;	// Add 3 to the total connected points
			}
			if (line.c_str()[0] == 'f')	// The first character is an 'f': on this line is a point stored
			{
				line[0] = ' ';	// Set first character to 0. This will allow us to use sscanf
				int vertexNumber[4] = { 0, 0, 0 };
				sscanf(line.c_str(),"%i%i%i",	// Read integers from the line:  f 1 2 3
				&vertexNumber[0],	// First point of our triangle. This is an
				&vertexNumber[1],	// pointer to our vertexBuffer list
				&vertexNumber[2] );	// each point represents an X,Y,Z.
				vertexNumber[0] -= 1;	// OBJ file starts counting from 1
				vertexNumber[1] -= 1;	// OBJ file starts counting from 1
				vertexNumber[2] -= 1;	// OBJ file starts counting from 1
				/********************************************************************
				* Create triangles (f 1 2 3) from points: (v X Y Z) (v X Y Z) (v X Y Z).
				* The vertexBuffer contains all verteces
				* The triangles will be created using the verteces we read previously
				*/
				int tCounter = 0;
				for (int i = 0; i < POINTS_PER_VERTEX; i++)
				{
					Faces_Triangles[triangle_index + tCounter   ] = vertexBuffer[3*vertexNumber[i] ];
					Faces_Triangles[triangle_index + tCounter +1 ] = vertexBuffer[3*vertexNumber[i]+1 ];
					Faces_Triangles[triangle_index + tCounter +2 ] = vertexBuffer[3*vertexNumber[i]+2 ];
					tCounter += POINTS_PER_VERTEX;
				}
				/*********************************************************************
				* Calculate all normals, used for lighting
				*/
				float coord1[3] = { Faces_Triangles[triangle_index], Faces_Triangles[triangle_index+1],Faces_Triangles[triangle_index+2]};
				float coord2[3] = {Faces_Triangles[triangle_index+3],Faces_Triangles[triangle_index+4],Faces_Triangles[triangle_index+5]};
				float coord3[3] = {Faces_Triangles[triangle_index+6],Faces_Triangles[triangle_index+7],Faces_Triangles[triangle_index+8]};
				float *norm = this->calculateNormal( coord1, coord2, coord3 );
				tCounter = 0;
				for (int i = 0; i < POINTS_PER_VERTEX; i++)
				{
					normals[normal_index + tCounter ] = norm[0];
					normals[normal_index + tCounter +1] = norm[1];
					normals[normal_index + tCounter +2] = norm[2];
					tCounter += POINTS_PER_VERTEX;
				}
				triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
				normal_index += TOTAL_FLOATS_IN_TRIANGLE;
				TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE;
			}
		}
		objFile.close();	// Close OBJ file
	}
	else
	{
		std::cout << "Unable to open file";
	}
	return 0;
}
void Model_OBJ::Release()
{
	free(this->Faces_Triangles);
	free(this->normals);
	free(this->vertexBuffer);
}
void Model_OBJ::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);	// Enable vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);	// Enable normal arrays
	glVertexPointer(3,GL_FLOAT,	0,Faces_Triangles);	// Vertex Pointer to triangle array
	glNormalPointer(GL_FLOAT, 0, normals);	// Normal pointer to normal array
	glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);	// Draw the triangles
	glDisableClientState(GL_VERTEX_ARRAY);	// Disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);	// Disable normal arrays
}

void
GUI::keyboard(unsigned char key, int mousePositionX, int mousePositionY )
{
	float dlin, dlat;
	float despx, despy, despz;
	dlin = dlat = 0.0f;

	float desp = 100.0f;
	switch ( key )
	{
	case KEY_ESCAPE:
		exit ( 0 );
		break;
	case 'w':
	dlin += desp;
	break;
	case 's':
		dlin -= desp;
		break;
	case 'a':
		dlat += desp;
		break;
	case 'd':
		dlat -= desp;
		break;
    case '+':
        Airport::getInstance()->UpdateSimTime(INC_SIMTIME);
        break;
    case '-':
        Airport::getInstance()->UpdateSimTime(-INC_SIMTIME);
        break;
	case 'c':
		cam_alpha = INIT_CAM_A;
		cam_beta = INIT_CAM_B;
		cam_x = INIT_CAM_X;
		cam_y = INIT_CAM_Y;
		cam_z = INIT_CAM_Z;

		break;
	case '\t':
		Airport::getInstance()->NextFocus();
		break;
	case 'f':

		cam_x = Airport::getInstance()->getFocused()->getPosition().get_x() - 2000.0;
		cam_y = Airport::getInstance()->getFocused()->getPosition().get_y();
		cam_z = Airport::getInstance()->getFocused()->getPosition().get_z();

		cam_alpha = M_PI;
		cam_beta = -M_PI/2.0f;

		break;
	case 'g':
		Airport::getInstance()->Emergency();
		break;
	default:
		std::cerr<<"Key: "<<key<<std::endl;
		break;
	}

	cam_x = cam_x + (dlin * cos(cam_alpha) * sin (cam_beta));
	cam_y = cam_y + (dlin * sin(cam_alpha) * sin (cam_beta));
	cam_z = cam_z + dlin * cos(cam_beta);

	cam_x = cam_x + (dlat * cos(cam_alpha+(M_PI/2.0)) * sin (cam_beta));
	cam_y = cam_y + (dlat * sin(cam_alpha+(M_PI/2.0)) * sin (cam_beta));
	//cam_z = cam_z + dlat * cos(cam_beta);


}

void
GUI::processMouseFunc(int _button, int _state, int _x, int _y) {

	mousebutton = _button;
	mousex = _x;
	mousey = _y;
}

void
GUI::processMouseMotion(int _x, int _y) {

	int deltax, deltay;

	deltax = _x - mousex;
	deltay = _y - mousey;

	mousex = _x;
	mousey = _y;


	if((mousebutton == GLUT_RIGHT_BUTTON) && (mousestate == GLUT_DOWN ))
	{
		cam_alpha = normalizePi(cam_alpha - toRadians(deltax));
		cam_beta = normalizePi(cam_beta - toRadians(deltay));
	}

}

void
GUI::init()
{

	GLfloat aspect = (GLfloat) win_width / win_height;

	glMatrixMode(GL_PROJECTION);												// select projection matrix
	glViewport(0, 0, win_width, win_height);									// set the viewport
	glMatrixMode(GL_PROJECTION);												// set matrix mode
	glLoadIdentity();															// reset projection matrix
	gluPerspective(field_of_view_angle, aspect, x_near, x_far);				// set up a perspective projection matrix
	glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix
	glShadeModel( GL_SMOOTH );
	glClearDepth( 1.0f );														// specify the clear value for the depth buffer
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );						// specify implementation-specific hints
	glClearColor(0.0, 0.0, 0.0, 1.0);											// specify clear values for the color buffers
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



}

void
GUI::run()
{
	glutMainLoop();
}

void
GUI::step()
{

}

