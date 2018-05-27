#pragma once

class Sprite
{
private:
	glm::uint texture_id = 0;
	glm::vec2 size;
	glm::vec4 texture_rect;
	bool delete_texture_itself;
public:
	Sprite(const glm::uint& texture_id, const glm::vec2& size, const glm::vec4& texture_rect, bool delete_texture_itself = true);
	
	inline glm::uint getTextureId()
	{
		return texture_id;
	}

	inline const glm::vec2& getSize()
	{
		return size;
	}

	inline const glm::vec4& getRect()
	{
		return texture_rect;
	}
	~Sprite();
};
