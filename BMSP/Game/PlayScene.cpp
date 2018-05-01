#include "../stdafx.h"
#include "../managers/GlobalManager.h"
#include "PlayScene.h"
#include "../BMS/BMSParser.h"
#include <GLFW/glfw3.h>

void PlayScene::SoundPlay(int value)
{
	if (sounds.find(value) != sounds.end()) {
		if (sounds[value])
		{
			if (soundpool.pool.empty())
			{
				soundpool.pool.push_back(std::shared_ptr<sfx::sfxSound>(new sfx::sfxSound()));
			}
			auto source = soundpool.pool.back();
			soundpool.pool.pop_back();
			source->setSound(sounds[value]);
			source->Play();
			soundpool.playing.push_back(source);
		}
	}
}

PlayScene::PlayScene(): gfxScene()
{
}

PlayScene::~PlayScene()
{
	for (auto& it : sounds)
	{
		IResourceManager->UnloadSound(it.second);
	}

	for (auto& it : bga_sprites)
	{
		IResourceManager->UnloadSprite(it.second);
	}
}

void PlayScene::Update(std::chrono::milliseconds delta)
{
	gfx::gfxScene::Update(delta);
	for (auto sound : soundpool.playing)
	{
		if (!sound->isPlaying())
		{
			soundpool.pool.push_back(sound);
		}
	}
	soundpool.playing.remove_if([](auto it) {return !it->isPlaying(); });

	switch (scene_state)
	{
	case SceneState::Initialzing:
	{
		std::shared_ptr<gfx::gfxUpdatableFunc> func = std::shared_ptr<gfx::gfxUpdatableFunc>(new gfx::gfxUpdatableFunc);
		std::shared_ptr<gfx::gfxUpdatableFunc> func2 = std::shared_ptr<gfx::gfxUpdatableFunc>(new gfx::gfxUpdatableFunc);
		func->setFunc([&, func, func2](auto delta) {
			if (func)
			{
				bool res = true;
				int elasped = func->getElasped();
				if (elasped > 500)
				{
					elasped = 500;
					res = false;
				}
				float pos = (elasped / 500.0f) - 1.0f;
				pos = pos * pos;
				pos = 1.0f - pos;
				float width_ratio = 800.0f / 640.0f;
				float height_ratio = 600.0f / 480.0f;
				glm::vec3 bottom_pos = glm::vec3(0.0f, 600.0f - skin_bottom->getSprite().size.y*pos, 0.0f);
				skin_bottom->setPosition(bottom_pos);
				skin_top->setPosition(glm::vec3(0.0f, -skin_top->getSprite().size.y*(1.0f - pos), 0.0f));

				
				maxcombo_sprite->setPosition(glm::vec3(bottom_pos.x+292.0f*width_ratio, bottom_pos.y+5.0f*height_ratio, 0.0f));
				bpm_sprite->setPosition(glm::vec3(bottom_pos.x + 388.0f*width_ratio, bottom_pos.y + 5.0f*height_ratio, 0.0f));
				minute_sprite->setPosition(glm::vec3(bottom_pos.x + 600.0f*width_ratio, bottom_pos.y + 28.0f*height_ratio, 0.0f));
				second_sprite->setPosition(glm::vec3(bottom_pos.x + 632.0f*width_ratio, bottom_pos.y + 28.0f*height_ratio, 0.0f));
				play_sprite->setPosition(glm::vec3(bottom_pos.x + 488.0f*width_ratio, bottom_pos.y + 5.0f*height_ratio, 0.0f));
				gauge_sprite->setPosition(glm::vec3(bottom_pos.x + 508.0f*width_ratio, bottom_pos.y + 28.0f*height_ratio, 0.0f));
				point_sprite->setPosition(glm::vec3(bottom_pos.x + 139.0f*width_ratio, bottom_pos.y + 28.0f*height_ratio, 0.0f));
				score_sprite->setPosition(glm::vec3(bottom_pos.x + 153.0f*width_ratio, bottom_pos.y + 5.0f*height_ratio, 0.0f));
				if (res == false)
				{
					AddUpdatable(func2);
					return false;
				}
				return true;
			}
			return false;
		});

		func2->setFunc([&, func2](auto delta) {
			int elasped = func2->getElasped() - 500;
			bool res = true;
			if (elasped > 1000)
			{
				res = false;
				elasped = 1000;
				scene_state = SceneState::Loading;
			}
			else if (elasped < 0)
			{
				elasped = 0;
			}
			float pos = (elasped / 1000.0f) - 1.0f;
			pos = pos * pos;
			pos = 1.0f - pos;
			for (int i = 0; i < 8; i++)
			{
				button_sprite[i]->setPosition(glm::vec3(button_sprite[i]->getPosition().x, 600.0f - (skin_bottom->getSprite().size.y + button_sprite[i]->getSprite().size.y)*pos, 0.0f));
				if (i < 7) {
					line_sprite[i]->setPosition(glm::vec3(line_sprite[i]->getPosition().x, skin_top->getSprite().size.y + 600.0f*(1.0f - pos), 0.0f));
				}
			}
			skin_left->setPosition(glm::vec3(0.0f, skin_top->getSprite().size.y - 600.0f*(1.0f - pos), 0.0f));
			skin_right->setPosition(glm::vec3(skin_right->getPosition().x, skin_top->getSprite().size.y - 600.0f*(1.0f - pos), 0.0f));
			lane->setPosition(glm::vec3(skin_left->getSprite().size.x, skin_top->getSprite().size.y - 600.0f*(1.0f - pos), 0.0f));
			return res;
		});

		AddUpdatable(func);
		scene_state = SceneState::Playing_Animation;
	}
	break;
	case SceneState::Loading:
	{
		if (init_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
		{
			std::experimental::filesystem::path path(path_to_load);
			std::array<std::string, 6> image_extension = { "bmp","png","jpg","jpeg","tga","tiff" };
			for (auto& bmp : bmsPlayer.getBMS().bmps)
			{
				auto dir = path.parent_path();
				auto bmp_filename = dir.string() + "\\" + bmp.second;
				bool is_image = false;
				for (int i = 0; i < image_extension.size(); i++)
				{
					auto extension = bmp_filename.substr(bmp_filename.size() - image_extension[i].size(), image_extension[i].size());
					if (image_extension[i] == extension) 
					{
						is_image = true;
						break;
					}
				}

				if (is_image)
				{
					auto bmp_ptr = IResourceManager->LoadSprite(bmp_filename);
					for (int i = 0; i < image_extension.size(); i++)
					{
						if (bmp_ptr)
							break;
						bmp_filename.replace(bmp_filename.size() - image_extension[i].size(), image_extension[i].size(), image_extension[i]);
					}
					bga_sprites[bmp.first] = bmp_ptr;
				}
				else
				{
					auto image_stream = IResourceManager->OpenImageStream(bmp_filename);
					if (image_stream && !bgaVideo)
					{
						this->bgaVideo = std::shared_ptr<gfx::gfxVideo>(new gfx::gfxVideo);
						video_value = bmp.first;
						bgaVideo->setVideo(image_stream);
					}
				}
			}
			if (bgaVideo)
			{
				bgaVideo->setScale(glm::vec3(800.0f / bgaVideo->getImageSize().x, 600.0f / bgaVideo->getImageSize().y, 1.0f));
				if(video_value == 0)
					AddUpdatable(bgaVideo);
			}
			scene_state = SceneState::Playing;
		}
	}
	break;
	case SceneState::Playing:
	{
		auto& bms = bmsPlayer.getBMS();
		bmsPlayer.Update();
		auto curr_measure = bmsPlayer.getCurrentMeasure();
		auto curr_progress = bmsPlayer.getCurrentPosition();
		auto curr_time = bmsPlayer.getCurrentTime().count();
		while ((processed_progress - curr_progress)*noteSpeed < 800.0f && (processed_measure + 1) < bms.measures.size())
		{
			processed_measure++;
			processed_progress = bms.measures[processed_measure].position + bms.measures[processed_measure].length;
			auto& measure = bms.measures[processed_measure];
			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					int channel = i + (j ? BMS::CH::P1L : BMS::CH::P1);
					auto& nodes = measure.nodes.find(channel);
					if (nodes == measure.nodes.end())
						continue;
					for (auto& node : nodes->second)
					{
						if (node_pool.pool.empty())
						{
							node_pool.pool.push_back(NodeSprite{ std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite),nullptr });
						}
						auto& sprite_node = node_pool.pool.back();
						sprite_node.node = &node;

						int lane_num = (i == 5) ? 0 : ((i < 5) ? i + 1 : i - 1);
						int note_num = (lane_num == 0) ? 2 : (lane_num % 2) == 0 ? 1 : 0;
						//std::string button_index = "note"+std::to_string(note_num)+"_0";
						sprite_node.sprite->setSprite(noteAnimation[note_num][0]);
						sprite_node.sprite->setPosition(glm::vec3(button_sprite[lane_num]->getPosition().x, 0.0f, 0.0f));
						sprite_node.sprite->setPivot(glm::vec3(0.0f, 1.0f, 0.0f));
						sprite_node.lane_index = lane_num;

						node_panel.addSprite(sprite_node.sprite.get());
						node_pool.playing.push_back(sprite_node);
						node_pool.pool.pop_back();
					}
				}
			}
		}

		float lane_pos = lane->getSprite().size.y + skin_top->getSprite().size.y;
		int note_index = (curr_time * 60 / 1000 / 10) % 4;
		for (auto& sprite_node : node_pool.playing)
		{
			if (sprite_node.node == nullptr)
				continue;
			auto pos = sprite_node.sprite->getPosition();
			double ypos = sprite_node.node->position - curr_progress;
			lane_info[sprite_node.lane_index].closest_node = (lane_info[sprite_node.lane_index].closest_node == nullptr) || (lane_info[sprite_node.lane_index].closest_node->node == nullptr) || (abs(ypos) < abs(lane_info[sprite_node.lane_index].closest_node->node->position - curr_progress)) ? &(sprite_node) : lane_info[sprite_node.lane_index].closest_node;
			lane_info[sprite_node.lane_index].sound_value = lane_info[sprite_node.lane_index].closest_node->node->value;
			pos.y = lane_pos - (ypos)* noteSpeed;
			sprite_node.sprite->setPosition(pos);
			
			int note_num = (sprite_node.lane_index == 0) ? 2 : (sprite_node.lane_index % 2) == 0 ? 1 : 0;
			sprite_node.sprite->setSprite(noteAnimation[note_num][note_index]);

			auto judge = curr_time - sprite_node.node->a_time.count();
			if (judge > 200)
			{
				sprite_node.node = nullptr;
				curr_judge = judgeAnimation[4];
				curr_judge->Rewind(4);
				judge_duration = 0;
				combo = 0;
				AddUpdatable(curr_judge);
				AddUpdatable(judge_func);
			}
		}

		for (int i = 0; i < 8; i++)
		{
			auto state = IInputManager->getKeyState(i);
			int button_num = (i == 0) ? 2 : (i % 2) == 0 ? 1 : 0;
			bool playsound = false;
			int judge_state = -1;
			if (lane_info[i].closest_node != nullptr && lane_info[i].closest_node->node != nullptr)
			{
				auto judge = (curr_time - lane_info[i].closest_node->node->a_time.count());
				if (autoplay && judge >= 0)
				{
					playsound = true;
					judge_state = 0;
					combo++;
				}
				else if (!autoplay && state == KeyState::State_Press)
				{
					judge = std::abs(judge);
					if (judge < 20)
					{
						playsound = true;
						judge_state = 0;
						combo++;
					}
					else if (judge < 40)
					{
						playsound = true;
						judge_state = 1;
						combo++;
					}
					else if (judge < 100)
					{
						playsound = true;
						judge_state = 2;
						combo++;
					}
					else if (judge < 200)
					{
						playsound = true;
						judge_state = 3;
						combo = 0;
					}
				}
			}

			if (judge_state >= 0)
			{
				curr_judge = judgeAnimation[judge_state];
				curr_judge->Rewind(4);
				judge_duration = 0;
				note_effect[i][0]->Rewind();
				note_effect[i][1]->Rewind();
				AddUpdatable(curr_judge);
				AddUpdatable(judge_func);
				AddUpdatable(note_effect[i][0]);
				AddUpdatable(note_effect[i][1]);
				lane_info[i].closest_node->node = nullptr;
			}

			if(playsound && lane_info[i].sound_value != -1)
				SoundPlay(lane_info[i].sound_value);

			lane_info[i].closest_node = nullptr;
		}

		node_pool.playing.remove_if([&](auto& it) {
			if (it.node == nullptr)
			{
				node_pool.pool.push_back(it);
				node_panel.RemoveSprite(it.sprite.get());
				return true;
			}
			return false;
		});

		auto& nodes = bmsPlayer.getUpdated();
		for (auto& node : nodes)
		{
			if (node.first == 1)
				SoundPlay(node.second.value);
			else if (node.first == BMS::CH::BGA || node.first == BMS::CH::BGALayer)
			{
				if (video_value && node.second.value == video_value)
				{
					AddUpdatable(bgaVideo);
					bgaVideo->Update(std::chrono::milliseconds(curr_time - node.second.a_time.count()));
				}
				else
				{
					auto& sprite = bga_sprites[node.second.value];
					if (sprite != nullptr)
					{
						auto bga = node.first == BMS::CH::BGA ? bgaSprite : bgaLayerSprite;
						bga->setSprite(sprite);
						bga->setScale(glm::vec3(800.0f / sprite->size.x, 600.0f / sprite->size.y, 0.0f));
					}
				}
			}
		}
		if (bmsPlayer.IsComplete())
		{
			scene_state = SceneState::End;
		}

		if (maxcombo < combo)
		{
			maxcombo = combo;
			maxcombo_sprite->setString(std::to_string(maxcombo));
		}
		if (curr_bpm != (int)bmsPlayer.getCurrentBPM())
		{
			curr_bpm = (int)bmsPlayer.getCurrentBPM();
			bpm_sprite->setString(std::to_string(curr_bpm));
		}
		int curr_minute = curr_time / 1000 / 60;
		if (curr_minute != minute)
		{
			minute = curr_minute;
			minute_sprite->setString(std::to_string(minute));
		}
		int curr_second = (curr_time / 1000) % 60;
		if (curr_second != second)
		{
			second = curr_second;
			second_sprite->setString(second < 10 ? "0" + std::to_string(second) : std::to_string(second));
		}
		int curr_play = (curr_progress / (bms.measures.back().length + bms.measures.back().position)) * 100;
		if (curr_play != play_percent)
		{
			play_percent = curr_play;
			play_sprite->setString(std::to_string(play_percent));
		}
		//gauge_sprite->setString("0");
		//point_sprite->setString("0");
		//score_sprite->setString("0");
		break;
	}
	case SceneState::End:
	{

	}
	break;
	}

	if (IInputManager->getKeyState(KeyIndex::Up) == KeyState::State_Press)
	{
		noteSpeed += 50;
	}

	if (IInputManager->getKeyState(KeyIndex::Down) == KeyState::State_Press && noteSpeed > 50)
	{
		noteSpeed -= 50;
	}

	for (int i = 0; i < 8; i++)
	{
		auto state = IInputManager->getKeyState(i);
		int button_num = (i == 0) ? 2 : (i % 2) == 0 ? 1 : 0;
		if (state == KeyState::State_Press)
		{
			std::string sprite_name = "button" + std::to_string(button_num) + "_1";
			button_sprite[i]->setSprite(sprite_pack2->sprite_map[sprite_name]);
		}
		else if (state == KeyState::State_Up)
		{
			std::string sprite_name = "button" + std::to_string(button_num) + "_0";
			button_sprite[i]->setSprite(sprite_pack2->sprite_map[sprite_name]);
		}
		lane_info[i].closest_node = nullptr;

		auto p = lane_back[i]->getPosition();
		float base_y = button_sprite[i]->getPosition().y - lane_back[i]->getSprite().size.y;
		if (state == KeyState::State_Press || state == KeyState::State_Down)
		{
			float y = p.y - delta.count() * 3.0f - base_y;
			if (y < 0)
				y = 0.0f;
			lane_back[i]->setPosition(glm::vec3(p.x, base_y + y, p.z));
		}
		else
		{
			float y = p.y + delta.count() * 3.0f - base_y;
			if (y > lane_back[i]->getSprite().size.y)
				y = lane_back[i]->getSprite().size.y;
			lane_back[i]->setPosition(glm::vec3(p.x, base_y + y, p.z));
		}
	}

	if (curr_judge && !HasUpdatable(curr_judge))
		curr_judge.reset();

	if (IInputManager->getKeyState(KeyIndex::ESC) == KeyState::State_Press)
	{
		for (auto& it : soundpool.playing)
		{
			it->Stop();
		}
		IGlobalManager->Pop_Scene();
	}
}

