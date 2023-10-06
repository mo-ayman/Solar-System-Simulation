///////////////////////////////////////////////////////////////////////////////////
// spaceTravel.cpp
//
// This program draws a conical spacecraft that can travel and an array of
// fixed spherical asteroids. The view in the left viewport is from a fixed
// camera; the view in the right viewport is from the spacecraft.
// There is approximate collision detection.
//
// User-defined constants:
// ROW is the number of rows of  asteroids.
// COLUMN is the number of columns of asteroids.
// FILL_PROBABILITY is the percentage probability that a particular row-column slot
// will be filled with an asteroid.
//
// Interaction:
// Press the left/right arrow keys to turn the craft.
// Press the up/down arrow keys to move the craft.
//
// Sumanta Guha.
///////////////////////////////////////////////////////////////////////////////////

#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



#define ROWS 10				 // Number of rows of asteroids.
#define COLUMNS 1			 // Number of columns of asteroids.
#define FILL_PROBABILITY 100 // Percentage probability that a particular row-column slot will be
int SUN = 8; 
int MOON = 9; 
int EARTH = 2; 
int SATURN = 5; 

// Globals.
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.
static int width, height;					  // Size of the OpenGL window.
static float angle = 45;					  // Angle of the spacecraft.
static float xVal = 115, yVal = 0.0, zVal = 115;			  // Co-ordinates of the spacecraft.
static int isCollision = 0;					  // Is there collision between the spacecraft and an asteroid?
static unsigned int spacecraft;				  // Display lists base index.
static int frameCount = 0;					  // Number of frames
float lightPos0[] = {0.0, 0.0, 0.0, 1.0}; 
const float baseOrbitRadius = 15.0;		 
const float orbitRadiusIncrement = 15.0; 
const float orbitSpeed = 0.01;

static float asteroidColor[][3] = {
	{0.831f, 0.831f, 0.831f}, 
	{1.0f, 0.784f, 0.0f},	  
	{0.0f, 0.502f, 1.0f},	  
	{0.8f, 0.2f, 0.0f},		  
	{1.0f, 0.725f, 0.42f},	  
	{0.878f, 0.812f, 0.671f}, 
	{0.576f, 0.859f, 0.925f}, 
	{0.165f, 0.341f, 0.553f}, 
	{1.0f, 1.0f, 0.0f},		  
	{0.863f, 0.863f, 0.863f}  

};

const char* texturePaths[] = {
    "../imgs/texture1.jpeg",
    "../imgs/texture2.jpeg",
    "../imgs/texture3.jpeg",
    "../imgs/texture4.jpeg",
    "../imgs/texture5.jpeg",
    "../imgs/texture6.jpeg",
    "../imgs/texture7.jpeg",
    "../imgs/texture8.jpeg",
	"../imgs/texture9.jpeg",
	"../imgs/texture10.jpeg"
};

static float asteroidRadius[] = {
	3,	// Mercury
	5,	// Venus
	6,	// Earth
	4,	// Mars
	7,	// Jupiter
	5,	// Saturn
	3,	// Uranus
	3,	// Neptune
	10, // Sun
	2	// Moon
};

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
	char *c;

	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(font, *c);
}

// Asteroid class.
class Asteroid
{
public:
	Asteroid();
	Asteroid(float x, float y, float z, float r, float colorR, float colorG, float colorB, float orbitRadius);
	float getCenterX() { return centerX; }
	float getCenterY() { return centerY; }
	float getCenterZ() { return centerZ; }
	float getRadius() { return radius; }
	void draw(int index);
	void setPosition(float x, float y, float z);
	float getOrbitRadius() { return orbitRadius; }
	// getColor
	// unsigned char* getColor() { return color; };
private:
	float centerX, centerY, centerZ, radius;
	float color[3];
	float orbitRadius;
};

// Asteroid default constructor.
Asteroid::Asteroid()
{
	centerX = 0.0;
	centerY = 0.0;
	centerZ = 0.0;
	radius = 0.0; // Indicates no asteroid exists in the position.
	color[0] = 0.0;
	color[1] = 0.0;
	color[2] = 0.0;
}

