#include "../stdafx.h"
#include <GL/glew.h>
#include "gfxSprite.h"
#include "../managers/ResourceManager.h"
#include "gfxPanel.h"

#include "gfxGlobal.h"

void gfx::gfxPanel::Render()
{
	GLuint vertexbuffer = gfxGlobal::Instance()->getGlobalVertexArrayBuffer();
	GLuint colorbuffer = gfxGlobal::Instance()->getGlobalColorArrayBuffer();
	GLuint uvbuffer = gfxGlobal::Instance()->getGlobalUVArrayBuffer();
	GLuint transformBuffer = gfxGlobal::Instance()->getGlobalMatrixArrayBuffer();
	GLuint samplerId = gfxGlobal::Instance()->getGlobalTextureSamplerId();

	for (auto group : renderlist)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, group.second.begin()->getSprite().textureId);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(samplerId, 0);

		glm::mat4 panelTransform = this->getTransform();

		std::vector<glm::vec3> vertex;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec4> color;
		std::vector<glm::mat4> matrix;

		for (auto& sprite : group.second)
		{
			vertex.insert(vertex.end(), sprite.getSprite().vertex.begin(), sprite.getSprite().vertex.end());
			uv.insert(uv.end(), sprite.getSprite().uv.begin(), sprite.getSprite().uv.end());
			color.push_back(sprite.getColor());
			matrix.push_back(panelTransform*sprite.getTransform());
		}
		int i = 0;
		for (auto& vert : vertex)
		{
			vert.x += 10.0f*i;
			i++;
		}

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertex.size(), static_cast<void*>(vertex.data()), GL_STATIC_DRAW);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*color.size(), static_cast<void*>(color.data()), GL_STATIC_DRAW);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uv.size(), static_cast<void*>(uv.data()), GL_STATIC_DRAW);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4)*matrix.size(), static_cast<void*>(matrix.data()), GL_STATIC_DRAW);
		glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
		glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
		glVertexAttribDivisor(pos1, 1);
		glVertexAttribDivisor(pos2, 1);
		glVertexAttribDivisor(pos3, 1);
		glVertexAttribDivisor(pos4, 1);

		// Draw the triangle !
		//glDrawArrays(GL_TRIANGLES, 0, vertex.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, group.second.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}
}

void gfx::gfxPanel::addSprite(gfxSprite& sprite)
{
	GLuint textureId = sprite.getSprite().textureId;
	if (renderlist.find(textureId) == renderlist.end())
	{
		renderlist[textureId] = std::list<gfxSprite>();
	}
	renderlist[textureId].push_back(sprite);
}
