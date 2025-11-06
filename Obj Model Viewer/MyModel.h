#pragma once

#include <vgl.h>
#include <vec.h>

struct MyModelVertex
{
	vec4 position;
	vec4 color;
	vec3 vertexNormal;
	vec3 surfaceNormal;
};

class MyModel
{
public:
	MyModel(void);
	~MyModel(void);

	static int NumVertices;

	MyModelVertex* Vertices;
	GLuint Init(char* file);
	void SetPositionAndColorAttribute(GLuint program);


	void ColorModel(vec4* vin, vec4 cin);
	void Triangle(int a, int b, int c, vec4* vin, vec4 cin);
	vec3 getNormal(vec4 a, vec4 b, vec4 c);

	int NumCurVertices;

	GLuint vao;
	GLuint buffer;
	bool bInitialized;

	FILE* fp;
	vec3* calcVertexNormal;
	vec4* vertex;
	vec4* face;
	vec4 vertex_color = vec4(0.7, 0.7, 0.7, 1);

	int vnum = 0, fnum = 0;
	float avgX, avgY, avgZ, sumX = 0, sumY = 0, sumZ = 0;
	float maxX = NULL, minX = NULL, maxY = NULL, minY = NULL, maxZ = NULL, minZ = NULL;
	float scaleX, scaleY, scaleZ, totalScale;

	void Draw(GLuint program);
};

MyModel::MyModel(void)
{
	bInitialized = false;
	NumCurVertices = 0;
}

MyModel::~MyModel(void)
{
}

vec3 MyModel::getNormal(vec4 a, vec4 b, vec4 c)
{
	vec3 p0(a.x, a.y, a.z);
	vec3 p1(b.x, b.y, b.z);
	vec3 p2(c.x, c.y, c.z);

	vec3 p = p1 - p0;
	vec3 q = p2 - p0;
	vec3 n = cross(p, q);
	n = normalize(n);
	return n;
}

void MyModel::Triangle(int a, int b, int c, vec4* vin, vec4 cin)
{
	vec3 n = getNormal(vin[a - 1] + vec4(-avgX, -avgY, -avgZ, 0), vin[b - 1] + vec4(-avgX, -avgY, -avgZ, 0), vin[c - 1] + vec4(-avgX, -avgY, -avgZ, 0));
	Vertices[NumCurVertices].position = vin[a - 1] + vec4(-avgX, -avgY, -avgZ, 0); Vertices[NumCurVertices].color = cin;
	Vertices[NumCurVertices].vertexNormal = normalize(calcVertexNormal[a - 1]);
	Vertices[NumCurVertices].surfaceNormal = n; NumCurVertices++;

	Vertices[NumCurVertices].position = vin[b - 1] + vec4(-avgX, -avgY, -avgZ, 0); Vertices[NumCurVertices].color = cin;
	Vertices[NumCurVertices].vertexNormal = normalize(calcVertexNormal[b - 1]);
	Vertices[NumCurVertices].surfaceNormal = n; NumCurVertices++;

	Vertices[NumCurVertices].position = vin[c - 1] + vec4(-avgX, -avgY, -avgZ, 0); Vertices[NumCurVertices].color = cin;
	Vertices[NumCurVertices].vertexNormal = normalize(calcVertexNormal[c - 1]);
	Vertices[NumCurVertices].surfaceNormal = n; NumCurVertices++;
}

void MyModel::ColorModel(vec4* vin, vec4 cin)
{
	vec3 n;
	int a, b, c;
	for (int i = 0; i < vnum; i++) {
		calcVertexNormal[i] = vec3(0, 0, 0);
	}
	for (int i = 0; i < fnum; i++) {
		a = (int)face[i].x;
		b = (int)face[i].y;
		c = (int)face[i].z;
		n = getNormal(vin[a - 1] + vec4(-avgX, -avgY, -avgZ, 0), vin[b - 1] + vec4(-avgX, -avgY, -avgZ, 0), vin[c - 1] + vec4(-avgX, -avgY, -avgZ, 0));
		calcVertexNormal[a - 1] += n;
		calcVertexNormal[b - 1] += n;
		calcVertexNormal[c - 1] += n;
	}
	for (int i = 0; i < fnum; i++) {
		a = (int)face[i].x;
		b = (int)face[i].y;
		c = (int)face[i].z;
		Triangle(a, b, c, vin, cin);
	}
}


