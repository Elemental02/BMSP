#version 430 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in mat4 vertexMatrix;
out vec4 fragmentColor;
out vec2 texCoord;
uniform mat4 MVP;
void main()
{
	gl_Position = MVP * vertexMatrix * vec4(vertexPosition, 1);
	fragmentColor = vertexColor;
	texCoord = vertexUV;
}