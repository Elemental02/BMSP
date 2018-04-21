#pragma once
struct Sprite
{
	glm::uint texture_id = 0;
	glm::vec2 size;
	glm::vec4 texture_rect;
	bool delete_itself = true;
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

	void unload_ffmpeg();
};

struct SpritePackage
{
	glm::uint texture_id = 0;
	glm::vec2 size;
	std::map<std::string, std::shared_ptr<Sprite>> sprite_map;

	~SpritePackage();
};

class ResourceManager
{
private:
	std::map<std::string, std::shared_ptr<Sprite>> sprites;
	std::map<std::string, std::shared_ptr<Sound>> sounds;
	std::map<std::string, std::shared_ptr<SpritePackage>> sprite_packs;
public:
	ResourceManager() {}
	std::shared_ptr<Sprite> LoadSprite(const std::string& path);
	std::shared_ptr<Sound> LoadSound(const std::string& path);
	std::shared_ptr<SpritePackage> LoadSpritePackage(const std::string& path);
	void LoadSoundFrame(std::shared_ptr<Sound> sound, int framesize = 5);

	void UnloadSprite(const std::string& path);
	void UnloadSound(const std::string& path);
	void UnloadSprite(std::shared_ptr<Sprite> sprite);
	void UnloadSound(std::shared_ptr<Sound> sound);
};