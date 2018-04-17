#pragma once
#include "../gfx/gfxScene.h"
#include "../gfx/gfxString.h"

class TestScene : public gfx::gfxScene
{
	std::shared_ptr<gfx::gfxFont> font;
	std::vector<gfx::gfxString> str;
	std::vector<std::shared_ptr<gfx::gfxSprite>> sprites;
public:
	TestScene();
	virtual ~TestScene();
	virtual void Update(std::chrono::milliseconds delta);
	virtual void Render();

	virtual void Init();
};