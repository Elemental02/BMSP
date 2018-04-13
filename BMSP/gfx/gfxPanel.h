#pragma once
#include "gfxObject.h"
namespace gfx
{
	class gfxPanel : public gfxObject
	{
	private:
		struct renderlistGroup
		{
			std::vector<glm::vec4> color;
			std::vector<glm::vec2> size;
			std::vector<glm::vec4> uv_rect;
			std::vector<glm::mat4> matrix;
			std::list<gfxSprite*> sprites;

			int buffersize = 0;
			GLuint colorbuffer = 0;
			GLuint sizebuffer = 0;
			GLuint uvbuffer = 0;
			GLuint matrixbuffer = 0;
		};
		std::map<GLuint, renderlistGroup> renderlist;

	public:
		virtual void Render();

		void addSprite(gfxSprite* sprite);
		void RemoveSprite(gfxSprite* sprite);
	};
}