void PlayScene::Render()
{
	if (bgaVideo && this->HasUpdatable(bgaVideo))
		bgaVideo->Render();
	bgaSprite->Render();
	bgaLayerSprite->Render();
	black_sprite->Render();
	back_skin_panel.Render();
	node_panel.Render();
	front_skin_panel.Render();
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 2; j++)
			if(this->HasUpdatable(note_effect[i][j]))
				note_effect[i][j]->Render();

	maxcombo_sprite->Render();
	bpm_sprite->Render();
	minute_sprite->Render();
	second_sprite->Render();
	play_sprite->Render();
	gauge_sprite->Render();
	point_sprite->Render();
	score_sprite->Render();

	if (curr_judge)
	{
		if (combo)
		{
			combo_max->Render();
			combo_sprite->setString(std::to_string(combo));
			combo_sprite->Render();
		}
		curr_judge->Render();
	}
}

void PlayScene::Init()
{
	BMSParser parser;
	init_future = std::async(std::launch::async, [&]() {
		std::experimental::filesystem::path path(path_to_load);
		bmsPlayer.setBMS(parser.Parse(path_to_load));
		auto& bms = bmsPlayer.getBMS();
		auto dir = path.parent_path();
		for (auto& wav : bms.wavs)
		{
			auto wav_filename = dir.string() + "\\" + wav.second;
			auto wavptr = IResourceManager->LoadSound(wav_filename);
			if (!wavptr)
			{
				wav_filename.replace(wav_filename.size() - 3, 3, "mp3");
				wavptr = IResourceManager->LoadSound(wav_filename);
			}
			if (!wavptr)
			{
				wav_filename.replace(wav_filename.size() - 3, 3, "ogg");
				wavptr = IResourceManager->LoadSound(wav_filename);
			}
			if (!wavptr)
				std::cout << "load failed?: " << wav.second <<std::endl;
			sounds[wav.first] = wavptr;
		}
		if (bms.metadata.find("VIDEOFILE") != bms.metadata.end())
		{
			this->bgaVideo = std::shared_ptr<gfx::gfxVideo>(new gfx::gfxVideo);
			std::string video_filename = (dir.string() + "\\").append(bms.metadata.find("VIDEOFILE")->second);
			bgaVideo->setVideo(video_filename);
		}
		return 0;
	});

	for (int i = 0; i < 8; i++)
	{
		lane_info[i].closest_node = 0;
	}
	lane_info[0].lane_value = BMS::CH::P1+5;
	lane_info[1].lane_value = BMS::CH::P1;
	lane_info[2].lane_value = BMS::CH::P1+1;
	lane_info[3].lane_value = BMS::CH::P1+2;
	lane_info[4].lane_value = BMS::CH::P1+3;
	lane_info[5].lane_value = BMS::CH::P1+4;
	lane_info[6].lane_value = BMS::CH::P1+7;
	lane_info[7].lane_value = BMS::CH::P1+8;

	bgaSprite = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	bgaLayerSprite = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	bgaLayerSprite->setShader("layer");

	sprite_pack2 = IResourceManager->LoadSpritePackage("skin2");
	sprite_pack1 = IResourceManager->LoadSpritePackage("skin1");
	skin_top = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	skin_top->setSprite(sprite_pack1->sprite_map["top"]);
	skin_top->setPosition(glm::vec3(0.0f, -skin_top->getSprite().size.y, 0.0f));

	skin_bottom = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	skin_bottom->setSprite(sprite_pack1->sprite_map["bottom"]);
	skin_bottom->setPosition(glm::vec3(0.0f, 600.0f, 0.0f));

	skin_left = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	skin_left->setSprite(sprite_pack2->sprite_map["left"]);
	skin_left->setPosition(glm::vec3(0.0f, 600.0f, 0.0f));

	float size_ratio = 800.0f / 640.0f;
	float lane_length = 33 + 15 * 3 + 19 * 4;
	lane_length *= size_ratio;

	int lanepos = skin_left->getSprite().size.x;
	for (int i = 0; i < 8; i++)
	{
		int button_num = (i == 0) ? 2 : (i % 2) == 0 ? 1 : 0;
		std::string sprite_name = "button" + std::to_string(button_num) + "_0";
		button_sprite[i] = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
		button_sprite[i]->setSprite(sprite_pack2->sprite_map[sprite_name]);
		button_sprite[i]->setPosition(glm::vec3(lanepos, 600.0f, 0.0f));

		lane_back[i] = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
		lane_back[i]->setSprite(sprite_pack2->sprite_map["lane"+std::to_string(button_num)]);
		lane_back[i]->setPosition(glm::vec3(lanepos, 600.0f, 0.0f));
		back_skin_panel.addSprite(lane_back[i].get());

		for (int j = 0; j < 2; j++)
		{
			note_effect[i][j] = std::shared_ptr<gfx::gfxSpriteAnimation>(new gfx::gfxSpriteAnimation);
			note_effect[i][j]->setPivot(glm::vec3(0.5f, 0.5f, 0.0f));
			note_effect[i][j]->setPosition(glm::vec3(lanepos + (button_sprite[i]->getSprite().size.x) / 2.0f, skin_top->getSprite().size.y + sprite_pack2->sprite_map["lane"]->size.y - 7.5f, 0.0f));
			note_effect[i][j]->setShader("layer");
			for (int k = 0; k < 3; k++)
				note_effect[i][j]->addLastFrame(sprite_pack1->sprite_map["effect_" + std::to_string(j + k * 2)]);
			note_effect[i][j]->setDuration(30);
		}

		lanepos += button_sprite[i]->getSprite().size.x;
		front_skin_panel.addSprite(button_sprite[i].get());
		if (i < 7) {
			line_sprite[i] = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
			line_sprite[i]->setSprite(sprite_pack2->sprite_map["line1"]);
			line_sprite[i]->setPosition(glm::vec3(lanepos, 600.0f, 0.0f));
			front_skin_panel.addSprite(line_sprite[i].get());
		}
	}

	skin_right = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	skin_right->setSprite(sprite_pack2->sprite_map["right"]);
	sprite_pack2->sprite_map["right"]->size.y = 600 - skin_top->getSprite().size.y - skin_bottom->getSprite().size.y;
	sprite_pack2->sprite_map["left"]->size.y = 600 - skin_top->getSprite().size.y - skin_bottom->getSprite().size.y;
	skin_right->setPosition(glm::vec3(lanepos, 600.0f, 0.0f));

	lane = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	lane->setSprite(sprite_pack2->sprite_map["lane"]);
	lane->setPosition(glm::vec3(skin_left->getSprite().size.x, 600.0f, 0.0f));
	lane->setScale(glm::vec3(lanepos - skin_left->getSprite().size.x, 1.0f, 1.0f));

	front_skin_panel.addSprite(skin_right.get());
	front_skin_panel.addSprite(skin_left.get());

	back_skin_panel.addSprite(lane.get());

	black_sprite = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	black_sprite->setSprite(sprite_pack2->sprite_map["black"]);
	black_sprite->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));
	black_sprite->setScale(glm::vec3(lanepos, lane->getSprite().size.y, 1.0f));
	black_sprite->setPosition(glm::vec3(skin_left->getSprite().size.x, skin_top->getSprite().size.y, 0.0f));

	front_skin_panel.addSprite(skin_top.get());
	front_skin_panel.addSprite(skin_bottom.get());

	std::string judgestr[5] = { "cool_" ,"great_" ,"good_" ,"bad_" ,"miss_" };
	for (int i = 0; i < 5; i++)
	{
		judgeAnimation[i] = std::shared_ptr<gfx::gfxSpriteAnimation>(new gfx::gfxSpriteAnimation);
		for (int j = 0; j < 4; j++)
		{
			std::string name = judgestr[i] + std::to_string(j);
			judgeAnimation[i]->addLastFrame(sprite_pack1->sprite_map[name]);
		}
		judgeAnimation[i]->setPivot(glm::vec3(0.5f, 0.5f, 0.0f));
		judgeAnimation[i]->setPosition(glm::vec3((skin_right->getPosition().x - skin_left->getSprite().size.x) / 2.0f + skin_left->getSprite().size.x, 380.0f, 0.0f));
		judgeAnimation[i]->setDuration(30);
		judgeAnimation[i]->setShader("layer");
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::string note_name = "note" + std::to_string(i) +"_" +std::to_string(j);
			noteAnimation[i][j] = sprite_pack2->sprite_map[note_name];
		}
	}

	judge_func = std::shared_ptr<gfx::gfxUpdatableFunc>(new gfx::gfxUpdatableFunc);
	judge_func->setFunc([&](auto delta) {
		if (curr_judge)
		{
			judge_duration += delta.count();
			if (judge_duration >= 100)
			{
				judge_duration = 0;
				return false;
			}
			float pos = (judge_duration / 100.0f) - 1.0f;
			pos = 1.0f - pos * pos;
			curr_judge->setScale(glm::vec3(pos, pos, 1.0f));
			return true;
		}
		return false;
	});

	combo_max = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	combo_max->setSprite(sprite_pack1->sprite_map["max"]);
	combo_max->setPosition(glm::vec3((skin_right->getPosition().x - skin_left->getSprite().size.x) / 2.0f + skin_left->getSprite().size.x, 270.0f, 0.0f));
	combo_max->setPivot(glm::vec3(0.5f, 0.5f, 0.5f));
	combo_max->setShader("layer");

	combo_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	maxcombo_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	bpm_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	minute_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	second_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	play_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	gauge_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	point_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);
	score_sprite = std::shared_ptr<gfx::gfxSpriteFont>(new gfx::gfxSpriteFont);

	for(int i=0;i<10;i++)
	{
		std::string str_l = std::to_string(i) + "_l";
		std::string str_s = std::to_string(i) + "_s";
		combo_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_l]);
		maxcombo_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
		bpm_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
		minute_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
		second_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
		play_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
		gauge_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
		point_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
		score_sprite->addSprite('0' + i, sprite_pack1->sprite_map[str_s]);
	}
	maxcombo_sprite->setAlign(gfx::Align::Right);
	bpm_sprite->setAlign(gfx::Align::Right);
	minute_sprite->setAlign(gfx::Align::Right);
	second_sprite->setAlign(gfx::Align::Right);
	play_sprite->setAlign(gfx::Align::Right);
	gauge_sprite->setAlign(gfx::Align::Right);
	point_sprite->setAlign(gfx::Align::Right);
	score_sprite->setAlign(gfx::Align::Right);

	maxcombo_sprite->setShader("layer");
	bpm_sprite->setShader("layer");
	minute_sprite->setShader("layer");
	second_sprite->setShader("layer");
	play_sprite->setShader("layer");
	gauge_sprite->setShader("layer");
	point_sprite->setShader("layer");
	score_sprite->setShader("layer");

	maxcombo_sprite->setString("0");
	bpm_sprite->setString("0");
	minute_sprite->setString("0");
	second_sprite->setString("00");
	play_sprite->setString("0");
	gauge_sprite->setString("0");
	point_sprite->setString("0");
	score_sprite->setString("0");

	combo_sprite->setShader("layer");
	combo_sprite->setPosition(glm::vec3((skin_right->getPosition().x - skin_left->getSprite().size.x) / 2.0f + skin_left->getSprite().size.x, 300.0f, 0.0f));
	combo_sprite->setAlign(gfx::Align::Center);
	front_skin_panel.setShader("layer");
	back_skin_panel.setShader("layer");

	autoplay = false;
}

void PlayScene::SetBMSPath(std::string path)
{
	path_to_load = path;
}
