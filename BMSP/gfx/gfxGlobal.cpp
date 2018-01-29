#include "../stdafx.h"
#include <GL/glew.h>

#include "gfxGlobal.h"

gfxGlobal::~gfxGlobal()
{
}

void gfxGlobal::initGlobalArrayBuffer(GLuint programID)
{
	glGenBuffers(1, &VertexArrayBuffer);
	glGenBuffers(1, &UVArrayBuffer);
	glGenBuffers(1, &ColorArrayBuffer);
	glGenBuffers(1, &MatrixArrayBuffer);

	glGetUniformLocation(programID, "myTextureSampler");
}

GLuint gfxGlobal::getGlobalVertexArrayBuffer()
{
	return VertexArrayBuffer;
}

GLuint gfxGlobal::getGlobalUVArrayBuffer()
{
	return UVArrayBuffer;
}

GLuint gfxGlobal::getGlobalColorArrayBuffer()
{
	return ColorArrayBuffer;
}

GLuint gfxGlobal::getGlobalMatrixArrayBuffer()
{
	return MatrixArrayBuffer;
}

GLuint gfxGlobal::getGlobalTextureSamplerId()
{
	return TextureSamplerId;
}
