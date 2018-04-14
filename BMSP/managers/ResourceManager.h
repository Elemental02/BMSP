#pragma once
struct Sprite
{
	glm::uint texture_id = 0;
	glm::vec2 size;
	glm::vec4 texture_rect;
	~Sprite();
};

struct Sound
{
	AVCodecContext* codec;
	AVFormatContext* container;
	FILE* file;
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
	std::map<std::string, std::shared_ptr<Sprite>> sprites;
	std::map<std::string, std::shared_ptr<Sound>> sounds;
public:
	ResourceManager() {}
	std::shared_ptr<Sprite> LoadSprite(const std::string& path);
	std::shared_ptr<Sound> LoadSound(const std::string& path);
	void LoadSoundFrame(std::shared_ptr<Sound> sound);
};