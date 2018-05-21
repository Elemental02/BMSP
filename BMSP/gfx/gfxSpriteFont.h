#pragma once
#include "gfxObject.h"
namespace gfx
{
	class gfxSpriteFont : public gfxObject
	{
	private:
		std::map<int, std::shared_ptr<Sprite>> sprite_list;
		std::string str;
		Align alignment = Align::Left;
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		struct renderlistGroup
		{
			int strsize;
			std::vector<glm::vec2> size;
			std::vector<glm::vec4> uv_rect;
			std::vector<glm::vec3> matrix;

			int buffersize = 0;
			GLuint colorbuffer = 0;
			GLuint sizebuffer = 0;
			GLuint uvbuffer = 0;
			GLuint matrixbuffer = 0;
		};
		std::map<GLuint, renderlistGroup> renderlist;

		bool is_dirty = true;
		void RenderString();
	public:
		virtual void Render();

		inline void setAlign(Align align)
		{
			alignment = align;
		}

		void addSprite(int ch, std::shared_ptr<Sprite> sprite)
		{
			sprite_list[ch] = sprite;
		}

		inline void setString(std::string string)
		{
			if (str == string)
				return;
			is_dirty = true;
			str = string;
		}

	};
}