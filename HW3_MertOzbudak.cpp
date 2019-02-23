//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 400
#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1   // 0:disable timer, 1:enable timer
#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;//ADD SPACEBAR TO FIRE
int  winWidth, winHeight; // current Window width and height

bool check = false;//FOR CHECKER FUNCTION

float A = 100, fq = 1, C = 0, B = 0;
float angler = 0;//FOR HARMONIC MOVEMENTS

typedef struct {
	float x,
		y;
} point_t;

point_t addV(point_t v1, point_t v2) {
	point_t tmp = { v1.x + v2.x, v1.y + v2.y };
	return tmp;
}
// convert from polar representation to rectangular representation
point_t pol2rec(float mag, float angle) {
	point_t tmp = { mag * cos(angle * D2R), mag * sin(angle * D2R) };
	return tmp;
}
/* Vector library Ends */

/* User defined Types and variables for the application */
#define FOV  5   // Field of View Angle
#define FOV1  10

typedef struct{
	point_t m;//position
	float angle;
	float rad;
}player_t;
typedef struct {
	point_t pos;
	float angle;
	bool active;
	float  rad;
} fire_t;

player_t player = { { -300, 0 },  0, 40 };
fire_t   fire = { { -210, 0}, 0, false, 5};
point_t  p;

float f(float x) {
	return A * sin((fq * x + C) * D2R) + B;
}
//
// to draw circle, center at (x,y)
//  radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}
void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}
void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}
// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}
void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}
void drawFiremachine(player_t mac)
{
	point_t vEnd = addV(player.m, pol2rec(30, player.angle));
	point_t E1 = addV(player.m, pol2rec(120, player.angle - FOV));
	point_t E2 = addV(player.m, pol2rec(120, player.angle + FOV1));
	
	//TO DRAW TRIANGLE FOR BARREL
    glColor3f(0.3, 1, 0.3);
	glBegin(GL_TRIANGLES);
	glVertex2f(mac.m.x, mac.m.y);
	glVertex2f(E1.x, E1.y);
	glVertex2f(E2.x, E2.y);
	glEnd();
	
	//TO DRAW BARREL LINE
	glLineWidth(4);
	glColor3ub(80, 200, 100);
	glBegin(GL_LINE_LOOP);
	glVertex2f(mac.m.x, mac.m.y);
	glVertex2f(E1.x, E1.y);
	glVertex2f(E2.x, E2.y); 
	glEnd();
	
	//TO DRAW CIRCLES FOR BODY OF FIRE MACHINE
	glColor3f(0, 0, 0);
	circle(mac.m.x, mac.m.y, mac.rad+10);
	glColor3f(1, 0, 0.1);
	circle(mac.m.x, mac.m.y, mac.rad - 10);
	glColor3f(0.1, 0, 1.0);
	circle(mac.m.x, mac.m.y, mac.rad - 30);
	glEnd();
	glColor3f(0, 0, 0);
	
	
}
void drawFire(fire_t fire)
{
	if (fire.active) 
	{
		glColor3f(0, 0, 1);
		glLineWidth(2);
		circle_wire(fire.pos.x, fire.pos.y, 5);
		//ALTERNATIVE DRAW FIRE
		/*
		glColor3f(0, 0, 1);
		circle(fire.pos.x, fire.pos.y, 6);
		glColor3f(1, 1, 1);
		circle(fire.pos.x, fire.pos.y, 3);
		*/
	}
}
void drawTarget()
{
	//DRAW FACE BORDER
	glColor3f(0, 0, 0);
	circle(f(p.y) + 180, p.x + 10, 20);
	//DRAW YELLOW FACE
	glColor3f(1, 0.9, 0);
	circle(f(p.y) + 180, p.x + 10, 18);
	//DRAW EYES AND MOUTH FOR YELLOW FACE
	glColor3f(0, 0, 0);
	circle(f(p.y) + 173, p.x + 15, 3);//eye1
	circle(f(p.y) + 187, p.x + 15, 3);//eye2
	circle(f(p.y) + 180, p.x + 2, 5);//mouth
	glEnd();

}
void displayGameOver()
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0, 0, 0);
	vprint(player.m.x + 240, player.m.y , GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
}
bool checker()
{
	if (sqrtf(powf((fire.pos.x - (f(p.y) + 180)), 2) + powf((fire.pos.y - (p.x + 10)), 2)) <= fire.rad + 20)
		return check=true;
	else
		return check=false;
}
void display()
{

	//
	// clear window to black
	//
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	
	//IF CHECK TO INTERSECTION BETWEEN TWO CIRCLE
	if (checker())
	{
		displayGameOver();
	}
	else
	{
		glColor3f(0, 0, 1);
		vprint(player.m.x + 40, player.m.y + 145, GLUT_BITMAP_9_BY_15, "Mert Ozbudak");
		drawTarget();
		drawFire(fire);
		drawFiremachine(player);
	}
	
	glutSwapBuffers();


}
//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = true;
	// to refresh the window it calls display() function
	//glutPostRedisplay();
}
void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = false;

	// to refresh the window it calls display() function
	//glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	// to refresh the window it calls display() function
	//glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}
	// to refresh the window it calls display() function
	//glutPostRedisplay();
}
//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	//glutPostRedisplay();
}
//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}
void onMoveDown(int x, int y) {
	// Write your codes here.

	x = x - winWidth / 2;
	y = winHeight / 2 - y;

	// to refresh the window it calls display() function	
	//glutPostRedisplay();
}
void onMove(int x, int y) {
	// Write your codes here.

	int cx = x - winWidth / 2;
	int cy = winHeight / 2 - y;
	
	// to refresh the window it calls display() function
	//glutPostRedisplay();
}
void turnPlayer(player_t *p, float inc){
	
	p->angle += inc;
	
	if (p->angle > 360) 
		p->angle -= 360;
	if (p->angle < 0) 
		p->angle += 360;
}
#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (!checker())//TO STOP THE GAME
	{
		//TO HARMONÝC MOVEMENTS FOR FACE
		angler+= 0.8;

		
		p.y = 40 * sin(8* angler* D2R)+30;
		p.x = 150 * sin(2 * angler * D2R) - 10;
		
		//TO MOVE BARREL
		if (down) 
		{
			if (player.angle != 315)
			{   
				turnPlayer(&player, -3);
			}
		}
		if (up) 
		{
			if (player.angle != 45)
			{
				turnPlayer(&player, 3);
			}
		}

		//TO FIRE
		if (spacebar && fire.active == false) 
		{
			fire.pos = player.m;
			fire.angle = player.angle;
			fire.active = true;
		}
		
		if (fire.active)
		{
			fire.pos.x += 20 * cos((fire.angle +2)* D2R);
			fire.pos.y += 21 * sin((fire.angle +2)* D2R);

			if (fire.pos.x > 300 || fire.pos.x < -300 || fire.pos.y>200 || fire.pos.y < -200)
				fire.active = false;
		}
	}
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif
void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
}
void main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CTIS164-HW #3-2016");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}