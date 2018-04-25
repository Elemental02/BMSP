#include "../stdafx.h"
#include "../managers/GlobalManager.h"
#include "../gfx/gfxString.h"
#include "../gfx/gfxSprite.h"
#include "testScene.h"

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Update(std::chrono::milliseconds delta)
{
	glm::vec3 pos = sprites[0]->getPosition();
	pos.x += delta.count();
	sprites[0]->setPosition(pos);
	//str.setString(str.getString()+"a");
}

void TestScene::Render()
{
	sprites[0]->Render();
	for (auto& s : str)
	{
		s.Render();
	}
}

void TestScene::Init()
{
	font = std::shared_ptr<gfx::gfxFont>(new gfx::gfxFont());
	font->LoadFont("resource/KOPUSGoM.ttf");
	for (int i = 0; i < 1; i++)
	{
		str.push_back(gfx::gfxString());
		str[i].setFont(font);
		str[i].setString("asdfÇÏ");
	}
	
	sprites.push_back(std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite));
	auto sprite = IResourceManager->LoadSprite("resource/action1_01.bmp");
	sprites[0]->setSprite(sprite);
}
