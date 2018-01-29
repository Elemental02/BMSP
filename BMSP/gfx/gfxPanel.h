#pragma once
#include "gfxObject.h"
namespace gfx
{
	class gfxPanel : public gfxObject
	{
	private:
		std::map<GLuint, std::list<gfxSprite>> renderlist;
	public:
		virtual void Render();

		void addSprite(gfxSprite& sprite);
	};
}