#include <vgl.h>
#include <InitShader.h>
#include <vec.h>
#include "MyPlain.h"

float gTime = 0.0f;
float rotate = 0.0f;
bool shaderButton = false;
GLuint prog;
MyPlain plain;

void init()
{
	plain.init();
	prog = InitShader("vShader.glsl", "fShader.glsl");
	glUseProgram(prog);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(prog);
	GLuint uTime = glGetUniformLocation(prog, "uTime");
	glUniform1f(uTime, gTime);

	GLuint uShaderButton = glGetUniformLocation(prog, "uShaderButton");
	glUniform1f(uShaderButton, shaderButton);

	plain.draw(prog);
	glFlush();
}

void myIdle()
{
	gTime += rotate;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		if (plain.division > 2)
		{
			plain.division--;
			plain.NUM_TRIANGLES = plain.division * plain.division * 2;
			plain.NUM_POINTS = 3 * plain.NUM_TRIANGLES;
			plain.reshape();
			plain.reshape();
		}
		printf("Division: %d, Num.of Triangles: %d, Num. of Vertices: %d\n", plain.division, plain.NUM_TRIANGLES, plain.NUM_POINTS);
		break;
	case '2':
		plain.division++;
		plain.NUM_TRIANGLES = plain.division * plain.division * 2;
		plain.NUM_POINTS = 3 * plain.NUM_TRIANGLES;
		plain.reshape();
		printf("Division: %d, Num.of Triangles: %d, Num. of Vertices: %d\n", plain.division, plain.NUM_TRIANGLES, plain.NUM_POINTS);
		break;
	case 32:	//space
		if (rotate == 0.0f)
		{
			rotate = 0.00005f;
		}
		else
		{
			rotate = 0.0f;
		}
		break;
	case 'w':
		if (shaderButton)
		{
			shaderButton = false;
		}
		else
		{
			shaderButton = true;
		}
		break;
	case 'q':
		exit(1);
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Waving Plain");

	glewExperimental = true;
	glewInit();

	init();
	glutIdleFunc(myIdle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}