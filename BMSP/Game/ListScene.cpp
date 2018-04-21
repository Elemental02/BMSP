#include "../stdafx.h"
#include "../BMS/BMS.h"
#include "../BMS/BMSParser.h"
#include "../managers/GlobalManager.h"

#include "ListScene.h"
#include "PlayScene.h"

const int pixelSize = 30;
ListScene::ListScene()
{
	
}

ListScene::~ListScene()
{
}

void ListScene::Update(std::chrono::milliseconds delta)
{
	int bms_count = bms_list.size();
	switch (this->scene_state)
	{
	case SceneState::Initializing:
		str_artist.setString(init_string.getString());
		if (init_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
		{
			init_future.get();
			/*for(auto& bms : bms_list)
			{
				str_bms_list.push_back(gfx::gfxString());
				auto& str = str_bms_list.back();
				str.setFont(mainfont);
				str.setPixelSize(pixelSize);
				str.setString(bms.metadata["TITLE"]);
			}*/
			for (int i = 0; i < 21; i++)
			{
				str_bms_list.push_back(gfx::gfxString());
				auto& str = str_bms_list.back();
				str.setFont(mainfont);
				str.setPixelSize(pixelSize);
			}
			scene_state = List_Loading;
		}
		break;
	case SceneState::List_Loading:
	{
		str_artist.setString("Artist: " + bms_list[cursor_index].metadata["ARTIST"]);
		str_genre.setString("Genre: " + bms_list[cursor_index].metadata["GENRE"]);
		str_bpm.setString("BPM: " + std::to_string(bms_list[cursor_index].bpm));
		scene_state = SceneState::List_Playing;
		int start_index = cursor_index - 10;
		for (int i = 0; i < 21; i++, start_index++)
		{
			if(start_index<0)
				start_index+= bms_list.size();
			else if(start_index>=bms_list.size())
				start_index -= bms_list.size();
			str_bms_list[i].setPosition(glm::vec3(240.0f, (i - 10) * pixelSize + 285, 0.0f));
			str_bms_list[i].setString(bms_list[start_index].metadata["TITLE"]);
		}
	}
		break;

	case SceneState::List_Playing:
		if (IInputManager->getKeyState(KeyIndex::Up) == KeyState::State_Press)
		{
			cursor_index--;
			if (cursor_index < 0)
				cursor_index = bms_count - 1;
			scene_state = List_Loading;
		}
		else if (IInputManager->getKeyState(KeyIndex::Down) == KeyState::State_Press)
		{
			cursor_index++;
			if (cursor_index >= bms_count)
				cursor_index = 0;
			scene_state = List_Loading;
		}

		if (IInputManager->getKeyState(KeyIndex::Enter) == KeyState::State_Press)
		{
			auto scene = std::shared_ptr<PlayScene>(new PlayScene);
			scene->SetBMSPath(bms_path_list[cursor_index]);
			IGlobalManager->Push_Scene(scene);
			scene_state = List_Loading;
		}


		if (IInputManager->getKeyState(KeyIndex::ESC) == KeyState::State_Press)
		{
			IGlobalManager->Pop_Scene();
		}
		break;
	}
}

void ListScene::Render()
{
	switch (this->scene_state)
	{
	case SceneState::Initializing:
		str_artist.Render();
		break;
	case SceneState::List_Loading:
	case SceneState::List_Playing:
		str_artist.Render();
		str_bpm.Render();
		str_genre.Render();
		str_list_corsor.Render();
		str_autoplay.Render();
		str_autoplay_on.Render();
		str_autoplay_off.Render();
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
	init_string.string = "Wait for Loading";

	str_artist.setFont(mainfont);
	str_bpm.setFont(mainfont);
	str_genre.setFont(mainfont);
	str_list_corsor.setFont(mainfont);
	str_autoplay_on.setFont(mainfont);
	str_autoplay_off.setFont(mainfont);

	str_artist.setPixelSize(pixelSize);
	str_bpm.setPixelSize(pixelSize);
	str_genre.setPixelSize(pixelSize);
	str_list_corsor.setPixelSize(pixelSize);
	str_autoplay_on.setPixelSize(pixelSize);
	str_autoplay_off.setPixelSize(pixelSize);

	str_list_corsor.setString("->");
	str_list_corsor.setPosition(glm::vec3(180.0f, 300 - 15, 0.0f));
	str_autoplay_on.setString("On");
	str_autoplay_on.setPosition(glm::vec3(380.0f, 570.0f, 0.0f));
	str_autoplay_off.setString("Off");
	str_autoplay_off.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	str_autoplay_off.setPosition(glm::vec3(430.0f, 570.0f, 0.0f));

	str_bpm.setPosition(glm::vec3(0, 45, 0.0f));
	str_genre.setPosition(glm::vec3(0, 90, 0.0f));

	str_autoplay.setFont(mainfont);
	str_autoplay.setPixelSize(pixelSize);
	str_autoplay.setString("AutoPlay(Tab to On or Off): ");
	str_autoplay.setPosition(glm::vec3(0.0f, 570.0f, 0.0f));

	init_future = std::async(std::launch::async, [&](){
		std::list<std::experimental::filesystem::path> path_queue;
		path_queue.push_back(std::experimental::filesystem::current_path());
		std::vector<std::string> str_list;
		auto curr_path = path_queue.begin();
		do
		{
			init_string.setString("search path: " + curr_path->relative_path().string());
			std::experimental::filesystem::directory_iterator dir_it(*curr_path);
			for (auto& path : dir_it)
			{
				if (std::experimental::filesystem::is_directory(path.path()))
				{
					path_queue.push_back(path.path());
				}
				else if (path.path().extension().string() == ".bms"|| path.path().extension().string() == ".bme")
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
			bms_path_list.push_back(path);
		}
		return 0;
	});
}
