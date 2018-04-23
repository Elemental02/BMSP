#pragma once
#include "gfxObject.h"
#include "gfxFont.h"
namespace gfx
{
	class gfxString : public gfxObject
	{
	private:
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		std::shared_ptr<gfxFont> font;

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

		std::string str;
		std::vector<int> char_vector;

		int pixelSize = 15;
		Align alignment = Align::Left;

		bool is_dirty = true;

		void RenderString();
	public:
		gfxString();
		virtual ~gfxString();

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