#include "../stdafx.h"
#include "Sprite.h"

Sprite::Sprite(const glm::uint& texture_id, const glm::vec2& size, const glm::vec4& texture_rect, bool delete_texture_itself) :texture_id(texture_id), size(size), texture_rect(texture_rect), delete_texture_itself(delete_texture_itself)
{

}

Sprite::~Sprite()
{
	if (delete_texture_itself)
		glDeleteTextures(1, &texture_id);
}