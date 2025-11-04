#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyUtil.h"

#include <vec.h>
#include <mat.h>
#include <math.h>
#include <queue>
#include <list>

#define MAZE_FILE	"maze.txt"
#define STOP_ROTATE 0
#define LEFT_ROTATE 1
#define RIGHT_ROTATE 2

MyCube cube;
GLuint program;

mat4 g_Mat = mat4(1.0f);
GLuint uMat;
GLuint uColor;

float wWidth = 1000;
float wHeight = 500;

vec3 cameraPos = vec3(0, 0, 0);
vec3 nextCameraPosX = vec3(100, 0, 0);
vec3 nextCameraPosZ = vec3(100, 0, 0);
vec3 viewDirection = vec3(0, 0, -1);
vec3 goalPos = vec3(0, 0, 0);

int MazeSize;
char maze[255][255] = { 0 };

float cameraSpeed = 0.08;

float g_time = 0;

float initTheta = M_PI;
float theta = initTheta;
float rotateSpeed = M_PI / 45;
int rotateDirection;

std::list<vec2> findPath;
std::list<vec2>::iterator pathIter;
bool hintButton = false;
bool followButton = false;

inline vec3 getPositionFromIndex(int i, int j)
{
	float unit = 1;
	vec3 leftTopPosition = vec3(-MazeSize / 2.0 + unit / 2, 0, -MazeSize / 2.0 + unit / 2);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	return leftTopPosition + i * xDir + j * zDir;
}

// aStar 노드 구조체
struct Node
{
	Node* parent;
	// 노드 좌표
	vec2 pos;
	// f = g + h 이고 f값을 최소로하는 노드를 우선으로 함
	float f;	// 출발 지점에서 목적지까지의 총 cost
	float g;	// 현재 노드에서 출발 지점까지의 총 cost
	float h;	// 현재 노드에서 목적지까지의 추정 거리
};

struct cmp
{
	bool operator()(const Node* a, const Node* b) {
		return a->f > b->f;
	}
};

Node* newNode(Node* parent, vec2 input_pos, vec2 end, float input_g)
{
	Node* node = (Node*)malloc(sizeof(Node));
	node->parent = parent;
	node->pos = input_pos;
	node->g = input_g;
	node->h = sqrt(pow((node->pos).x - end.x, 2) + pow((node->pos).y - end.y, 2));
	node->f = node->g + node->h;
	return node;
}

bool existOnTheClosedList(std::list<Node*> closedList, int x, int y)
{
	for (std::list<Node*>::iterator itr = closedList.begin(); itr != closedList.end(); itr++)
	{
		if ((*itr)->pos.x == x && (*itr)->pos.y == y)
		{
			return true;
		}
	}
	return false;
}

void aStar(vec2 start, vec2 end)
{
	std::priority_queue<Node*, std::vector<Node*>, cmp> openQueue;
	std::list<Node*> closedList;

	Node* currentNode;
	Node* startNode = newNode(NULL, start, end, 0);
	openQueue.push(startNode);
	while (!openQueue.empty())
	{
		// 현재 노드 불러오기 & openQueue에서 삭제
		currentNode = openQueue.top();
		openQueue.pop();

		// 현재 노드가 목표 노드면 종료
		if (currentNode->pos.x == end.x && currentNode->pos.y == end.y)
		{
			// 리스트를 비우고 새로 저장
			findPath.clear();

			// findPath 리스트에 경로 저장
			Node* temp = currentNode;
			while (temp != NULL)	// 부모노드를 통해 역추적
			{
				findPath.push_front(temp->pos);
				temp = temp->parent;
			}
			break;
		}

		// 현재 노드의 인접 좌표 탐색 및 openQueue에 추가, 상하좌우만 파악 & 해당 노드는 현재 노드를 부모로 함
		int x = currentNode->pos.x;
		int y = currentNode->pos.y;
		if (!existOnTheClosedList(closedList, x, y - 1) && maze[y - 1][x] != '*')	// up
		{
			openQueue.push(newNode(currentNode, vec2(x, y - 1), end, currentNode->g + 1));
		}
		if (!existOnTheClosedList(closedList, x, y + 1) && maze[y + 1][x] != '*')	// down
		{
			openQueue.push(newNode(currentNode, vec2(x, y + 1), end, currentNode->g + 1));
		}
		if (!existOnTheClosedList(closedList, x - 1, y) && maze[y][x - 1] != '*')	//left
		{
			openQueue.push(newNode(currentNode, vec2(x - 1, y), end, currentNode->g + 1));
		}
		if (!existOnTheClosedList(closedList, x + 1, y) && maze[y][x + 1] != '*')	//right
		{
			openQueue.push(newNode(currentNode, vec2(x + 1, y), end, currentNode->g + 1));
		}

		// 현재 노드는 closedList로 이동
		closedList.push_back(currentNode);
	}
}

