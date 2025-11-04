#version 330

in vec4 vPosition;
in vec4 vColor;

uniform float uTime;
uniform bool uShaderButton;

out vec4 color;
out vec4 position;

void main()
{
	vec4 pos = vPosition;
	if(uShaderButton)
	{
		float height = 0.8f;
		float distance = sqrt(pos.x*pos.x + pos.y*pos.y);	
		
		if(distance!=0)
			height -= (distance);
		if(height<0)
			height = 0;
		distance *=20.0f;
		float rad = (20.0f*uTime);
		if(pos.x != -0.8f && pos.y != -0.8f && pos.y != 0.8f && pos.y != -0.8f)
			pos.z = (0.5 * height * sin(rad-(distance)));
	}

	float angle1 = 45.0f;
	float rad1 = angle1 / 180.0f*3.141592f;

	float angle2 = uTime * 90.0f;
	float rad2 = angle2 / 180.0f*3.141592f;

	mat4 rotX = mat4(1.0f);
	mat4 rotZ = mat4(1.0f);

	rotX[1][1] = cos(rad1);	rotX[2][1] = -sin(rad1);
	rotX[1][2] = sin(rad1);	rotX[2][2] = cos(rad1);

	rotZ[0][0] = cos(rad2);	rotZ[1][0] = -sin(rad2);
	rotZ[0][1] = sin(rad2);	rotZ[1][1] = cos(rad2);

	vec4 wPos = rotX*rotZ*pos;

	gl_Position = wPos;
	color = vColor;
	pos.z = -pos.z;
	position = pos;
}