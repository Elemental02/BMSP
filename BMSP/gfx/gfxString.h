#pragma once
#include "gfxObject.h"
#include "gfxFont.h"
namespace gfx
{
	class gfxString : public gfxObject
	{
	private:
		std::vector<glm::vec2> size;
		std::vector<glm::vec4> uv_rect;
		std::vector<glm::mat4> mats;
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		std::shared_ptr<gfxFont> font;

		int buffersize = 0;
		GLuint sizebuffer = 0;
		GLuint uvrectbuffer = 0;
		GLuint matrixbuffer = 0;

		GLuint texture_id = 0;
		std::string str;

		bool is_dirty = true;

		int pixelSize = 200;

		void RenderString();
	public:
		void setString(std::string str);
		inline std::string getString() {
			return str;
		}
		virtual void Render();

		inline void setPixelSize(int size) {
			pixelSize = size;
		}
		inline void setFont(std::shared_ptr<gfxFont> font) {
			this->font = font;
		}

		inline void setColor(glm::vec4 color)
		{
			this->color = color;
		}
		inline glm::vec4& getColor()
		{
			return this->color;
		}
	};
}