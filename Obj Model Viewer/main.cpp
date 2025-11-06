#define _CRT_SECURE_NO_WARNINGS
#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyModel.h"
#include "MyCube.h"

#define STOP_ROTATION 0
#define X_AXIS_ROTATION 1
#define Y_AXIS_ROTATION 2
#define Z_AXIS_ROTATION 3

MyModel model;
MyCube cube;
int MyModel::NumVertices = 0;

GLuint program;
GLuint phong_prog;
GLuint uMat;
GLuint uColor;

mat4 CTM(1.0f);

FILE* fp;

float g_Time = 0;
float g_aspect = 1.0f;
float mShiny = 50.0f;		//1~100
float spec = 0.5f;

char file[20];
float rotateSpeed = 45;
float rx = 0, ry = 0, rz = 0;
float normalType = 0.0f;
int RotateDirection = 0;

void InputFileName()
{
	printf("SIMPLE OBJ MODEL VIEWER\n");
	printf("\n----------------------------------------------------------------\n");
	printf("Spacebar: starting/stoping rotation\n\nLeft Mouse Button: rotating around x-axis\n");
	printf("Middle Mouse Button: rotating around y-axis\nRight Mouse Button: rotating around z-axis\n\n");
	printf("'1' key: Using Vertex Normal for shading\n'2' key: Using Surface Normal for shading\'3' key: Increasing Specular effect (ks)\n");
	printf("'4' key: Decreasing Specular effect (ks)\n'5' key: Increasing Shininess (n)\n'6' key: Decreasing Shininess (n)\n\n");
	printf("'Q' Key: Exit the program.\n");
	printf("----------------------------------------------------------------\n\n");
	while (true) {
		printf("Input File Path: ");
		scanf("%s", file);
		fp = fopen(file, "r");
		if (!fp) {
			printf("File not Found!\n");
			continue;
		}
		else {
			fclose(fp);
			break;
		}
	}
}

void MyInit()
{
	model.Init(file);
	cube.Init();
	phong_prog = InitShader("vphong.glsl", "fphong.glsl");
}

mat4 MyLookAt(vec3 e, vec3 f, vec3 u)
{
	vec3 n = normalize(f - e);
	float a = dot(u, n);
	vec3 v = normalize(u - a * n);
	vec3 w = cross(v, -n);

	mat4 Rw(1.0f);
	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	mat4 Rc(1.0f);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Rc[i][j] = Rw[j][i];

	mat4 Tc = Translate(-e.x, -e.y, -e.z);

	mat4 V = Rc * Tc;
	return V;
}

mat4 MyPerspective(float fovy, float aspect, float nearr, float farr)
{
	float ymax = farr * tan(fovy / 2.0f / 180.0f * 3.141592);
	float xmax = ymax * aspect;
	mat4 S(1.0f);
	S[0][0] = 1.0f / xmax;
	S[1][1] = 1.0f / ymax;
	S[2][2] = 1.0f / farr;

	float c = -nearr / farr;
	mat4 M(1.0f);
	M[2][2] = 1.0f / (c + 1.0f);
	M[2][3] = -c / (c + 1.0f);
	M[3][2] = -1.0f;
	M[3][3] = 0.0f;

	return M * S;
}

