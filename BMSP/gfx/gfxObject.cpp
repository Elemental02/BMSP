#include "../stdafx.h"
#include "gfxObject.h"

gfx::gfxObject::gfxObject():is_dirty(true)
{

}

gfx::gfxObject::~gfxObject()
{
}

glm::mat4 gfx::gfxObject::getTransform()
{
	if (is_dirty)
	{
		glm::mat4 matScale = glm::scale(scale);
		glm::mat4 matRotate = glm::toMat4(glm::quat(rotation));
		glm::mat4 matPosition = glm::translate(position);

		mat = matPosition * matRotate * matScale;
		is_dirty = false;
	}
	return mat;
}