// // Asteroid constructor.
Asteroid::Asteroid(float x, float y, float z, float r, float colorR,
				   float colorG, float colorB, float orbitRadius)
{
	centerX = x;
	centerY = y;
	centerZ = z;
	radius = r;
	color[0] = colorR;
	color[1] = colorG;
	color[2] = colorB;
	this->orbitRadius = orbitRadius;
}
const int numTextures = 10;


// Function to draw asteroid.
void Asteroid::draw(int index)
{
	if (radius > 0.0) // If asteroid exists.
	{
		float matAmp[] = {0.2, 0.2, 0.2, 1.0};
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmp);

		float matDif[] = {color[0], color[1], color[2], 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);

		// float matSpec[] = {1.0, 1.0, 1.0, 1.0};
		float matShine[] = {100.0};
		// glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	
		if(index == SUN) {


			float matEmi[] = {1.0, 1.0, 0.0, 1.0};	
			glMaterialfv(GL_FRONT, GL_EMISSION, matEmi);
		
		} else {
			float matEmi[] = {0.0, 0.0, 0.0, 1.0};
				glMaterialfv(GL_FRONT, GL_EMISSION, matEmi);
		
		}

		glPushMatrix();
		glTranslatef(centerX, centerY, centerZ);

		// bind texture
		glEnable(GL_TEXTURE_2D);
		if(index == SUN)
			glDisable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, index);

		// set texture environment parameters

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluQuadricNormals(quadric, GLU_SMOOTH);

		gluQuadricTexture(quadric, GL_TRUE);
		gluSphere(quadric, radius, 100, 100);
		
		// glEnable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		



		// glutSolidSphere(radius, (int)radius * 6, (int)radius * 6);
		glPopMatrix();
	}
}

void Asteroid::setPosition(float x, float y, float z)
{
	centerX = x;
	centerY = y;
	centerZ = z;
}

Asteroid arrayAsteroids[ROWS][COLUMNS]; // Global array of asteroids.
// Function to check if two spheres centered at (x1,y1,z1) and (x2,y2,z2) with
// radius r1 and r2 intersect.
int checkSpheresIntersection(float x1, float y1, float z1, float r1,
							 float x2, float y2, float z2, float r2)
{
	return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2) <= (r1 + r2) * (r1 + r2));
}

// Function to check if the spacecraft collides with an asteroid when the center of the base
// of the craft is at (x, 0, z) and it is aligned at an angle a to to the -z direction.
// Collision detection is approximate as instead of the spacecraft we use a bounding sphere.
int asteroidCraftCollision(float x, float z, float a)
{
	int i, j;

	// Check for collision with each asteroid.
	for (j = 0; j < COLUMNS; j++)
		for (i = 0; i < ROWS; i++)
			if (arrayAsteroids[i][j].getRadius() > 0) // If asteroid exists.
				if (checkSpheresIntersection(x - 5 * sin((M_PI / 180.0) * a), 0.0,
											 z - 5 * cos((M_PI / 180.0) * a), 7.072,
											 arrayAsteroids[i][j].getCenterX(), arrayAsteroids[i][j].getCenterY(),
											 arrayAsteroids[i][j].getCenterZ(), arrayAsteroids[i][j].getRadius()))
					return 1;
	return 0;
}

// Routine to count the number of frames drawn every second.
void frameCounter(int value)
{
	if (value != 0) // No output the first time frameCounter() is called (from main()).
		std::cout << "FPS = " << frameCount << std::endl;
	frameCount = 0;
	glutTimerFunc(1000, frameCounter, 1);
}

