#include "../stdafx.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../managers/ResourceManager.h"
#include "gfxSprite.h"

#include "../managers/GlobalManager.h"

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
	if (sprite == nullptr)
		return;
	IgfxGlobal->UseShaders(shader);
	//auto transform = getTransform();

	GLuint vertexbuffer = IgfxGlobal->getGlobalVertexArrayBuffer();
	GLuint colorbuffer = IgfxGlobal->getGlobalColorArrayBuffer();
	GLuint uvbuffer = IgfxGlobal->getGlobalUVArrayBuffer();
	GLuint transformBuffer = IgfxGlobal->getGlobalMatrixArrayBuffer();
	GLuint sizebuffer = IgfxGlobal->getGlobalSizeArrayBuffer();
	GLuint texturerectbuffer = IgfxGlobal->getGlobalTexPosArrayBuffer();
	GLuint samplerId = IgfxGlobal->getGlobalTextureSamplerId();

	IgfxGlobal->setUniformMatrix(glm::mat4());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite->texture_id);
	glUniform1i(samplerId, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->getColor()), static_cast<void*>(&this->getColor()));
	glVertexAttribPointer(
		2,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, sizebuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite->size), static_cast<void*>(&sprite->size));
	glVertexAttribPointer(
		3,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, texturerectbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite->texture_rect), static_cast<void*>(&sprite->texture_rect));
	glVertexAttribPointer(
		4,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(4, 1);

	int pos = 5;
	int pos1 = pos + 0;
	int pos2 = pos + 1;
	int pos3 = pos + 2;
	int pos4 = pos + 3;
	glEnableVertexAttribArray(pos1);
	glEnableVertexAttribArray(pos2);
	glEnableVertexAttribArray(pos3);
	//glEnableVertexAttribArray(pos4);
	glm::vec3 posscale[3] = { getPosition(), getScale(), getPivot() };
	glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(posscale), static_cast<void*>(&posscale));
	glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(0));
	glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(sizeof(float) * 6));
	//glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(pos1, 1);
	glVertexAttribDivisor(pos2, 1);
	glVertexAttribDivisor(pos3, 1);
	//glVertexAttribDivisor(pos4, 1);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(pos1);
	glDisableVertexAttribArray(pos2);
	glDisableVertexAttribArray(pos3);
	//glDisableVertexAttribArray(pos4);
}
