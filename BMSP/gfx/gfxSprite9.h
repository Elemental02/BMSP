#pragma once

namespace gfx
{
	class gfxSprite9 : public gfxObject
	{
	private:
		std::shared_ptr<Sprite> sprite;
		std::vector<Sprite> sprites;
		std::array<glm::vec3, 9> positions;
		glm::vec2 size;
		glm::vec3 pivot = { 0.0f, 0.0f, 0.0f };
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 rect = { 0.0f, 0.0f, 1.0f, 1.0f };

		void Update();
	public:
		gfxSprite9();

		void setSprite(std::shared_ptr<Sprite> sprite, glm::vec4 rect);
		inline Sprite& getSprite()
		{
			return *sprite;
		}

		virtual ~gfxSprite9();
		virtual void Render();

		inline void setSize(glm::vec2 size)
		{
			this->size = size;
		}

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