#pragma once
#include "gfxSprite.h"
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
			std::vector<glm::vec3> matrix;
			//std::vector<glm::mat4> matrix;
			std::set<gfxSprite*> sprites;

			int buffersize = 0;
			GLuint colorbuffer = 0;
			GLuint sizebuffer = 0;
			GLuint uvbuffer = 0;
			GLuint matrixbuffer = 0;
		};
		std::map<GLuint, renderlistGroup> renderlist;

	public:
		virtual ~gfxPanel();
		virtual void Render();

		void addSprite(gfxSprite* sprite);
		void RemoveSprite(gfxSprite* sprite);
	};
}