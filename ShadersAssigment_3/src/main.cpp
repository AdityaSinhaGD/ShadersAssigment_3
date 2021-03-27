// This OpenGL project demonstrates a simple interactive camera implementation and a few vertex and fragment shaders. 
// Two modes of camera controls (press c to switch between two modes): 
// 1. Focus Mode: Holding ALT and Left Mouse Button (LMB) and moving the mouse will rotate the camera about the LookAt Position
//                Holding ALT and MMB and moving the mouse will pan the camera.
//                Holding ALT and RMB and moving the mouse will zoom the camera.
// 2. First-Person Mode: Pressing W, A, S, or D will move the camera
//                       Holding LMB and moving the mouse will roate the camera.
// Basic shader - demonstrate the basic use of vertex and fragment shader
// Displacement shader - a special fireball visual effects with Perlin noise function
// Toon shading shader - catoonish rendering effects
// Per-vertex shading v.s. per-fragment shading = visual comparison between two types of shading 

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Camera.h"
#include "Text.h"
#include "Mesh.h"

#include <iostream>
using namespace std;
using namespace glm;

int g_winWidth  = 1024;
int g_winHeight = 512;

Camera g_cam;
Text g_text;

unsigned char g_keyStates[256];

char v_shader_file_1[] =
".\\src\\PhongPerVert.vert";
//".\\shaders\\displacement.vert"; // vertex displacement shader with perlin noise
//".\\shaders\\perVert_lambert.vert"; // basic lambert lighting  
// ".\\shaders\\perFrag_lambert.vert"; // basic lambert lighting with per-fragment implementation
// ".\\shaders\\toon_shading.vert"; // basic toon shading with per-fragment implementation

char v_shader_file_2[] = ".\\src\\PhongPerFrag.vert";

char f_shader_file_1[] =
".\\src\\PhongPerVert.frag";
// ".\\shaders\\displacement.frag"; // vertex displacement shader with perlin noise
// ".\\shaders\\perVert_lambert.frag"; // basic lambert shading 
// ".\\shaders\\perFrag_lambert.frag"; // basic lambert shading with per-fragment implementation
// ".\\shaders\\toon_shading.frag"; // basic toon shading with per-fragment implementation

char f_shader_file_2[] = ".\\src\\PhongPerFrag.frag";

const char meshFile[128] = 
//"Mesh/sphere.obj";
//"Mesh/bunny2K.obj";
"src/teapot.obj";
//"Mesh/teddy.obj";

Mesh g_meshFirst, g_meshSecond;

vec3 g_lightPos = vec3(3, 3, 3);
float g_time = 0.0f;
PointLight lights[2];

int lightSelectionIndex = 0;

vec3 currentLightPositions[] = {
	vec3(3.0f,3.0f,3.0f),
	vec3(1.0f,0.0,-2.0f)
};

void initializePointLightData()
{
	lights[0] = {};
	lights[0].ambient = vec3(0.0, 0.15f, 0.0);
	lights[0].diffuse = vec3(1.0f, 1.0f, 0.0);
	lights[0].specular = vec3(1.0f, 0.0, 0.0);
	lights[0].position = currentLightPositions[0];
	lights[0].coeff = 20;

	lights[1].ambient = vec3(0.0, 0.0, 0.15f);
	lights[1].diffuse = vec3(1.0f, 0.0, 1.0f);
	lights[1].specular = vec3(1.0f, 0.0, 0.0);
	lights[1].position = currentLightPositions[1];
	lights[1].coeff = 20;
}

