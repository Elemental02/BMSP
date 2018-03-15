#pragma once
struct Sprite
{
	std::array<glm::vec3, 6> vertex;
	std::array<glm::vec2, 6> uv;
	glm::uint textureId = 0;

	~Sprite();
};

struct Sound
{
	AVCodecContext* codec;
	AVFormatContext* container;
	SwrContext* swr_ctx;
	int stream_id;

	std::vector<ALuint> buffers;
	bool is_load_complete;

	Sound();
	~Sound();
};

class ResourceManager
{
private:
	ResourceManager() {}
	std::map<std::string, std::shared_ptr<Sprite>> sprites;
	std::map<std::string, std::shared_ptr<Sound>> sounds;
public:
	static ResourceManager* Instance()
	{
		static ResourceManager* instance = nullptr;
		if (instance == nullptr)
			instance = new ResourceManager();
		return instance;
	}
	std::shared_ptr<Sprite> LoadSprite(const std::string& path);
	std::shared_ptr<Sound> LoadSound(const std::string& path);
	void LoadSoundFrame(std::shared_ptr<Sound> sound);
};