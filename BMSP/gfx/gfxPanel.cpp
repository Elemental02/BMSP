#include "../stdafx.h"
#include "gfxSprite.h"
#include "../managers/ResourceManager.h"
#include "gfxPanel.h"

#include "../managers/GlobalManager.h"

gfx::gfxPanel::~gfxPanel()
{
	for (auto& group : renderlist)
	{
		glDeleteBuffers(1, &group.second.colorbuffer);
		glDeleteBuffers(1, &group.second.sizebuffer);
		glDeleteBuffers(1, &group.second.matrixbuffer);
		glDeleteBuffers(1, &group.second.uvbuffer);
	}
}

void gfx::gfxPanel::Render()
{
	IgfxGlobal->UseShaders(shader);
	GLuint vertexbuffer = IgfxGlobal->getGlobalVertexArrayBuffer();
	GLuint uvbuffer = IgfxGlobal->getGlobalUVArrayBuffer();
	GLuint samplerId = IgfxGlobal->getGlobalTextureSamplerId();

	glm::mat4 panelTransform = this->getTransform();
	IgfxGlobal->setUniformMatrix(panelTransform);

	for (auto& group : renderlist)
	{
		if (group.second.sprites.empty())
			continue;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (*(group.second.sprites.begin()))->getSprite().getTextureId());
		
		if (!group.second.colorbuffer)
		{
			glGenBuffers(1, &group.second.colorbuffer);
		}
		if (!group.second.sizebuffer)
		{
			glGenBuffers(1, &group.second.sizebuffer);
		}
		if (!group.second.uvbuffer)
		{
			glGenBuffers(1, &group.second.uvbuffer);
		}
		if (!group.second.matrixbuffer)
		{
			glGenBuffers(1, &group.second.matrixbuffer);
		}

		if (group.second.buffersize < group.second.sprites.size())
		{
			group.second.buffersize = group.second.sprites.size();
			group.second.color.resize(group.second.buffersize);
			group.second.size.resize(group.second.buffersize);
			group.second.uv_rect.resize(group.second.buffersize);
			group.second.matrix.resize(group.second.buffersize * 3);

			glBindBuffer(GL_ARRAY_BUFFER, group.second.colorbuffer);
			glBufferData(GL_ARRAY_BUFFER, group.second.buffersize * sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, group.second.sizebuffer);
			glBufferData(GL_ARRAY_BUFFER, group.second.buffersize * sizeof(glm::vec2), 0, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, group.second.uvbuffer);
			glBufferData(GL_ARRAY_BUFFER, group.second.buffersize * sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, group.second.matrixbuffer);
			glBufferData(GL_ARRAY_BUFFER, group.second.buffersize * sizeof(glm::vec3) * 3, 0, GL_DYNAMIC_DRAW);
		}

		glUniform1i(samplerId, 0);

		int color_count = 0;
		for (auto& sprite : group.second.sprites)
		{			
			group.second.color[color_count]= sprite->getColor();
			group.second.size[color_count] = sprite->getSprite().getSize();
			group.second.uv_rect[color_count] = sprite->getSprite().getRect();

			group.second.matrix[color_count * 3] = sprite->getPosition();
			group.second.matrix[color_count * 3 + 1] = sprite->getScale();
			group.second.matrix[color_count * 3 + 2] = sprite->getPivot();
			color_count++;
		}

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
			2,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, group.second.colorbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*group.second.color.size(), static_cast<void*>(group.second.color.data()));
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
		glBindBuffer(GL_ARRAY_BUFFER, group.second.sizebuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2)*group.second.size.size(), static_cast<void*>(group.second.size.data()));
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
		glBindBuffer(GL_ARRAY_BUFFER, group.second.uvbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*group.second.uv_rect.size(), static_cast<void*>(group.second.uv_rect.data()));
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
		glBindBuffer(GL_ARRAY_BUFFER, group.second.matrixbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*group.second.matrix.size(), static_cast<void*>(group.second.matrix.data()));
		glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(0));
		glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(sizeof(float) * 3));
		glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(sizeof(float) * 6));
		//glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
		glVertexAttribDivisor(pos1, 1);
		glVertexAttribDivisor(pos2, 1);
		glVertexAttribDivisor(pos3, 1);
		//glVertexAttribDivisor(pos4, 1);

		// Draw the triangle !
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, group.second.sprites.size());
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
}

void gfx::gfxPanel::addSprite(gfxSprite* sprite)
{
	GLuint texture_id = sprite->getSprite().getTextureId();
	auto& pair = renderlist.find(texture_id);
	if (pair == renderlist.end())
	{
		renderlist[texture_id] = renderlistGroup();
	}
	auto& group = renderlist[texture_id];

	group.sprites.insert(sprite);
}

void gfx::gfxPanel::RemoveSprite(gfxSprite* sprite)
{
	GLuint texture_id = sprite->getSprite().getTextureId();
	auto& pair = renderlist.find(texture_id);
	if (pair == renderlist.end())
	{
		return;
	}
	auto& group = pair->second;
	group.sprites.erase(sprite);
}