void setup(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST); 

	  glEnable(GL_TEXTURE_2D);

    for (int i = 0; i < numTextures; ++i) {
        int imageWidth, imageHeight, imageChannels;

        unsigned char* imageData = stbi_load(texturePaths[i], &imageWidth, &imageHeight, &imageChannels, 0);

        glBindTexture(GL_TEXTURE_2D, i);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

        stbi_image_free(imageData);
    }

	spacecraft = glGenLists(1);
	glNewList(spacecraft, GL_COMPILE);
	glPushMatrix();
	glRotatef(180.0, 0.0, 1.0, 0.0); 
	glColor3f(1.0, 1.0, 1.0);
	glutWireCone(5.0, 10.0, 10, 1);
	glPopMatrix();
	glEndList();

	for (int j = 0; j < COLUMNS; j++)
	{
		for (int i = 0; i < ROWS; i++)
		{
			if (rand() % 100 < FILL_PROBABILITY)
			{
				float asteroidOrbitRadius = baseOrbitRadius + (i * orbitRadiusIncrement);
				if(i == MOON) {
					asteroidOrbitRadius = asteroidRadius[EARTH] + asteroidRadius[MOON] + 1.0;
				}

				arrayAsteroids[i][j] = Asteroid(0.0, 0.0, 0.0, asteroidRadius[i], asteroidColor[i][0], asteroidColor[i][1], asteroidColor[i][2], asteroidOrbitRadius);

			}
		}
	}

	

	glEnable(GL_LIGHT0); 

	float matAmb[] = {0.2, 0.2, 0.2, 1.0};

	float lightAmb[] = {1.0, 1.0, 1.0, 1.0}; 
	float lightDif[] = {1.0, 1.0, 1.0, 1.0}; 
	float lightSpc[] = {1.0, 1.0, 1.0, 1.0}; 
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpc);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	glutTimerFunc(0, frameCounter, 0); // Initial call of frameCounter().
}

void update()
{
	static float cnt = 0.0;
	cnt += 1.0;

	for (int j = 0; j < COLUMNS; j++)
	{
		for (int i = 0; i < ROWS - 2; i++)
		{
			float asteroidAngle = 2.0 * 3.14159 * (i + 1) / (ROWS + 1);

			float asteroidOrbitRadius = arrayAsteroids[i][j].getOrbitRadius();

			float posX = asteroidOrbitRadius * cos(asteroidAngle + 10 * (i + 1) + cnt * orbitSpeed / (i + 1));
			float posY = 0.0;
			float posZ = -asteroidOrbitRadius * sin(asteroidAngle + 10 * (i + 1) + cnt * orbitSpeed / (i + 1));

			arrayAsteroids[i][j].setPosition(posX, posY, posZ);
			if (i == EARTH)
			{
				float moonOrbitRadius = arrayAsteroids[MOON][j].getOrbitRadius();

				float moonPosX = posX + moonOrbitRadius * cos(5 * cnt * orbitSpeed);
				float moonPosY = posY;
				float moonPosZ = posZ - moonOrbitRadius * sin(5 * cnt * orbitSpeed);

				arrayAsteroids[MOON][0].setPosition(moonPosX, moonPosY, moonPosZ);
			}
		}
	}

	if (cnt * orbitSpeed > 360.0)
		cnt = 0.0;
}

void drawBorder() {
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(-5.0, -5.0, -5.0);
	glVertex3f(-5.0, 5.0, -5.0);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(-5.0, 5.0, -5.0);
	glVertex3f(5.0, 5.0, -5.0);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(-5.0, -5.0, -5.0);
	glVertex3f(5.0, -5.0, -5.0);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(5.0, -5.0, -5.0);
	glVertex3f(5.0, 5.0, -5.0);
	glEnd();
}

void drawSaturnRings(float r, float x, float y, float z) {
    glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(45.0, 1.0, 0.0, 0.0);
	glRotatef(45.0, 0.0, 0.0, 1.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidTorus(1, r * 1.5, 100, 100);
	glPopMatrix();
	
}

void drawSolarSystem() {

	arrayAsteroids[SUN][0].draw(8);

	arrayAsteroids[MOON][0].draw(9);

	for (int j = 0; j < COLUMNS; j++) {
		for (int i = 0; i < ROWS - 2; i++) {
			glDisable(GL_LIGHTING);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0);

			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_LINE_LOOP);
			for (int k = 0; k < 360; k++) {
				float rad = k * 3.14159 / 180;

				glVertex3f(arrayAsteroids[i][j].getOrbitRadius() * cos(rad), 0.0, -arrayAsteroids[i][j].getOrbitRadius() * sin(rad));
			}
			glEnd();
			glEnable(GL_LIGHTING);

			arrayAsteroids[i][j].draw(i);

			if(i == SATURN) {
				drawSaturnRings(arrayAsteroids[i][j].getRadius(), arrayAsteroids[i][j].getCenterX(), arrayAsteroids[i][j].getCenterY(), arrayAsteroids[i][j].getCenterZ());
			}
		}
	}

}

