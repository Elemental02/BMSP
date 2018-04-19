#include "../stdafx.h"
#include "../managers/GlobalManager.h"
#include "gfxString.h"

void gfx::gfxString::RenderString()
{
	if (texture_id == 0)
	{
		glGenTextures(1, &texture_id);
	}
	font->setPixelSize(pixelSize);
	int strlen = str.length();
	
	glm::u8* buffer = new glm::u8[pixelSize * pixelSize * strlen];
	memset(buffer, 0, pixelSize * pixelSize * strlen);
	size.resize(strlen);
	uv_rect.resize(strlen);
	mats.resize(strlen);
	int pos = 0;
	for (int i = 0; i < strlen; i++)
	{
		auto glyph = font->LoadChar(str[i]);
		for (int j = 0; j < glyph->bitmap.rows; j++)
		{
			memcpy(buffer + (i*pixelSize + j * pixelSize*strlen), glyph->bitmap.buffer + glyph->bitmap.width*j, glyph->bitmap.width);
		}
		size[i] = glm::vec2(glyph->bitmap.width, glyph->bitmap.rows);

		float uv_left = i * pixelSize, uv_right = glyph->bitmap.width;
		uv_left /= pixelSize * strlen;
		uv_right /= pixelSize * strlen;
		uv_rect[i] = glm::vec4(uv_left, 0.0f, uv_right, glyph->bitmap.rows / (float)pixelSize);
		
		mats[i] = glm::translate(glm::vec3(pos+ glyph->bitmap_left, pixelSize-glyph->bitmap_top, 0.0f));
		pos += glyph->advance.x / 64.0f;
	}
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, strlen*pixelSize, pixelSize, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (!sizebuffer)
	{
		glGenBuffers(1, &sizebuffer);
	}
	if (!uvrectbuffer)
	{
		glGenBuffers(1, &uvrectbuffer);
	}
	if (!matrixbuffer)
	{
		glGenBuffers(1, &matrixbuffer);
	}

	if (buffersize < strlen)
	{
		buffersize = strlen;
		glBindBuffer(GL_ARRAY_BUFFER, sizebuffer);
		glBufferData(GL_ARRAY_BUFFER, buffersize * sizeof(glm::vec2), 0, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uvrectbuffer);
		glBufferData(GL_ARRAY_BUFFER, buffersize * sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, matrixbuffer);
		glBufferData(GL_ARRAY_BUFFER, buffersize * sizeof(glm::mat4), 0, GL_DYNAMIC_DRAW);
	}

	delete[] buffer;

	is_dirty = false;
}

gfx::gfxString::~gfxString()
{
	glDeleteBuffers(1, &sizebuffer);
	glDeleteBuffers(1, &matrixbuffer);
	glDeleteBuffers(1, &uvrectbuffer);
	
	glDeleteTextures(1, &texture_id);
}

void gfx::gfxString::setString(std::string str)
{
	is_dirty = true;
	this->str = str;
}

void gfx::gfxString::Render()
{
	if (str.length() == 0)
		return;
	if (is_dirty)
	{
		RenderString();
	}
	IgfxGlobal->UseShaders("font");

	GLuint vertexbuffer = IgfxGlobal->getGlobalVertexArrayBuffer();
	GLuint colorbuffer = IgfxGlobal->getGlobalColorArrayBuffer();
	GLuint uvbuffer = IgfxGlobal->getGlobalUVArrayBuffer();
	GLuint samplerId = IgfxGlobal->getGlobalTextureSamplerId();

	IgfxGlobal->setUniformMatrix(getTransform());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(samplerId, 0);

	glm::mat4 panelTransform = this->getTransform();


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
	//glVertexAttribDivisor(0, 0);

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
	//glVertexAttribDivisor(1, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4), static_cast<void*>(&color));
	glVertexAttribPointer(
		2,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(2, str.length());

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, sizebuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2)*size.size(), static_cast<void*>(size.data()));
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
	glBindBuffer(GL_ARRAY_BUFFER, uvrectbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*uv_rect.size(), static_cast<void*>(uv_rect.data()));
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
	glEnableVertexAttribArray(pos4);
	glBindBuffer(GL_ARRAY_BUFFER, matrixbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4)*mats.size(), static_cast<void*>(mats.data()));
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4)*mats.size(), static_cast<void*>(mats.data()), GL_STATIC_DRAW);
	glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(pos1, 1);
	glVertexAttribDivisor(pos2, 1);
	glVertexAttribDivisor(pos3, 1);
	glVertexAttribDivisor(pos4, 1);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, str.length());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}