void MyDisplay()
{
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(phong_prog);
	GLuint uColor = glGetUniformLocation(phong_prog, "uColor");

	// ÇØ´ç ÆÄÀÏ ¸ðµ¨
	mat4 ModelMat = MyLookAt(vec3(0, 0, 4), vec3(0, 0, 0), vec3(0, 1, 0)) * CTM;
	mat4 ProjMat = MyPerspective(60, g_aspect, 0.1, 100);
	GLuint uModelMat = glGetUniformLocation(phong_prog, "uModelMat");
	GLuint uProjMat = glGetUniformLocation(phong_prog, "uProjMat");

	// ÁÂÇ¥Ãà
	glUniformMatrix4fv(uModelMat, 1, true, ModelMat * Translate(1.15, 0, 0) * Scale(2.3, 0.02, 0.02));
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(phong_prog);

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat * Translate(0, 1.15, 0) * Scale(0.02, 2.3, 0.02));
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(phong_prog);

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat * Translate(0, 0, 1.15) * Scale(0.02, 0.02, 2.3));
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(phong_prog);

	ModelMat *= Scale(model.totalScale, model.totalScale, model.totalScale);
	// 1. Define Light Properties
	// 
	vec4 lPos = vec4(2, 2, 0.5, 1);
	vec4 lAmb = vec4(0.5, 0.5, 0.5, 1);
	vec4 lDif = vec4(0.5, 0.5, 0.5, 1);
	vec4 lSpc = vec4(1, 1, 1, 1);// lDif;

	// 2. Define Material Properties
	//
	vec4 mAmb = vec4(0.3, 0.3, 0.3, 1);
	vec4 mDif = vec4(0.8, 0.8, 0.8, 1);
	vec4 mSpc = vec4(spec, spec, spec, 1);

	// I = lAmb*mAmb + lDif*mDif*(N¡¤L) + lSpc*mSpc*(V¡¤R)^n; 
	vec4 amb = lAmb * mAmb;
	vec4 dif = lDif * mDif;
	vec4 spc = lSpc * mSpc;

	// 3. Send Uniform Variables to the shader
	//
	GLuint uLPos = glGetUniformLocation(phong_prog, "uLPos");
	GLuint uAmb = glGetUniformLocation(phong_prog, "uAmb");
	GLuint uDif = glGetUniformLocation(phong_prog, "uDif");
	GLuint uSpc = glGetUniformLocation(phong_prog, "uSpc");
	GLuint uShininess = glGetUniformLocation(phong_prog, "uShininess");
	GLuint uTime = glGetUniformLocation(phong_prog, "uTime");
	GLuint uNormalType = glGetUniformLocation(phong_prog, "uNormalType");

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat);
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniform4f(uLPos, lPos[0], lPos[1], lPos[2], lPos[3]);
	glUniform4f(uAmb, amb[0], amb[1], amb[2], amb[3]);
	glUniform4f(uDif, dif[0], dif[1], dif[2], dif[3]);
	glUniform4f(uSpc, spc[0], spc[1], spc[2], spc[3]);
	glUniform1f(uShininess, mShiny);
	glUniform1f(uNormalType, normalType);
	glUniform4f(uColor, 0, 0, 0, 0);
	glUniform1f(uTime, g_Time);

	model.Draw(phong_prog);
	glutSwapBuffers();
}

void MyIdle()
{
	if (RotateDirection == X_AXIS_ROTATION) {
		CTM *= RotateX(1);
	}
	else if (RotateDirection == Y_AXIS_ROTATION) {
		CTM *= RotateY(1);
	}
	else if (RotateDirection == Z_AXIS_ROTATION) {
		CTM *= RotateZ(1);
	}

	g_Time += 0.2f;
	Sleep(16);
	glutPostRedisplay();
}

void MyReshape(int wx, int wy)
{
	glViewport(0, 0, wx, wy);
	g_aspect = (float)wx / (float)wy;
}

void MyKeyboard(unsigned char c, int x, int y)
{
	switch (c)
	{
	case ' ':
		if (RotateDirection == STOP_ROTATION) {
			printf("Play!\n");
			RotateDirection = X_AXIS_ROTATION;
		}
		else {
			printf("Stop!\n");
			RotateDirection = STOP_ROTATION;
		}
		break;
	case '1':
		normalType = 0.0f;
		printf("Using Vertex Normal!\n");
		break;
	case '2':
		normalType = 1.0f;
		printf("Using Surface Normal!\n");
		break;
	case '3':
		if (spec < 1.0)
			spec += 0.1;
		printf("Increasing Specular Effect!\n");
		break;
	case '4':
		if (spec > 0.1)
			spec -= 0.1;
		printf("Decreasing Specular Effect!\n");
		break;
	case '5':
		if (mShiny < 100)
			mShiny += 10;
		printf("Increasing Shininess!\n");
		break;
	case '6':
		if (mShiny > 10)
			mShiny -= 10;
		printf("Decreasing Shininess!\n");
		break;
	default:
		break;
	}
}

void MyMouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && RotateDirection != STOP_ROTATION) {
		RotateDirection = X_AXIS_ROTATION;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP && RotateDirection != STOP_ROTATION) {
		RotateDirection = Y_AXIS_ROTATION;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && RotateDirection != STOP_ROTATION) {
		RotateDirection = Z_AXIS_ROTATION;
	}

}

int main(int argc, char** argv)
{
	InputFileName();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Homework4");

	glewExperimental = true;
	glewInit();

	MyInit();
	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutMouseFunc(MyMouseClick);
	glutIdleFunc(MyIdle);
	glutReshapeFunc(MyReshape);

	glutMainLoop();

	return 0;
}