bool isCollision(vec3 pos) {
	vec3 center;
	vec3 leftup;
	vec3 rightdown;
	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++)
			if (maze[i][j] == '*')
			{
				center = vec3(getPositionFromIndex(i, j).x, 0, getPositionFromIndex(i, j).z);
				leftup = vec3(center.x - 0.75, 0, center.z - 0.75);
				rightdown = vec3(center.x + 0.75, 0, center.z + 0.75);
				if ((pos.x >= leftup.x && pos.x <= rightdown.x) && (pos.z >= leftup.z && pos.z <= rightdown.z))
				{
					return true;
				}
			}
	return false;
}

void LoadMaze()
{
	FILE* file = fopen(MAZE_FILE, "r");
	char buf[255];
	fgets(buf, 255, file);
	sscanf(buf, "%d", &MazeSize);
	for (int j = 0; j < MazeSize; j++)
	{
		fgets(buf, 255, file);
		for (int i = 0; i < MazeSize; i++)
		{
			maze[i][j] = buf[i];
			if (maze[i][j] == 'C')				// Setup Camera Position
				cameraPos = getPositionFromIndex(i, j);
			if (maze[i][j] == 'G')				// Setup Goal Position
				goalPos = getPositionFromIndex(i, j);
		}
	}
	fclose(file);
}