GLuint MyModel::Init(char* file)
{
	// The Cube should be initialized only once;
	if (bInitialized == true) return vao;

	const int max = 100;
	char line[max];

	// v와 f 개수 카운트
	fp = fopen(file, "r");
	while (!feof(fp)) {
		fgets(line, max, fp);
		if (line[0] == 'v') {
			vnum++;
		}
		else if (line[0] == 'f') {
			fnum++;
		}
		else {
			continue;
		}
	}
	fclose(fp);

	// 동적 할당
	NumVertices += (fnum * 3);
	Vertices = new MyModelVertex[NumVertices];
	calcVertexNormal = (vec3*)malloc(sizeof(vec3) * vnum);
	vertex = (vec4*)malloc(sizeof(vec4) * vnum);
	face = (vec4*)malloc(sizeof(vec4) * fnum);

	// model 정보 저장
	fp = fopen(file, "rb");
	int vertIndex = 0, faceIndex = 0;
	while (!feof(fp)) {
		fscanf(fp, "%s", line);
		if (line[0] == 'v' && line[1] == '\0') {
			fscanf(fp, "%f", &vertex[vertIndex].x);
			fscanf(fp, "%f", &vertex[vertIndex].y);
			fscanf(fp, "%f", &vertex[vertIndex].z);
			vertex[vertIndex].w = 1.0f;
			if (vertex[vertIndex].x > maxX || vertIndex == 0)
				maxX = vertex[vertIndex].x;
			if (vertex[vertIndex].y > maxY || vertIndex == 0)
				maxY = vertex[vertIndex].y;
			if (vertex[vertIndex].z > maxZ || vertIndex == 0)
				maxZ = vertex[vertIndex].z;
			if (vertex[vertIndex].x < minX || vertIndex == 0)
				minX = vertex[vertIndex].x;
			if (vertex[vertIndex].y < minY || vertIndex == 0)
				minY = vertex[vertIndex].y;
			if (vertex[vertIndex].z < minZ || vertIndex == 0)
				minZ = vertex[vertIndex].z;
			sumX += vertex[vertIndex].x;
			sumY += vertex[vertIndex].y;
			sumZ += vertex[vertIndex].z;
			vertIndex++;
		}
		else if (line[0] == 'f' && line[1] == '\0') {
			fscanf(fp, "%f", &face[faceIndex].x);
			fscanf(fp, "%f", &face[faceIndex].y);
			fscanf(fp, "%f", &face[faceIndex].z);
			face[faceIndex].w = 1.0f;
			faceIndex++;
		}
		fgets(line, max, fp);
	}
	fclose(fp);
	vnum = vertIndex;
	fnum = faceIndex;
	avgX = sumX / (float)vertIndex;
	avgY = sumY / (float)vertIndex;
	avgZ = sumZ / (float)vertIndex;

	scaleX = 1.7 / abs(maxX - minX);
	scaleY = 1.7 / abs(maxY - minY);
	scaleZ = 1.7 / abs(maxZ - minZ);
	if (scaleX > scaleY) {
		if (scaleY > scaleZ)
			totalScale = scaleZ;
		else
			totalScale = scaleY;
	}
	else if (scaleX < scaleY) {
		if (scaleX < scaleZ)
			totalScale = scaleX;
		else
			totalScale = scaleZ;
	}
	else
		totalScale = scaleX;
	totalScale *= 1.5;
	vec4 temp = totalScale * vertex[0];

	ColorModel(vertex, vertex_color);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyModelVertex) * NumVertices, Vertices, GL_STATIC_DRAW);

	bInitialized = true;
	return vao;
}

void MyModel::SetPositionAndColorAttribute(GLuint program)
{
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyModelVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyModelVertex), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(MyModelVertex), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4)));

	GLuint sNormal = glGetAttribLocation(program, "sNormal");
	glEnableVertexAttribArray(sNormal);
	glVertexAttribPointer(sNormal, 3, GL_FLOAT, GL_FALSE, sizeof(MyModelVertex), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4) + sizeof(vec3)));
}


void MyModel::Draw(GLuint program)
{
	if (!bInitialized) return;			// check whether it is initiazed or not. 

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndColorAttribute(program);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}