#pragma once
namespace gfx
{
	class gfxScene
	{
	public:
		gfxScene();
		virtual ~gfxScene();
		virtual void Render();
	};
};