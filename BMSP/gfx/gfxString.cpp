#include "../stdafx.h"
#include "../managers/GlobalManager.h"
#include "gfxString.h"

#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/locid.h>
void gfx::gfxString::RenderString()
{
	int strlen = str.length();
	for (auto& rendergroup : renderlist)
	{
		rendergroup.second.strsize = 0;
	}
	
	auto& loc = icu::Locale::getJapanese();
	auto t = icu::UnicodeString(str.c_str());
	auto len = t.length();
	int char_len = t.length();
	for (int i = 0; i < char_len; i++)
	{
		if (char_vector.size() <= i)
			char_vector.push_back(t[i]);
		else
			char_vector[i] = t[i];
	}

	int pos_x = 0, pos_y = 0;
	int linestart = 0;
	std::vector<std::pair<int, int>> line;
	for (int i = 0; i < char_len; i++)
	{
		auto glyph = font->LoadChar(char_vector[i], pixel_size);

		if (max_width && pos_x + glyph.advance > max_width)
		{
			if (this->alignment != Align::Left)
			{
				int alignment = this->alignment == Align::Center ? -pos_x / 2.0f : -pos_x;

				for (auto pair : line)
				{
					renderlist[pair.first].matrix[pair.second*2].x += alignment;
				}
				/*for (auto& rendergroup : renderlist)
				{
					for (int i = 0; i < rendergroup.second.strsize; i++)
					{
						rendergroup.second.matrix[i * 2].x += alignment;
					}
				}*/
			}
			pos_x = 0;
			pos_y += pixel_size;
			line.erase(line.begin(), line.end());
		}

		int texture_id = glyph.sprite->texture_id;
		if (renderlist.find(texture_id) == renderlist.end())
			renderlist[texture_id] = renderlistGroup();
		
		int str_cnt = renderlist[texture_id].strsize;
		if (renderlist[texture_id].size.size() > str_cnt)
			renderlist[texture_id].size[str_cnt] = glyph.sprite->size;
		else
			renderlist[texture_id].size.push_back(glyph.sprite->size);

		if (renderlist[texture_id].uv_rect.size() > str_cnt)
			renderlist[texture_id].uv_rect[str_cnt] = glyph.sprite->texture_rect;
		else
			renderlist[texture_id].uv_rect.push_back(glyph.sprite->texture_rect);
		
		if (renderlist[texture_id].matrix.size() > str_cnt * 2)
		{
			renderlist[texture_id].matrix[str_cnt * 2] = glm::vec3(pos_x + glyph.left, pos_y + pixel_size - glyph.top, 0.0f);
			renderlist[texture_id].matrix[str_cnt * 2 + 1] = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else
		{
			renderlist[texture_id].matrix.push_back(glm::vec3(pos_x + glyph.left, pos_y + pixel_size - glyph.top, 0.0f));
			renderlist[texture_id].matrix.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
		}
		if (max_width)
			line.push_back(std::pair<int, int>(texture_id, renderlist[texture_id].strsize));
		renderlist[texture_id].strsize++;
		pos_x += glyph.advance;
	}
	for (auto& rendergroup : renderlist)
	{
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

	is_dirty = false;
}

gfx::gfxString::gfxString()
{
}

gfx::gfxString::~gfxString()
{
	for (auto& rendergroup : renderlist)
	{
		glDeleteBuffers(1, &rendergroup.second.sizebuffer);
		glDeleteBuffers(1, &rendergroup.second.matrixbuffer);
		glDeleteBuffers(1, &rendergroup.second.uvbuffer);
	}
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

	IgfxGlobal->setUniformMatrix(getTransform());

	GLuint vertexbuffer = IgfxGlobal->getGlobalVertexArrayBuffer();
	GLuint colorbuffer = IgfxGlobal->getGlobalColorArrayBuffer();
	GLuint uvbuffer = IgfxGlobal->getGlobalUVArrayBuffer();
	GLuint samplerId = IgfxGlobal->getGlobalTextureSamplerId();

	for (auto& rendergroup : renderlist)
	{
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