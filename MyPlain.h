#ifndef _MY_PLAIN_H_
#define _MY_PLAIN_H_

#include <vgl.h>
#include <vec.h>

struct MyPlainVertex
{
	vec4 position;
	vec4 color;
};

class MyPlain
{
public:
	int division = 30;
	int NUM_TRIANGLES = division * division * 2;
	int NUM_POINTS = NUM_TRIANGLES * 3; //초기값 30x30 체커보드
	MyPlainVertex* vertices = NULL;
	GLuint vao;
	GLuint vbo;

	void init();
	void setPlain();
	void draw(GLuint prog);
	void setAttributePointers(GLuint prog);
	void reshape();
};

void MyPlain::setPlain()
{
	int index = 0;
	float step = 1.6 / division;
	vec4 currentPos = vec4(-0.8f, 0.8f, 0.0f, 1.0f);
	int color = 0;

	vec4 vertex_color[2] = {
		vec4(0.55f,0.55f,0.55f,1.0f),
		vec4(0.65f,0.65f,0.65f,1.0f)
	};
	for (int i = 0; i < division; i++)
	{
		for (int j = 0; j < division; j++)
		{
			if (currentPos.x == -0.8f && division % 2 == 0)
			{
				color++;
				color %= 2;
			}

			vertices[index].position = currentPos; vertices[index].color = vertex_color[color]; index++;
			vertices[index].position = currentPos + vec4(step, 0.0f, 0.0f, 0.0f); vertices[index].color = vertex_color[color]; index++;
			vertices[index].position = currentPos + vec4(0.0f, -step, 0.0f, 0.0f); vertices[index].color = vertex_color[color]; index++;
			vertices[index].position = currentPos + vec4(step, 0.0f, 0.0f, 0.0f); vertices[index].color = vertex_color[color]; index++;
			vertices[index].position = currentPos + vec4(step, -step, 0.0f, 0.0f); vertices[index].color = vertex_color[color]; index++;
			vertices[index].position = currentPos + vec4(0.0f, -step, 0.0f, 0.0f); vertices[index].color = vertex_color[color]; index++;

			if (color == 0)
			{
				color = 1;
			}
			else
			{
				color = 0;
			}
			currentPos.x += step;
		}
		currentPos.x = -0.8f;
		currentPos.y -= step;
	}
}

void MyPlain::init()
{
	printf("A Waving Color Plain\nProgramming Assignment #1 for Computer Graphics\n");
	printf("Department of Software, Sejong University\n");
	printf("----------------------------------------------------------------\n");
	printf("'1' key: Decreaing the Number of Division\n");
	printf("'2' key: Increaing the Number of Division\n");
	printf("'w' key: Showing/hiding the waving pattern\n");
	printf("Spacebar: starting/stoping rotating and waving\n\n");
	printf("'Q' key: Exit the program.\n");
	printf("----------------------------------------------------------------\n\n");
	printf("Division: %d, Num.of Triangles: %d, Num. of Vertices: %d\n", division, NUM_TRIANGLES, NUM_POINTS);
	vertices = new MyPlainVertex[NUM_POINTS];
	setPlain();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyPlainVertex) * NUM_POINTS, vertices, GL_STATIC_DRAW);

	delete[] vertices;
}

void MyPlain::setAttributePointers(GLuint prog)
{
	GLuint vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyPlainVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(prog, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyPlainVertex), BUFFER_OFFSET(sizeof(vec4)));
}

void MyPlain::draw(GLuint prog)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glUseProgram(prog);
	setAttributePointers(prog);

	glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS);
}

void MyPlain::reshape()
{
	vertices = new MyPlainVertex[NUM_POINTS];
	setPlain();
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyPlainVertex) * NUM_POINTS, vertices, GL_STATIC_DRAW);
	delete[] vertices;
}

#endif
