#include "../stdafx.h"
#include "gfxObject.h"

gfx::gfxObject::gfxObject()
{

}

gfx::gfxObject::~gfxObject()
{
}

glm::mat4 gfx::gfxObject::getTransform()
{
	glm::mat4 matScale = glm::scale(scale);
	glm::mat4 matRotate = glm::toMat4(glm::quat(rotation));
	glm::mat4 matPosition = glm::translate(position);
	
	return matPosition * matRotate * matScale;
}
