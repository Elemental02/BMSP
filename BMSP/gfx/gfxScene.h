#pragma once
namespace gfx
{
	class gfxScene
	{
	public:
		gfxScene();
		virtual ~gfxScene();
		virtual void Update(std::chrono::milliseconds delta) = 0;
		virtual void Render() = 0;
	};
};