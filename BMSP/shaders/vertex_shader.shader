#version 430 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec2 vertexSize;
layout(location = 4) in vec4 rectUV;
//layout(location = 5) in mat4 vertexMatrix;
layout(location = 5) in vec3 objPosition;
layout(location = 6) in vec3 objScale;
layout(location = 7) in vec3 objPivot;
out vec4 fragmentColor;
out vec2 texCoord;
uniform mat4 GlobalUniform;
void main()
{
	vec4 pos= vec4(vertexPosition, 1);
	pos.x = ((pos.x-objPivot.x)*vertexSize.x)*objScale.x + objPosition.x;
	pos.y = ((pos.y-objPivot.y)*vertexSize.y)*objScale.y + objPosition.y;
	//pos.z = (pos.z*vertexSize.z + objPosition.y)*objScale.z;
	texCoord.x = vertexUV.x*rectUV.z + rectUV.x;
	texCoord.y = vertexUV.y*rectUV.w + rectUV.y;
	gl_Position = GlobalUniform * pos;
	fragmentColor = vertexColor;
}