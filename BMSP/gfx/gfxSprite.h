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
		std::string shader="sprite";
	public:
		gfxSprite();

		void setSprite(std::shared_ptr<Sprite> sprite);
		inline Sprite& getSprite()
		{
			return *sprite;
		}

		virtual ~gfxSprite();
		virtual void Render();

		inline void setColor(glm::vec4 color)
		{
			this->color = color;
		}
		inline glm::vec4& getColor()
		{
			return color;
		}
		inline void setShader(std::string str)
		{
			shader = str;
		}
	};
};