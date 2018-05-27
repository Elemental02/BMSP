#include "../stdafx.h"
#include "../managers/GlobalManager.h"
#include "gfxSpriteFont.h"

void gfx::gfxSpriteFont::RenderString()
{
	int strlen = str.length();
	for (auto& rendergroup : renderlist)
	{
		rendergroup.second.strsize = 0;
	}

	float pos = 0;
	for (int i = 0; i < strlen; i++)
	{
		auto glyph = sprite_list[str[i]];
		int texture_id = glyph->getTextureId();
		if (renderlist.find(texture_id) == renderlist.end())
			renderlist[texture_id] = renderlistGroup();

		int str_cnt = renderlist[texture_id].strsize;
		if (renderlist[texture_id].size.size() > str_cnt)
			renderlist[texture_id].size[str_cnt] = glyph->getSize();
		else
			renderlist[texture_id].size.push_back(glyph->getSize());

		if (renderlist[texture_id].uv_rect.size() > str_cnt)
			renderlist[texture_id].uv_rect[str_cnt] = glyph->getRect();
		else
			renderlist[texture_id].uv_rect.push_back(glyph->getRect());

		if (renderlist[texture_id].matrix.size() > (str_cnt * 2))
		{
			renderlist[texture_id].matrix[str_cnt * 2] = glm::vec3(pos, 0.0f, 0.0f);
			renderlist[texture_id].matrix[str_cnt * 2 + 1] = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else
		{
			renderlist[texture_id].matrix.push_back(glm::vec3(pos, 0.0f, 0.0f));
			renderlist[texture_id].matrix.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
		}
		renderlist[texture_id].strsize++;
		pos += glyph->getSize().x;
	}

	int alignment = this->alignment == Align::Center ? -pos / 2.0f : -pos;
	for (auto& rendergroup : renderlist)
	{
		if (this->alignment != Align::Left)
		{
			for (int i = 0; i < rendergroup.second.strsize; i++)
			{
				rendergroup.second.matrix[i * 2].x += alignment;
			}
		}

		if (!rendergroup.second.sizebuffer)
		{
			glGenBuffers(1, &rendergroup.second.sizebuffer);
		}
		if (!rendergroup.second.uvbuffer)
		{
			glGenBuffers(1, &rendergroup.second.uvbuffer);
		}
		if (!rendergroup.second.matrixbuffer)
		{
			glGenBuffers(1, &rendergroup.second.matrixbuffer);
		}

		if (rendergroup.second.buffersize < rendergroup.second.strsize)
		{
			rendergroup.second.buffersize = rendergroup.second.strsize;
			glBindBuffer(GL_ARRAY_BUFFER, rendergroup.second.sizebuffer);
			glBufferData(GL_ARRAY_BUFFER, rendergroup.second.buffersize * sizeof(glm::vec2), 0, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, rendergroup.second.uvbuffer);
			glBufferData(GL_ARRAY_BUFFER, rendergroup.second.buffersize * sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, rendergroup.second.matrixbuffer);
			glBufferData(GL_ARRAY_BUFFER, rendergroup.second.buffersize * sizeof(glm::vec3) * 2, 0, GL_DYNAMIC_DRAW);
		}
	}
}

void gfx::gfxSpriteFont::Render()
{
	if (str.length() == 0)
		return;
	if (is_dirty)
		RenderString();
	IgfxGlobal->UseShaders(shader);
	IgfxGlobal->setUniformMatrix(getTransform());

	GLuint vertexbuffer = IgfxGlobal->getGlobalVertexArrayBuffer();
	GLuint colorbuffer = IgfxGlobal->getGlobalColorArrayBuffer();
	GLuint uvbuffer = IgfxGlobal->getGlobalUVArrayBuffer();
	GLuint samplerId = IgfxGlobal->getGlobalTextureSamplerId();

	for (auto& rendergroup : renderlist)
	{
		if (rendergroup.second.strsize == 0)
			continue;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rendergroup.first);
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
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4), static_cast<void*>(&color));
		glVertexAttribPointer(
			2,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
			4,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glVertexAttribDivisor(2, rendergroup.second.strsize);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, rendergroup.second.sizebuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2)*rendergroup.second.strsize, static_cast<void*>(rendergroup.second.size.data()));
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
		glBindBuffer(GL_ARRAY_BUFFER, rendergroup.second.uvbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*rendergroup.second.strsize, static_cast<void*>(rendergroup.second.uv_rect.data()));
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
		//glEnableVertexAttribArray(pos3);
		//glEnableVertexAttribArray(pos4);
		glBindBuffer(GL_ARRAY_BUFFER, rendergroup.second.matrixbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*rendergroup.second.matrix.size(), static_cast<void*>(rendergroup.second.matrix.data()));
		glVertexAttribPointer(pos1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2 * 3, (void*)(0));
		glVertexAttribPointer(pos2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2 * 3, (void*)(sizeof(float) * 3));
		//glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		//glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
		glVertexAttribDivisor(pos1, 1);
		glVertexAttribDivisor(pos2, 1);
		//glVertexAttribDivisor(pos3, 1);
		//glVertexAttribDivisor(pos4, 1);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, rendergroup.second.strsize);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(pos1);
		glDisableVertexAttribArray(pos2);
	}
}
