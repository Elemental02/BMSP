#include "../stdafx.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../managers/ResourceManager.h"
#include "gfxSprite.h"

#include "gfxGlobal.h"

gfx::gfxSprite::gfxSprite()
{
}

void gfx::gfxSprite::setSprite(std::shared_ptr<Sprite> sprite)
{
	this->sprite = sprite;
}

gfx::gfxSprite::~gfxSprite()
{
}

void gfx::gfxSprite::Render()
{
	auto transform = getTransform();

	GLuint vertexbuffer = gfxGlobal::Instance()->getGlobalVertexArrayBuffer();
	GLuint colorbuffer = gfxGlobal::Instance()->getGlobalColorArrayBuffer();
	GLuint uvbuffer = gfxGlobal::Instance()->getGlobalUVArrayBuffer();
	GLuint transformBuffer = gfxGlobal::Instance()->getGlobalMatrixArrayBuffer();
	GLuint samplerId = gfxGlobal::Instance()->getGlobalTextureSamplerId();
	
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	//glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(sprite->uv), static_cast<void*>(sprite->uv.data()), GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite->textureId);
	glUniform1i(samplerId, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sprite->vertex), static_cast<void*>(sprite->vertex.data()), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->color), static_cast<void*>(&this->color), GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sprite->uv), static_cast<void*>(sprite->uv.data()), GL_STATIC_DRAW);
	glVertexAttribPointer(
		2,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	int pos = 3;
	int pos1 = pos + 0;
	int pos2 = pos + 1;
	int pos3 = pos + 2;
	int pos4 = pos + 3;
	glEnableVertexAttribArray(pos1);
	glEnableVertexAttribArray(pos2);
	glEnableVertexAttribArray(pos3);
	glEnableVertexAttribArray(pos4);
	glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transform), static_cast<void*>(&transform), GL_STATIC_DRAW);
	glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(pos1, 1);
	glVertexAttribDivisor(pos2, 1);
	glVertexAttribDivisor(pos3, 1);
	glVertexAttribDivisor(pos4, 1);

	//glVertexAttribDivisor(3, 5);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, sprite->vertex.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}
