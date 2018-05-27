#pragma once
#include "../resource/Sprite.h"
#include "gfxObject.h"

namespace gfx
{
	class gfxSprite : public gfxObject
	{
	private:
		std::shared_ptr<Sprite> sprite;
		glm::vec3 pivot = { 0.0f, 0.0f, 0.0f };
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
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

		inline void setPivot(glm::vec3 piv)
		{
			this->pivot = piv;
		}
		inline glm::vec3& getPivot()
		{
			return pivot;
		}
	};
};