void sunLight() {
	glDisable(GL_LIGHTING);

	glPushMatrix();

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
	glColor3f(1.0, 1.0, 1.0);
	glutWireSphere(5, 20, 8);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}
// Drawing routine.
void drawScene(void)
{

	frameCount++; // Increment number of frames every redraw.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	sunLight();

	// Begin left viewport.
	// Space craft view port

	glViewport(0, 0, width, height);
	glLoadIdentity();

	// Write text in isolated (i.e., before gluLookAt) translate block.

	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos3f(-28.0, 25.0, -30.0);
	if (isCollision)
		writeBitmapString((void *)font, "Cannot - will crash!");
	glPopMatrix();

	// Locate the camera at the tip of the cone and pointing in the direction of the cone.
		
	gluLookAt(xVal - 10 * sin((M_PI / 180.0) * angle),
			yVal,
			zVal - 10 * cos((M_PI / 180.0) * angle),
			xVal - 11 * sin((M_PI / 180.0) * angle),
			yVal,
			zVal - 11 * cos((M_PI / 180.0) * angle),
			0.0,
			1.0,
			0.0);

	// Draw solar system
	drawSolarSystem();

	// End right viewport.
	// --------------------------------------------------------------------------

	// Begin right viewport.
	// Solar system view port
	glEnable(GL_SCISSOR_TEST);
	glScissor(width - width / 4 - 10, 10, width / 4, height / 4);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(width - width / 4 - 10, 10, width / 4, height / 4); // demo
	glLoadIdentity();

	// Write text in isolated (i.e., before gluLookAt) translate block.
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos3f(-28.0, 25.0, -30.0);
	if (isCollision)
		writeBitmapString((void *)font, "Cannot - will crash!");
	glPopMatrix();


	// Draw border of the solar system
	drawBorder();

	// Fixed camera on the plane of the solar system
	gluLookAt(0.0, 130.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);

	// Draw solar system
	drawSolarSystem();
	

	// Draw spacecraft.
	glPushMatrix();
	glTranslatef(xVal, yVal, zVal);	
	glRotatef(angle, 0.0, 1.0, 0.0);
	glCallList(spacecraft);
	glPopMatrix();



	// Update positions
	update();

	glutSwapBuffers();
	// render the scene
	glutPostRedisplay();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 250.0);
	glMatrixMode(GL_MODELVIEW);

	// Pass the size of the OpenGL window.
	width = w;
	height = h;
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	float tempxVal = xVal, tempzVal = zVal, tempAngle = angle;

	// Compute next position.
	if (key == GLUT_KEY_LEFT)
		tempAngle = angle + 5.0;
	if (key == GLUT_KEY_RIGHT)
		tempAngle = angle - 5.0;
	if (key == GLUT_KEY_UP)
	{
		tempxVal = xVal - sin(angle * M_PI / 180.0);
		tempzVal = zVal - cos(angle * M_PI / 180.0);
	}
	if (key == GLUT_KEY_DOWN)
	{
		tempxVal = xVal + sin(angle * M_PI / 180.0);
		tempzVal = zVal + cos(angle * M_PI / 180.0);
	}

	// Angle correction.
	if (tempAngle > 360.0)
		tempAngle -= 360.0;
	if (tempAngle < 0.0)
		tempAngle += 360.0;

	// Move spacecraft to next position only if there will not be collision with an asteroid.
	if (!asteroidCraftCollision(tempxVal, tempzVal, tempAngle))
	{
		isCollision = 0;
		xVal = tempxVal;
		zVal = tempzVal;
		angle = tempAngle;
		// std::cout << "xVal: " << xVal << " zVal: " << zVal << " angle: " << angle << std::endl;
	}
	else
		isCollision = 1;
	glutPostRedisplay();
}

void mouseControl(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		yVal += 1.0;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		yVal -= 1.0;
	}



	if (!asteroidCraftCollision(xVal, zVal, angle))
	{
		isCollision = 0;
	}
	else
		isCollision = 1;
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press the left/right arrow keys to turn the craft." << std::endl
			  << "Press the up/down arrow keys to move the craft." << std::endl
			  << "Press mouse left button to move the craft up." << std::endl
			  << "Press mouse right button to move the craft down." << std::endl;

}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1200, 1200);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("spaceTravel.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);
	glutMouseFunc(mouseControl);


	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}