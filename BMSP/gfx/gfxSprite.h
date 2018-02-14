#pragma once
#include "gfxObject.h"

struct Sprite;
namespace gfx
{
	class gfxSprite : public gfxObject
	{
	private:
		std::shared_ptr<Sprite> sprite;
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	public:
		gfxSprite();

		void setSprite(std::shared_ptr<Sprite> sprite);
		inline Sprite& getSprite()
		{
			return *sprite;
		}

		inline void setColor(glm::vec4 color)
		{
			this->color = color;
		}
		inline glm::vec4& getColor()
		{
			return color;
		}

		virtual ~gfxSprite();
		virtual void Render();
	};
};