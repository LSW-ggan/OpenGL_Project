#version 330

in vec4 color;
in vec4 position;

uniform bool uShaderButton;

out vec4 fColor;

void main()
{
	float z = 1 - (3*position.z);
	fColor = color;
	if(uShaderButton)
	{
		if(position.z > 0)
			fColor = vec4((1.0f * (1-z)) + (color.r * z), (0.4f * (1-z)) + (color.g * z), (0.1f * (1-z)) + (color.b * z), 1.0f);
		else if(position.z!=0)
			fColor = vec4(((1.0f *  (1-z)) + (color.r * z)), ((0.6f *  (1-z)) + (color.g * z)), ((0.0f * (1-z)) + (color.b * z)), 1.0f);
	}
}