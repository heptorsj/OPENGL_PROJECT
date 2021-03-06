// Alumno : Jacales Rojas Hector Daniel
// No. Cta : 312240761
// Laboratorio De Computación Grafica
#include "Main.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <string>
#include "imageloader.hpp"
#include <time.h>
#include <math.h>
// TEXTURAS
int total = 0,s =0;
int intento = 1;
float anx = 0.0, any = 1.0, anz = 8.0;
float rotacion[13] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
float roty[13] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
int puntaje = 0;
float _angle = 0.0;
float angulo = -2.0;
GLuint _textureBrick, _textureDoor, _textureGrass, _textureRoof, _textureWindow, _textureSky, _texturePino;
float bx = 0, by = 0.1, bz = 6;
/*    Create checkerboard texture    */
#define    checkImageWidth 64
#define    checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
GLdouble z1 = 0.0, x1 = 0.0, y2 = 1.0; // rotacion de camera
#ifdef GL_VERSION_1_1
static GLuint texName;
#endif
float randCoord() {
	return -10.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (13.5)));
}
float randAngle() {
	return -45 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(90)));
}
int randInt(int min, int max) {
	return min + (rand()) / (RAND_MAX / (max));

}
void sleepcp(int milliseconds) // Cross-platform sleep function
{
	clock_t time_end;
	time_end = clock() + milliseconds * 500 / 1000;
	while (clock() < time_end)
	{
	}
}

void makeCheckImage(void)
{
	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
		0,                            //0 for now
		GL_RGB,                       //Format OpenGL uses for image
		image->width, image->height,  //Width and height
		0,                            //The border of the image
		GL_RGB, //GL_RGB, because pixels are stored in RGB format
		GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
		//as unsigned numbers
		image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

//load objs
class obj3dmodel
{
	struct vertex {
		double x;
		double y;
		double z;
	};
	struct face {
		unsigned int v1, v2, v3, v4;
	};
	std::vector<vertex> vetexes;
	std::vector<face> faces;

public:
	void readfile(std::string filename);
	void draw();
	float angulo = 0.0;
};

void obj3dmodel::readfile(std::string filename)
{
	std::string s;
	this->angulo = randAngle();
	std::ifstream fin(filename);
	int contador = 0;
	if (!fin)
		return;
	while (fin >> s)
	{
		std::string aux = "v",aux2 = "f";
		//std::cout << s << "\n";
		if (s.compare(aux) == 0 )  {
			// cargando vertice 
			//std::cout << "Cargando Vertice" << "\n";
			vertex v;
			fin >> v.x ;
			fin >> v.y; 
			fin >> v.z;
			this->vetexes.push_back(v);
		} else if (s.compare(aux2) == 0) {
			//std::cout << contador << "\n";
			face f;
			fin >> f.v1;
			//std::cout << f.v3 << "\n";
			fin >> s;
			fin >> f.v2;
			fin >> s;
			fin >> f.v3;
			fin >> s;
			fin >> f.v4;
			this->faces.push_back(f);
			//contador++;
		}
	}
	
	fin.close();
}int b = 0;
void obj3dmodel::draw()
{
	//std::cout << this->faces.size() << "\n";
	
	glBegin(GL_QUADS);
	for (int i = 0; i < faces.size(); i++)
	{
		//std::cout << i << '/n' ;
		vertex v1 = vetexes[faces[i].v1 - 1];
		vertex v2 = vetexes[faces[i].v2 - 1];
		vertex v3 = vetexes[faces[i].v3 - 1];
		vertex v4 = vetexes[faces[i].v4 - 1];
		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v3.x, v3.y, v3.z);
		glVertex3f(v4.x, v4.y, v4.z);
	}
	glEnd();
	
	b++;
}
// programa
obj3dmodel bola,pino[15];
GLdouble camina_adelante = 0.0, camina_hor = 0.0; // movimiento de camara en x y z
void init(void)
{
	bola.readfile("ball.obj");
	for (int i = 0; i < 15; i++) {
		pino[i].readfile("bolo.obj");
	}
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	makeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef GL_VERSION_1_1
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
#endif

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#ifdef GL_VERSION_1_1
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, 4, checkImageWidth, checkImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
#endif

	Image* image = loadBMP("/Users/Daniel Jacales/Downloads/bricks.bmp");
	_textureBrick = loadTexture(image);
	image = loadBMP("/Users/Daniel Jacales/Downloads/door.bmp");
	_textureDoor = loadTexture(image);
	image = loadBMP("/Users/Daniel Jacales/Downloads/grass.bmp");
	_textureGrass = loadTexture(image);
	image = loadBMP("wood.bmp");
	_textureRoof = loadTexture(image);
	image = loadBMP("ruby.bmp");
	_textureWindow = loadTexture(image);
	image = loadBMP("wall.bmp");
	_textureSky = loadTexture(image);
	image = loadBMP("marmol.bmp");
	_texturePino = loadTexture(image);
	delete image;
}
// MATERIALES 

