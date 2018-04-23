#include "../stdafx.h"
#include "gfxUpdatable.h"

gfx::gfxUpdatableFunc::gfxUpdatableFunc()
{
}

gfx::gfxUpdatableFunc::gfxUpdatableFunc(std::function<bool(std::chrono::milliseconds)> func) : function(func)
{

}

bool gfx::gfxUpdatableFunc::Update(std::chrono::milliseconds delta)
{
	elasped_time += delta.count();
	if (function)
	{
		return function(delta);
	}
	return false;
}
