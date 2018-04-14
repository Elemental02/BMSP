#include "../stdafx.h"
#include "../BMS/BMS.h"
#include "../BMS/BMSParser.h"
#include "ListScene.h"

#include "../managers/InputManager.h"
//using namespace std::experimental::filesystem;

void find_bms(std::experimental::filesystem::path path_to_explore, std::vector<std::string>& bms_list)
{
	std::experimental::filesystem::directory_iterator dir_it(path_to_explore);
	for (auto& path : dir_it)
	{
		if (std::experimental::filesystem::is_directory(path.path()))
		{
			find_bms(path.path(), bms_list);
		}
		else if (path.path().extension().string() == ".bms" || path.path().extension().string() == ".bml" || path.path().extension().string() == ".bme")
		{
			bms_list.push_back(path.path().string());
		}
	}
}

ListScene::ListScene()
{
	
}

ListScene::~ListScene()
{
}

void ListScene::Update(std::chrono::milliseconds delta)
{
	switch (this->scene_state)
	{
	case SceneState::Initializing:
		scene_state = SceneState::List_Loading;
		break;
	case SceneState::List_Loading:
		scene_state = SceneState::List_Playing;
		break;

	case SceneState::List_Playing:
		
		break;
	}
}

void ListScene::Render()
{
	for (auto& str : str_bms_list)
	{
		str.Render();
	}
}

void ListScene::Init()
{
	int pos = 0;
	mainfont = std::make_shared<gfx::gfxFont>(gfx::gfxFont());
	mainfont->LoadFont("resource/KOPUSGoM.ttf");
	find_bms(std::experimental::filesystem::current_path(), bms_path_list);
	for (auto path : bms_path_list)
	{
		bms_list.push_back(BMSParser::Parse(path, true));
		auto& bms = bms_list.back();

		str_bms_list.push_back(gfx::gfxString());
		auto& str = str_bms_list.back();
		str.setFont(mainfont);
		str.setPixelSize(30);
		str.setString(bms.metadata["TITLE"]);
	}
}
