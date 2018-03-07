#include "../stdafx.h"
#include "PlayScene.h"
#include "../BMS/BMSParser.h"

PlayScene::PlayScene(): gfxScene()
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update(std::chrono::milliseconds delta)
{
	std::cout << delta.count() << std::endl;
}

void PlayScene::Render()
{
}

void PlayScene::Init()
{
	BMSParser parser;
	bms = parser.Parse("resource/Fellhound/Fellhound (Original Mix)7.bms");
}