void DrawMaze()
{
	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++)
			if (maze[i][j] == '*')
			{
				vec3 color = vec3(i / (float)MazeSize, j / (float)MazeSize, 1);
				mat4 ModelMat = Translate(getPositionFromIndex(i, j));
				vec3 center = center = vec3(getPositionFromIndex(i, j).x, 0, getPositionFromIndex(i, j).z);
				vec3 leftup = vec3(center.x - 0.75, 0, center.z - 0.75);
				vec3 rightdown = vec3(center.x + 0.75, 0, center.z + 0.75);
				if ((nextCameraPosX.x >= leftup.x && nextCameraPosX.x <= rightdown.x) && (nextCameraPosX.z >= leftup.z && nextCameraPosX.z <= rightdown.z))
				{
					color = vec3(255, 0, 0);
				}
				if ((nextCameraPosZ.x >= leftup.x && nextCameraPosZ.x <= rightdown.x) && (nextCameraPosZ.z >= leftup.z && nextCameraPosZ.z <= rightdown.z))
				{
					color = vec3(255, 0, 0);
				}
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				cube.Draw(program);
			}
	if (hintButton)
	{
		vec3 prePos;
		vec3 nextPos;
		float w;
		float h;
		float minimun;
		vec3 directionVector;
		for (std::list<vec2>::iterator iter = findPath.begin(); iter != findPath.end(); iter++)
		{
			prePos = getPositionFromIndex(iter->y, iter->x);
			iter++;
			if (iter == findPath.end())
				break;
			else
				nextPos = getPositionFromIndex(iter->y, iter->x);
			iter--;
			w = abs(prePos.x - nextPos.x);
			h = abs(prePos.z - nextPos.z);
			if (w == 0)
				w = 0.1;
			else
				h = 0.1;
			mat4 ModelMat = Translate((prePos + nextPos) / 2 + vec3(0, -0.5, 0)) * Scale(w, 0.05, h);
			vec3 color = vec3(255, 0, 0);
			glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
			glUniform4f(uColor, color.x, color.y, color.z, 1);
			cube.Draw(program);
		}
		if (followButton)
		{
			pathIter++;
			nextPos = getPositionFromIndex(pathIter->y, pathIter->x);
			pathIter--;
			prePos = getPositionFromIndex(pathIter->y, pathIter->x);
			// viewDirection 조정
			directionVector = (nextPos - prePos);
			if (directionVector.x != 0)
				directionVector.x /= abs(directionVector.x);
			else
				directionVector.z /= abs(directionVector.z);
			if (rotateDirection == LEFT_ROTATE && (abs(sin(theta) - directionVector.x) > 0.001 || abs(cos(theta) - directionVector.z) > 0.001))
			{
				theta += rotateSpeed;
				viewDirection = vec3(sin(theta), 0, cos(theta));
				if ((abs(sin(theta) - directionVector.x) < 0.001 || abs(cos(theta) - directionVector.z) < 0.001))
					rotateDirection = STOP_ROTATE;
			}
			else if (rotateDirection == RIGHT_ROTATE && (abs(sin(theta) - directionVector.x) > 0.001 || abs(cos(theta) - directionVector.z) > 0.001))
			{
				theta -= rotateSpeed;
				viewDirection = vec3(sin(theta), 0, cos(theta));
				if ((abs(sin(theta) - directionVector.x) < 0.001 || abs(cos(theta) - directionVector.z) < 0.001))
					rotateDirection = STOP_ROTATE;
			}
			else
			{
				// 최단 경로를 따라 이동
				float moveSpeed = 0.003 * MazeSize;
				cameraPos.x += (nextPos.x - prePos.x) > 0 ? moveSpeed : ((nextPos.x - prePos.x) != 0 ? -moveSpeed : 0);
				cameraPos.z += (nextPos.z - prePos.z) > 0 ? moveSpeed : ((nextPos.z - prePos.z) != 0 ? -moveSpeed : 0);
				// 모서리에 도착
				if ((abs(cameraPos.x - nextPos.x)) < moveSpeed && (abs(cameraPos.z - nextPos.z)) < moveSpeed)
				{
					cameraPos.x = nextPos.x;
					cameraPos.z = nextPos.z;
					prePos = nextPos;
					pathIter++;
					pathIter++;
					if (pathIter == findPath.end())
					{
						followButton = false;
					}
					else
					{
						nextPos = getPositionFromIndex(pathIter->y, pathIter->x);
						pathIter--;
						directionVector = (nextPos - prePos);
						if (directionVector.x != 0)
							directionVector.x /= abs(directionVector.x);
						else
							directionVector.z /= abs(directionVector.z);
						// 다음 진행 방향과 거의 일치할 때까지 왼쪽으로 회전
						// 더 적게 움직이는 쪽으로 회전 방향을 결정
						for (int i = 0;; i++)
						{
							if (abs(sin(theta + (rotateSpeed * i)) - directionVector.x) < 0.01 || abs(cos(theta + (rotateSpeed * i)) - directionVector.z) < 0.01)
							{
								rotateDirection = LEFT_ROTATE;
								minimun = i;
								break;
							}
						}
						// 다음 진행 방향과 거의 일치할 때까지 오른쪽으로 회전
						// 더 적게 움직이는 쪽으로 회전 방향을 결정
						for (int i = 0;; i++)
						{
							if (abs(sin(theta - (rotateSpeed * i)) - directionVector.x) < 0.01 || abs(cos(theta - (rotateSpeed * i)) - directionVector.z) < 0.01)
							{
								if (minimun > i)
									rotateDirection = RIGHT_ROTATE;
								break;
							}
						}
					}
				}
			}
		}
	}
}

void myInit()
{
	printf("A Maze Navigator\nProgramming Assignment #3 for Computer Graphics. Department of Software, Sejong University\n");
	printf("\n----------------------------------------------------------------\n");
	printf("'W' key: Go Forward\n'S' key: Go Backward\n'A' key: Turn Left\n'D' key: Turn Right\n'Q' key: Find Shortese Path to the Goal\n'Spacebar' : Start following the Path\n");
	printf("\n----------------------------------------------------------------\n");
	LoadMaze();
	cube.Init();
	program = InitShader("vshader.glsl", "fshader.glsl");

}