void drawSelectedPointLight()
{
	glUseProgram(0);

	glPushMatrix();
	glTranslatef(lights[0].position.x, lights[0].position.y, lights[0].position.z);
	if (lightSelectionIndex == 0)
	{
		glutSolidSphere(0.2, 50, 50);
	}
	else
	{
		glutWireSphere(0.2, 50, 50);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(lights[1].position.x, lights[1].position.y, lights[1].position.z);
	if (lightSelectionIndex == 1)
	{
		glutSolidSphere(0.2, 50, 50);
	}
	else
	{
		glutWireSphere(0.2, 50, 50);
	}

	glPopMatrix();
}

void initialization() 
{    
    g_cam.set(3.0f, 4.0f, 14.0f, 0.0f, 1.0f, -0.5f, g_winWidth, g_winHeight);
	g_text.setColor(0.0f, 0.0f, 0.0f);

	g_meshFirst.create(meshFile, v_shader_file_1, f_shader_file_1);
	g_meshSecond.create(meshFile, v_shader_file_2, f_shader_file_2);
	// add any stuff you want to initialize ...
}

/****** GL callbacks ******/
void initialGL()
{    
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
    // add any stuff to update at runtime ....

    g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);

	glutPostRedisplay();
}

void display()
{	 
	glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// add any stuff you'd like to draw	

	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	g_cam.drawGrid();
    g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
    g_cam.drawCoordinate();

	// display the text
	string str;
	if(g_cam.isFocusMode()) {
        str = "Cam mode: Focus";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	} else if(g_cam.isFPMode()) {
        str = "Cam mode: FP";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	}

	/*str = "vertex count: " + std::to_string(g_meshFirst.vert_num);
	g_text.draw(10, 45, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	str = "triangle count: " + std::to_string(g_meshFirst.tri_num);
	g_text.draw(10, 60, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);*/
		
	initializePointLightData();

	g_time = (float)glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	//g_mesh.draw(g_cam.viewMat, g_cam.projMat, g_lightPos, g_time);
	g_meshFirst.draw(g_cam.viewMat, g_cam.projMat, lights, vec3(0.0, 2.0, 0.0), vec3(0.5, 0.5, 0.5), vec3(g_cam.eye.x, g_cam.eye.y, g_cam.eye.z));
	g_meshSecond.draw(g_cam.viewMat, g_cam.projMat, lights, vec3(3.0, 2.0, 0.0), vec3(0.5, 0.5, 0.5), vec3(g_cam.eye.x, g_cam.eye.y, g_cam.eye.z));

	drawSelectedPointLight();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;
	if (h == 0) {
		h = 1;
	}
	g_cam.setProjectionMatrix(g_winWidth, g_winHeight);
    g_cam.setViewMatrix();
    glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
    g_cam.mouseClick(button, state, x, y, g_winWidth, g_winHeight);
}

void motion(int x, int y)
{
    g_cam.mouseMotion(x, y, g_winWidth, g_winHeight);
}

void keyup(unsigned char key, int x, int y)
{
    g_keyStates[key] = false;
}

void keyboard(unsigned char key, int x, int y)
{
    g_keyStates[key] = true;
	switch(key) { 
		case 27:
			exit(0);
			break;
        case 'c': // switch cam control mode
            //g_cam.switchCamMode();
			//glutPostRedisplay();
            break;
        case ' ':
            g_cam.PrintProperty();
            break;
		case '+':
			g_meshFirst.normal_offset += 0.01;
			break;
		case'-':
			g_meshFirst.normal_offset -= 0.01;
			break;
		case '1':
			lightSelectionIndex = 0;
			break;
		case '2':
			lightSelectionIndex = 1;
			break;
		case 'a':
			currentLightPositions[lightSelectionIndex] -= vec3(0.1f, 0.0, 0.0);
			break;
		case 'd':
			currentLightPositions[lightSelectionIndex] += vec3(0.1f, 0.0, 0.0);
			break;
		case 'w':
			currentLightPositions[lightSelectionIndex] += vec3(0.0, 0.0, 0.1f);
			break;
		case's':
			currentLightPositions[lightSelectionIndex] -= vec3(0.0, 0.0, 0.1f);
			break;
		case 'u':
			currentLightPositions[lightSelectionIndex] += vec3(0.0, 0.1f, 0.0);
			break;
		case 'j':
			currentLightPositions[lightSelectionIndex] -= vec3(0.0, 0.1f, 0.0);
			break;
	}
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_winWidth, g_winHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("VertFrag Shader Example");
	
	glewInit();
	initialGL();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	initialization();
	
    glutMainLoop();
    return EXIT_SUCCESS;
}