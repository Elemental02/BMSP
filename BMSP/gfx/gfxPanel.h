#pragma once
#include "gfxObject.h"
namespace gfx
{
	class gfxPanel : public gfxObject
	{
	private:
		struct renderlistGroup
		{
			std::vector<glm::vec3> vertex;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec4> color;
			std::vector<glm::mat4> matrix;
			std::list<gfxSprite> sprites;
		};
		std::map<GLuint, renderlistGroup> renderlist;

	public:
		virtual void Render();

		void addSprite(gfxSprite& sprite);
		void RemoveSprite(gfxSprite& sprite);
	};
}