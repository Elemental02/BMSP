#include "../stdafx.h"
#include "../managers/ResourceManager.h"
#include "../sfx/sfxSound.h"
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
	for (auto sound : soundpool.playing)
	{
		if (!sound->isPlaying())
		{
			soundpool.pool.push_back(sound);
		}
	}
	soundpool.playing.remove_if([](auto it) {return !it->isPlaying(); });

	bmsPlayer.Update(delta);
	auto& nodes = bmsPlayer.getUpdated();
	for (auto& node : nodes)
	{
		if (node.first > 1 && node.first < 9)
			continue;
		if (soundpool.pool.empty())
		{
			soundpool.pool.push_back(std::shared_ptr<sfx::sfxSound>(new sfx::sfxSound()));
		}
		auto source = soundpool.pool.back();
		soundpool.pool.pop_back();
		if (sounds.find(node.second.value) != sounds.end()) {
			source->setSound(sounds[node.second.value]);
			source->Play();
			soundpool.playing.push_back(source);
		}
	}
}

void PlayScene::Render()
{
}

void PlayScene::Init()
{
	std::string filename = "resource/Fellhound/Fellhound (Original Mix)7.bms";
	std::experimental::filesystem::path path(filename);
	BMSParser parser;
	//bms = parser.Parse(filename);
	bmsPlayer.setBMS(parser.Parse(filename));
	for (auto& wav : bmsPlayer.getBMS().wavs)
	{
		auto dir = path.parent_path();
		auto wav_filename = dir.append(wav.second);
		sounds[wav.first] = ResourceManager::Instance()->LoadSound(wav_filename.string());
	}
}