void DrawGrid()
{
	float n = 40;
	float w = MazeSize;
	float h = MazeSize;

	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h / 2 + h / n * i) * Scale(w, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w / 2 + w / n * i, -0.5, 0) * Scale(0.02, 0.02, h);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}


void drawCamera()
{
	float cameraSize = 0.5;

	mat4 ModelMat = Translate(cameraPos) * RotateY((theta - initTheta) * (180 / M_PI)) * Scale(vec3(cameraSize));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	ModelMat = Translate(cameraPos + viewDirection * cameraSize / 2) * RotateY((theta - initTheta) * (180 / M_PI)) * Scale(vec3(cameraSize / 2));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);
}

void drawGoal()
{
	glUseProgram(program);
	float GoalSize = 0.7;

	mat4 ModelMat = Translate(goalPos) * RotateY(g_time * 3) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);

	ModelMat = Translate(goalPos) * RotateY(g_time * 3 + 45) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);
}


void drawScene(bool bDrawCamera = true)
{
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawGrid();
	DrawMaze();
	drawGoal();

	if (bDrawCamera) {
		drawCamera();
		nextCameraPosX = cameraPos;
		nextCameraPosZ = cameraPos;
	}
}

void display()
{
	glEnable(GL_DEPTH_TEST);

	float vWidth = wWidth / 2;
	float vHeight = wHeight;

	// LEFT SCREEN : View From Camera (Perspective Projection)
	glViewport(0, 0, vWidth, vHeight);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float h = 4;
	float aspectRatio = vWidth / vHeight;
	float w = aspectRatio * h;

	vec3 eyePoint = cameraPos;
	vec3 focusPoint = cameraPos + viewDirection;
	vec3 upVector = vec3(0, 1, 0);

	mat4 ViewMat = myLookAt(eyePoint, focusPoint, upVector);
	mat4 ProjMat = myPerspective(45, aspectRatio, 0.01, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(false);							// drawing scene except the camera

	// RIGHT SCREEN : View from above (Orthographic parallel projection)
	glViewport(vWidth, 0, vWidth, vHeight);
	h = MazeSize;
	w = aspectRatio * h;
	ViewMat = myLookAt(vec3(0, 5, 0), vec3(0, 0, 0), vec3(0, 0, -1));
	ProjMat = myOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(true);

	glutSwapBuffers();
}

void idle()
{
	g_time += 1;
	if ((GetAsyncKeyState('A') & 0x8000) == 0x8000 && !followButton)		// if "A" key is pressed	: Go Left
	{
		theta += rotateSpeed;
		viewDirection = vec3(sin(theta), 0, cos(theta));
	}
	if ((GetAsyncKeyState('D') & 0x8000) == 0x8000 && !followButton) 	// if "D" key is pressed	: Go Right
	{
		theta -= rotateSpeed;
		viewDirection = vec3(sin(theta), 0, cos(theta));
	}
	if ((GetAsyncKeyState('W') & 0x8000) == 0x8000 && !followButton)		// if "W" key is pressed	: Go Forward
	{
		// X축 방향 이동과 Z축 방향 이동을 각각 따로 충돌검사 후 순차적 이동
			//Z 방향으로 진행시 충돌하는지 검사 후 충돌하지 않으면 Z값만큼 이동
		nextCameraPosZ = cameraPos + cameraSpeed * vec3(0, 0, viewDirection.z);
		if (!isCollision(nextCameraPosZ))
		{
			cameraPos = nextCameraPosZ;
		}
		//X 방향으로 진행시 충돌하는지 검사 후 충돌하지 않으면 X값만큼 이동
		nextCameraPosX = cameraPos + cameraSpeed * vec3(viewDirection.x, 0, 0);
		if (!isCollision(nextCameraPosX))
		{
			cameraPos = nextCameraPosX;
		}
	}
	if ((GetAsyncKeyState('S') & 0x8000) == 0x8000 && !followButton)		// if "S" key is pressed	: Go Backward
	{
		// X축 방향 이동과 Z축 방향 이동을 각각 따로 충돌검사 후 순차적 이동
			//Z 방향으로 진행시 충돌하는지 검사 후 충돌하지 않으면 Z값만큼 이동
		nextCameraPosZ = cameraPos - cameraSpeed * vec3(0, 0, viewDirection.z);
		if (!isCollision(nextCameraPosZ))
		{
			cameraPos = nextCameraPosZ;
		}
		//X 방향으로 진행시 충돌하는지 검사 후 충돌하지 않으면 X값만큼 이동
		nextCameraPosX = cameraPos - cameraSpeed * vec3(viewDirection.x, 0, 0);
		if (!isCollision(nextCameraPosX))
		{
			cameraPos = nextCameraPosX;
		}
	}

	Sleep(16);											// for vSync
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	std::list<vec2> tempList;
	vec2 prePoint = NULL;
	vec2 nextPoint = NULL;
	vec2 startPoint = vec2(0, 0);
	vec2 endPoint = vec2(0, 0);
	vec3 tempPos;
	float minimum;
	switch (key)
	{
	case 'q':
		// 가장 가까운 경로 탐색 시작 포인트 설정
		minimum = 10000;
		for (int j = 0; j < MazeSize; j++)
		{
			for (int i = 0; i < MazeSize; i++)
			{
				if (maze[i][j] != '*')
				{
					tempPos = getPositionFromIndex(i, j);
					if (minimum > (abs(cameraPos.x - tempPos.x) + abs(cameraPos.z - tempPos.z)))
					{
						startPoint = vec2(j, i);
						minimum = (abs(cameraPos.x - tempPos.x) + abs(cameraPos.z - tempPos.z));
					}
				}
				if (maze[i][j] == 'G')
					endPoint = vec2(j, i);
			}
		}

		// A* 알고리즘 경로탐색
		aStar(startPoint, endPoint);

		// 경로의 모서리 좌표를 제외한 나머지 좌표는 리스트에서 제거
		for (std::list<vec2>::iterator iter = findPath.begin(); iter != findPath.end(); iter++)
		{
			if (prePoint == NULL)
			{
				prePoint = vec2(iter->x, iter->y);
				continue;
			}
			iter++;
			if (iter != findPath.end())
				nextPoint = vec2(iter->x, iter->y);
			else
				nextPoint = vec2(0, 0);
			iter--;
			if (!(prePoint.x == iter->x && iter->x == nextPoint.x) && !(prePoint.y == iter->y && iter->y == nextPoint.y))
				tempList.push_back(vec2(iter->x, iter->y));
			prePoint = vec2(iter->x, iter->y);
		}
		findPath.clear();
		findPath = tempList;
		if (!hintButton)
			hintButton = true;
		else
			if ((getPositionFromIndex(startPoint.y, startPoint.x).x == goalPos.x) && (getPositionFromIndex(startPoint.y, startPoint.x).z == goalPos.z))
				hintButton = false;
		if (followButton)
		{
			pathIter = findPath.begin();
			cameraPos = getPositionFromIndex(pathIter->y, pathIter->x);
		}
		break;
	case ' ':
		if (hintButton)
		{
			if (followButton)
			{
				followButton = false;
			}
			else
			{
				followButton = true;
				pathIter = findPath.begin();
				cameraPos = getPositionFromIndex(pathIter->y, pathIter->x);
				vec3 prePos = getPositionFromIndex(pathIter->y, pathIter->x);
				pathIter++;
				vec3 nextPos = getPositionFromIndex(pathIter->y, pathIter->x);
				pathIter--;
				if ((nextPos.x - prePos.x) / abs(nextPos.x - prePos.x) == 1)
					theta = M_PI / 2;
				else if ((nextPos.x - prePos.x) / abs(nextPos.x - prePos.x) == -1)
					theta = -M_PI / 2;
				else if ((nextPos.z - prePos.z) / abs(nextPos.z - prePos.z) == 1)
					theta = 0;
				else if ((nextPos.z - prePos.z) / abs(nextPos.z - prePos.z) == -1)
					theta = M_PI;
				viewDirection= vec3(sin(theta), 0, cos(theta));
			}
		}
		break;
	}
}
void reshape(int wx, int wy)
{
	wWidth = wx;
	wHeight = wy;
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("Homework3 (Maze Navigator)");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(display);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}