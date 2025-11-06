#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <algorithm>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"
#include <list>

MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);

GLuint program;
GLuint uMat;

mat4 CTM;

bool bPlay = false;
bool bChasingTarget = false;
bool bDrawTarget = false;

float ang1 = 0;
float ang2 = 0;
float ang3 = 0;
vec3 prePos = vec3(0.0, 0.4, 0.0);

void MyInit()
{
	cube.Init();
	pyramid.Init();

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}

float mytime = 0;

void DrawRobotArm(float ang1, float ang2, float ang3)
{
	std::list<mat4> matrixStack;
	mat4 CTM(1.0);
	{
		CTM = Translate(0, -0.4f, 0) *RotateY(mytime * 30);
		matrixStack.push_back(CTM);

	// BASE
		mat4 M(1.0);
		CTM *= Translate(0, 0, 0.085);
		M = Scale(0.3, 0.2, 0.05);
		glUniformMatrix4fv(uMat, 1, true, CTM * M);
		pyramid.Draw(program);
		CTM = matrixStack.back();

		CTM *= Translate(0, 0, -0.085);
		M = Scale(0.3, 0.2, 0.05);
		glUniformMatrix4fv(uMat, 1, true, CTM * M);
		pyramid.Draw(program);
		CTM = matrixStack.back();

		CTM *= Translate(0.12, -0.08, 0);
		M = Scale(0.06, 0.04, 0.12);
		glUniformMatrix4fv(uMat, 1, true, CTM * M);
		pyramid.Draw(program);
		CTM = matrixStack.back();

		CTM *= Translate(-0.12, -0.08, 0);
		M = Scale(0.06, 0.04, 0.12);
		glUniformMatrix4fv(uMat, 1, true, CTM * M);
		pyramid.Draw(program);
		CTM = matrixStack.back();

		CTM *= RotateZ(ang1);
		M = Scale(0.04, 0.04, 0.23);
		glUniformMatrix4fv(uMat, 1, true, CTM * M);
		cube.Draw(program);
		CTM = matrixStack.back();

		{
		// UPPER ARM
			CTM *= RotateZ(ang1);
			matrixStack.push_back(CTM);

			//UPPER - BASE 연결부
			CTM *= Translate(0, -0.18, 0);
			M = Translate(0, 0.18, 0) * Scale(0.16, 0.08, 0.12);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			//기둥
			CTM *= Translate(0.055, 0, 0.045);
			M = Translate(0, 0.18, 0) * Scale(0.05, 0.28, 0.03);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			CTM *= Translate(0.055, 0, -0.045);
			M = Translate(0, 0.18, 0) * Scale(0.05, 0.28, 0.03);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			CTM *= Translate(-0.055, 0, 0.045);
			M = Translate(0, 0.18, 0) * Scale(0.05, 0.28, 0.03);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			CTM *= Translate(-0.055, 0, -0.045);
			M = Translate(0, 0.18, 0) * Scale(0.05, 0.28, 0.03);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			//UPPER - LOWER 연결부
			CTM *= Translate(0, 0.17, 0.045);
			M = Translate(0, 0.18, 0) * Scale(0.16, 0.06, 0.03);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			CTM *= Translate(0, 0.17, -0.045);
			M = Translate(0, 0.18, 0) * Scale(0.16, 0.06, 0.03);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			CTM *= Translate(0, 0.17, 0);
			M = Translate(0, 0.18, 0) * Scale(0.03, 0.03, 0.13);
			glUniformMatrix4fv(uMat, 1, true, CTM * M);
			cube.Draw(program);
			CTM = matrixStack.back();

			{
			// LOWER ARM
				CTM *= Translate(0, 0.37, 0) * RotateZ(ang2); 
				matrixStack.push_back(CTM);

				//LOWER - UPPER 연결부
				CTM *= Translate(0, -0.06, 0);
				M = Translate(0, 0.08, 0) * Scale(0.09, 0.12, 0.06);
				glUniformMatrix4fv(uMat, 1, true, CTM * M);
				cube.Draw(program);
				CTM = matrixStack.back();

				CTM *= Translate(0, -0.01, 0);
				M = Translate(0, 0.08, 0) * Scale(0.17, 0.04, 0.07);;
				glUniformMatrix4fv(uMat, 1, true, CTM* M);
				cube.Draw(program);
				CTM = matrixStack.back();

				//기둥
				CTM *= Translate(0, -0.01, 0.02);
				M = Translate(0.05, 0.2, 0) * Scale(0.06, 0.25, 0.02);
				glUniformMatrix4fv(uMat, 1, true, CTM* M);
				cube.Draw(program);
				CTM = matrixStack.back();

				CTM *= Translate(0, -0.01, -0.02);
				M = Translate(0.05, 0.2, 0) * Scale(0.06, 0.25, 0.02);
				glUniformMatrix4fv(uMat, 1, true, CTM* M);
				cube.Draw(program);
				CTM = matrixStack.back();

				CTM *= Translate(0, -0.01, 0.02);
				M = Translate(-0.05, 0.2, 0) * Scale(0.06, 0.25, 0.02);
				glUniformMatrix4fv(uMat, 1, true, CTM* M);
				cube.Draw(program);
				CTM = matrixStack.back();

				CTM *= Translate(0, -0.01, -0.02);
				M = Translate(-0.05, 0.2, 0) * Scale(0.06, 0.25, 0.02);
				glUniformMatrix4fv(uMat, 1, true, CTM* M);
				cube.Draw(program);
				CTM = matrixStack.back();
				 
				//LOWER - HAND 연결부
				CTM *= Translate(0, 0.25, 0);
				M = Translate(0, 0.08, 0) * Scale(0.17, 0.04, 0.09);;
				glUniformMatrix4fv(uMat, 1, true, CTM* M);
				cube.Draw(program);
				CTM = matrixStack.back();

				CTM *= Translate(0, 0.3, 0);
				M = Translate(0, 0.08, 0) * Scale(0.06, 0.12, 0.015);
				glUniformMatrix4fv(uMat, 1, true, CTM* M);
				cube.Draw(program);
				CTM = matrixStack.back();

				{
				// HAND
					CTM *= Translate(0, 0.4, 0) * RotateZ(ang3);
					matrixStack.push_back(CTM);

					//HAND - LOWER 연결부
					M = Scale(0.03, 0.03, 0.09);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.01, 0, -0.025);
					M = Scale(0.1, 0.06, 0.03);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.01, 0, 0.025);
					M = Scale(0.1, 0.06, 0.03);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.06, 0, 0);
					matrixStack.push_back(CTM);

					M = Scale(0.02, 0.06, 0.065);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					M = Scale(0.02, 0.06, 0.065);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.02, 0, 0);
					M = Scale(0.03, 0.03, 0.04);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.04, 0, 0);
					M = Scale(0.01, 0.06, 0.04);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					//HAND
					CTM *= Translate(-0.04, 0, 0);
					matrixStack.push_back(CTM);

					CTM *= Translate(-0.03, 0.05, 0) * RotateZ(-45);
					M = Scale(0.08, 0.01, 0.04);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.03, -0.05, 0) * RotateZ(45);
					M = Scale(0.08, 0.01, 0.04);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.09, 0.055, 0) * RotateZ(30);
					M = Scale(0.08, 0.01, 0.04);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();

					CTM *= Translate(-0.09, -0.055, 0) * RotateZ(-30);
					M = Scale(0.08, 0.01, 0.04);
					glUniformMatrix4fv(uMat, 1, true, CTM* M);
					cube.Draw(program);
					CTM = matrixStack.back();
				}
			}
		}
	}
}

