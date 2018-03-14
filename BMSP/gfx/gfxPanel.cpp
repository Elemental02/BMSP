#include "../stdafx.h"
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
		glBindTexture(GL_TEXTURE_2D, (*(group.second.sprites.begin()))->getSprite().textureId);
		
		glUniform1i(samplerId, 0);

		glm::mat4 panelTransform = this->getTransform();

		int vertex_count = 0, color_count = 0, uv_count = 0, matrix_count = 0;
		for (auto& sprite : group.second.sprites)
		{
			for (int i = 0; i < sprite->getSprite().vertex.size(); i++)
			{
				group.second.vertex[vertex_count] = sprite->getSprite().vertex[i];
				vertex_count++;
			}
			for (int i = 0; i < sprite->getSprite().uv.size(); i++)
			{
				group.second.uv[uv_count] = sprite->getSprite().uv[i];
				uv_count++;
			}
			
			group.second.color[color_count]= sprite->getColor();
			color_count++;

			group.second.matrix[matrix_count] = panelTransform * sprite->getTransform();
			matrix_count++;
		}

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*group.second.vertex.size(), static_cast<void*>(group.second.vertex.data()), GL_STATIC_DRAW);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*group.second.color.size(), static_cast<void*>(group.second.color.data()), GL_STATIC_DRAW);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*group.second.uv.size(), static_cast<void*>(group.second.uv.data()), GL_STATIC_DRAW);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4)*group.second.matrix.size(), static_cast<void*>(group.second.matrix.data()), GL_STATIC_DRAW);
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
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, group.second.vertex.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}
}

void gfx::gfxPanel::addSprite(gfxSprite* sprite)
{
	GLuint textureId = sprite->getSprite().textureId;
	auto& pair = renderlist.find(textureId);
	if (pair == renderlist.end())
	{
		renderlist[textureId] = renderlistGroup();
	}
	auto& group = renderlist[textureId];

	group.vertex.insert(group.vertex.end(), sprite->getSprite().vertex.begin(), sprite->getSprite().vertex.end());
	group.uv.insert(group.uv.end(), sprite->getSprite().uv.begin(), sprite->getSprite().uv.end());
	group.color.push_back(sprite->getColor());
	group.matrix.push_back(sprite->getTransform());

	group.sprites.push_back(sprite);
}

void gfx::gfxPanel::RemoveSprite(gfxSprite* sprite)
{
	GLuint textureId = sprite->getSprite().textureId;
	auto& pair = renderlist.find(textureId);
	if (pair == renderlist.end())
	{
		return;
	}
	auto& group = pair->second;
	group.vertex.erase(group.vertex.end() - sprite->getSprite().vertex.size(), group.vertex.end());
	group.uv.erase(group.uv.end() - sprite->getSprite().uv.size(), group.uv.end());
	group.color.pop_back();
	group.matrix.pop_back();
	group.sprites.remove(sprite);
}
