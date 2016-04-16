/*
*
* Demonstrates how to load and display an Wavefront OBJ file.
* Using triangles and normals as static object. No texture mapping.
* https://tutorialsplay.com/opengl/
*
* OBJ files must be triangulated!!!
* Non triangulated objects wont work!
* You can use Blender to triangulate
*
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#define KEY_ESCAPE 27
using namespace std;
/************************************************************************
  Window
************************************************************************/
typedef struct {
    int width;
int height;
char* title;
float field_of_view_angle;
float z_near;
float z_far;
} glutWindow;
/***************************************************************************
  OBJ Loading
***************************************************************************/
class Model_OBJ
{
  public:
	Model_OBJ();
    float* Model_OBJ::calculateNormal(float* coord1,float* coord2,float* coord3 );
    int Model_OBJ::Load(char *filename);	// Loads the model
	void Model_OBJ::Draw();	// Draws the model on the screen
	void Model_OBJ::Release();	// Release the model
	float* normals;	// Stores the normals
    float* Faces_Triangles;	// Stores the triangles
	float* vertexBuffer;	// Stores the points which make the object
	long TotalConnectedPoints;	// Stores the total number of connected verteces
	long TotalConnectedTriangles;	// Stores the total number of connected triangles
};