void ComputeAngle()
{
	vec3 targetPos = target.GetPosition(mytime); 
	double x, y, z;
	double a1 = 0.37;
	double a2 = 0.4;
	double a3 = 0.2;
	double pi = M_PI;
	double theta1, theta2, theta3;
	double step = 0.06;
	double currentDistance = sqrt((prePos.x - targetPos.x) * (prePos.x - targetPos.x) + 
(prePos.y - targetPos.y) * (prePos.y - targetPos.y));

	if (currentDistance > a3) 
	{
		if (targetPos.x > prePos.x)
			x = prePos.x + step * currentDistance;
		else
			x = prePos.x - step * currentDistance;
		if (targetPos.y > prePos.y)
			y = prePos.y + step * currentDistance;
		else
			y = prePos.y - step * currentDistance;
		z = targetPos.z;
	}
	else
	{
		x = prePos.x;
		y = prePos.y;
		z = prePos.z;
	}

	theta2 = acosf((x * x + y * y - a1 * a1 - a2 * a2) / (2 * a1 * a2));
	theta1 = atan2(y, x) - atan2(a2 * sin(theta2), a1 + a2 * cos(theta2));
	theta3 = atan2((prePos.y - targetPos.y), (prePos.x - targetPos.x)) - theta1 - theta2;

	ang1 = -90 + theta1 * (180 / pi);
	ang2 = theta2 * (180 / pi);
	ang3 = 90 + theta3 * (180 / pi);

	prePos = vec3(x, y, z);
}


void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLuint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, -1, -1, -1, -1);


	uMat = glGetUniformLocation(program, "uMat");
	CTM = Translate(0, -0.4, 0) * RotateY(mytime * 30);
	DrawRobotArm(ang1, ang2, ang3);


	glUniform4f(uColor, 1, 0, 0, 1);
	if (bDrawTarget == true)
		target.Draw(program, CTM, mytime);

	glutSwapBuffers();
}

void MyIdle()
{
	if (bPlay)
	{
		mytime += 1 / 60.0f;
		Sleep(1.0 / 60.0f * 1000);

		if (bChasingTarget == false)
		{
			ang1 = 45 * sin(mytime * 3.141592);
			ang2 = 60 * sin(mytime * 2 * 3.141592);
			ang3 = 30 * sin(mytime * 3.141592);
		}
		else
			ComputeAngle();

		glutPostRedisplay();
	}
}

void MyKeyboard(unsigned char c, int x, int y)
{

	switch (c)
	{
	case '1':
		bChasingTarget = !bChasingTarget;
		break;
	case '2':
		bDrawTarget = !bDrawTarget;
		break;
	case '3':
		target.toggleRandom();
		break;
	case ' ':
		bPlay = !bPlay;
		break;
	default:
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Simple Robot Arm");

	glewExperimental = true;
	glewInit();

	MyInit();
	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutIdleFunc(MyIdle);

	glutMainLoop();

	return 0;
}