//FUNCION QUE DESPLIEGA EL GRID
void piso() {
	
		glBegin(GL_QUADS);
		glTexCoord3f(0.0, 1.0, 0.1); glVertex3f(-10, 0, -10);
		glTexCoord3f(1.0, 1.0, 0.1); glVertex3f(10, 0, -10);
		glTexCoord3f(1.0, 0.0, 0.1); glVertex3f(10, 0, 10);
		glTexCoord3f(0.0, 0.0, 0.1); glVertex3f(-10, 0, 10);
		glEnd();
	
}
float tpinos[13] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
void drawPinos() {
	int actual = 0;
	int filas = 5; int columnas = 5;
	float inicial = -1.35;
	
	for (int i = 0; i < filas; i++) {
		for (int j = 0; j < columnas; j++) {
			glPushMatrix();
			//bx by bz
			glTranslatef(inicial+(j*0.35), 0.1 , -5 - (i* 0.3) );
			glScalef(0.01, 0.01, 0.02);
			glTranslatef(tpinos[actual], 0, tpinos[actual]);
			glRotatef(270, 1, 0, 0);
			glRotatef(roty[actual], 0, 1, 1);
			glRotatef(rotacion[actual],1,0,0);
			pino[actual].draw();
			glPopMatrix();
			actual++;
		}
		columnas--;
		inicial += 0.08;
	}

}
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();             /* clear the matrix */
	/* viewing transformation  */
	gluLookAt(anx, any, anz, x1, y2, z1, 0.0, 1.0, 0.0);
	glScalef(1.0, 2.0, 1.0);      /* modeling transformation */
	glPushMatrix();
	glTranslatef(camina_hor, 0, camina_adelante);
	glPushMatrix();
	glTranslatef(0, 0.5, 0);
	glBindTexture(GL_TEXTURE_2D, _textureSky);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTranslatef(0, 0, -10);
	glBegin(GL_QUADS);
	glTexCoord3f(0.0, 1.0, 0.1);  glVertex3f(-30, 20, 0);
	glTexCoord3f(1.0, 1.0, 0.1);  glVertex3f(20, 20, 0);
	glTexCoord3f(1.0, 0.0, 0.1);  glVertex3f(20, -20, 0);
	glTexCoord3f(0.0, 0.0, 0.1);  glVertex3f(-30, -20, 0);
	glEnd();
	glPopMatrix();
	glLineWidth(2.0);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, _textureRoof);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_QUADS);
	glEnd();
	glPopMatrix();
	piso();
	 //BOLA
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, _textureWindow);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_QUADS);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	//bx by bz
	glTranslatef(bx, by, bz);
	glScalef(0.5, 0.25, 0.5);
	glRotatef(270, 1, 0, 0);
	glutSolidSphere(0.5, 10, 10);
	glPopMatrix();
	// PINOS
	/*
	*/
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, _texturePino);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_QUADS);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glRotatef(525,0,1,0);
	glTranslatef(2.22, 0, 10);
	drawPinos();
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glFlush();
	glutSwapBuffers();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
}
bool izq = true,startg = false;
void pregame() {
	if (izq) {
		if (bx > -1.5) {
			bx -= 0.1;
			glutPostRedisplay();
		}else{
			izq = false;
		}
	}
	else {
		if (bx < 1.5) {
			bx += 0.1;
			glutPostRedisplay();
		}
		else {
			izq = !izq;
		}
	}

}
bool active = true;
void animacion1() {
	if (active) {
		if (bz < -4) {
			for (int i = 0; i < 13; i++) {
				rotacion[i] = -90;
				roty[i] = randAngle();
				tpinos[i] = randCoord();
				glutPostRedisplay();
				//std::cout << roty[i];
			}
			active = false;
		}
		
	}
}
void animacion2() {
	if (active) {
		if (bz < -4) {
			int pino1 = randInt(0, 5);
			int pino2 = randInt(6, 9);
			int pino3 = randInt(10, 13);
			//pino1
			rotacion[pino1] = -90;
			roty[pino1] = randAngle();
			tpinos[pino1] = randCoord();
			glutPostRedisplay();
			//pino2
			rotacion[pino2] = -90;
			roty[pino2] = randAngle();
			tpinos[pino2] = randCoord();
			glutPostRedisplay();
			//pino3
			rotacion[pino3] = -90;
			roty[pino3] = randAngle();
			tpinos[pino3] = randCoord();
			glutPostRedisplay();
			if (bx < 0) {
				rotacion[11] = -90;
				roty[11] = randAngle();
				tpinos[11] = randCoord();
				glutPostRedisplay();
				// otro pino
				rotacion[6] = -90;
				roty[6] = randAngle();
				tpinos[6] = randCoord();
				glutPostRedisplay();
			}
			else {
				rotacion[2] = -90;
				roty[2] = randAngle();
				tpinos[2] = randCoord();
				glutPostRedisplay();
				// otro pino
				rotacion[8] = -90;
				roty[8] = randAngle();
				tpinos[8] = randCoord();
				glutPostRedisplay();

			}
			active = false;
		}
	}


}
int puntajef() {
	int p = 0;
	if (bx > -0.5 && bx < 0.5) {
		animacion1();
	}
	if ((bx > -1.2 && bx <= -0.5) || (bx > 0.5 && bx < 1.2)) {
		animacion2();
	}
	else {
		// no se tira ningun bolo

	}
	//std::cout << p << "\n";
	return p;
}
void start() {
	puntaje = puntajef();
	if (bz > -11) {
		bz -= 0.3;
		glutPostRedisplay();
	}
	else {

	}
}
void game() {
	if (!startg) {
		pregame();
	}
	else {
		start();
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		x1 += 0.2;
		glutPostRedisplay();
		break;
	case 'A':
		x1 -= 0.2;
		glutPostRedisplay();
		break;
	case 'd':
		y2 -= 1.0;
		glutPostRedisplay();
		break;
	case 'D':
		y2 += 1.0;
		glutPostRedisplay();
		break;
	case 'w':
		z1 += 1.0;
		glutPostRedisplay();
		break;
	case 'W':
		z1 -= 1.0;
		glutPostRedisplay();
		break;
	case 's':
		camina_adelante += 0.5;
		glutPostRedisplay();
		break;
	case 'S':
		camina_adelante -= 0.5;
		glutPostRedisplay();
		break;
	case 'Z':
			camina_hor -= 0.5;
			glutPostRedisplay();
			break;
	case 'z':
			camina_hor += 0.5;
			glutPostRedisplay();
			break;
	case 32:
		s = 0;
		if (bx > -0.5 && bx < 0.5) {
			s = randInt(11, 11);
			std::cout << "Usted Tiro ";
			std::cout << s ;
			std::cout << " Pinos\n";
		}else if ((bx > -1.2 && bx <= -0.5) || (bx > 0.5 && bx < 1.2)) {
			s = randInt(2, 6);
			std::cout << "Usted Tiro ";
			std::cout << s;
			std::cout << " Pinos\n";
			animacion2();
		}
		else {
			std::cout << "Usted No Tiro Ningun Pino\n";

		}
		total += s;
		intento++;
		startg = true;
		anx = -5;
		any = 1.5;
		anz = 0;
		camina_hor = 5;
		z1 = -1;
		//p = randInt(11, 13); // chuza
		//std::cout << p;
		break;
	case 'r':
		if (intento > 5) {
			std::cout << "Juego Terminado ";
			std::cout << total;
			std::cout << " Puntos ! \n  Comenzando Nuevo Juego\n";
			intento = 0;

		}
		startg = false;
		active = true;
		bx = 0;
		bz = 6;
		camina_hor = 0;
		z1 = 0;
		anx = 0;
		any = 1;
		anz = 8;
		for (int i = 0; i < 13; i++) {
			rotacion[i] = 0;
			roty[i] = 0;
			glutPostRedisplay();
		}
		glutPostRedisplay();
		break;
	case 'f':
		animacion1();
		break;
	case 'p':
		angulo += 0.5;
		std::cout << angulo << "\n";
		glutPostRedisplay();
		break;
		case 27:
			exit(0);
			break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(game);
	glutMainLoop();
	return 0;
}
