#include "../stdafx.h"
#include "../BMS/BMS.h"
#include "../BMS/BMSParser.h"
#include "ListScene.h"

#include "../managers/InputManager.h"
/*
void find_bms(std::experimental::filesystem::path path_to_explore, std::vector<std::string>& bms_list)
{
	std::list<std::experimental::filesystem::path> path_queue;
	path_queue.push_back(path_to_explore);
	auto& curr_path = path_queue.begin();
	do
	{
		std::experimental::filesystem::directory_iterator dir_it(curr_path);
		for (auto& path : dir_it)
		{
			if (std::experimental::filesystem::is_directory(path.path()))
			{
				path_queue.push_back(path.path());
			}
			else if (path.path().extension().string() == ".bms" || path.path().extension().string() == ".bml" || path.path().extension().string() == ".bme")
			{
				bms_list.push_back(path.path().string());
			}
		}
		curr_path++;
	} while (curr_path != path_queue.end());
}*/

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
		str_autoplay.setString(init_string.getString());
		if (init_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
		{
			init_future.get();
			for(auto& bms : bms_list)
			{
				str_bms_list.push_back(gfx::gfxString());
				auto& str = str_bms_list.back();
				str.setFont(mainfont);
				str.setPixelSize(30);
				str.setString(bms.metadata["TITLE"]);
			}
			scene_state = List_Loading;
		}
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
	switch (this->scene_state)
	{
	case SceneState::Initializing:
		str_autoplay.Render();
		break;
	case SceneState::List_Loading:
		break;
	case SceneState::List_Playing:
		for (auto& str : str_bms_list)
		{
			str.Render();
		}
		break;
	}
}

void ListScene::Init()
{
	int pos = 0;
	mainfont = std::make_shared<gfx::gfxFont>(gfx::gfxFont());
	mainfont->LoadFont("resource/KOPUSGoM.ttf");
	str_autoplay.setFont(mainfont);
	str_autoplay.setPixelSize(30);
	init_string.string = "Wait for Loading";
	init_future = std::async(std::launch::async, [&](){
		std::list<std::experimental::filesystem::path> path_queue;
		path_queue.push_back(std::experimental::filesystem::current_path());
		std::vector<std::string> str_list;
		auto curr_path = path_queue.begin();
		do
		{
			init_string.setString("search path: " + curr_path->string());
			std::experimental::filesystem::directory_iterator dir_it(*curr_path);
			for (auto& path : dir_it)
			{
				if (std::experimental::filesystem::is_directory(path.path()))
				{
					path_queue.push_back(path.path());
				}
				else if (path.path().extension().string() == ".bms" || path.path().extension().string() == ".bml" || path.path().extension().string() == ".bme")
				{
					str_list.push_back(path.path().string());
				}
			}
			curr_path++;
		} while (curr_path != path_queue.end());

		for (auto path : str_list)
		{
			init_string.setString("parsing bms: " + path);
			bms_list.push_back(BMSParser::Parse(path, true));
		}
		return 0;
	});
}
