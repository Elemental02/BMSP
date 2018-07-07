#pragma once
#include "../resource/Sprite.h"
#include "../resource/SpritePackage.h"
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

struct Image
{
	int height = 0, width = 0;
	uint8_t* data = nullptr;

	~Image();
};

struct ImageStream
{
	AVCodecContext* codec;
	AVFormatContext* container;
	SwsContext* sws_ctx;
	int stream_id;

	bool is_load_complete = false;
	float frame_rate;

	void unload_ffmpeg();

	std::array<Image, 50> image_buffer;
};

class ResourceManager
{
private:
	std::map<std::string, std::shared_ptr<Sprite>> sprites;
	std::map<std::string, std::shared_ptr<Sound>> sounds;
	std::map<std::string, std::shared_ptr<SpritePackage>> sprite_packs;

	struct ffmpeg_stream
	{
		AVMediaType media_type;
		AVCodecContext* ctx;
		AVFormatContext* container;
		SwsContext* sws_ctx;
		SwrContext* swr_ctx;
		int stream_id;

		bool is_load_complete = false;
		
		int open_media_stream(std::string path, AVMediaType media_type);
		void set_sws();
		void set_swr();

		void read_image_frame();
		void read_audio_frame();

		void unload_ffmpeg();
		~ffmpeg_stream();
	};
public:
	ResourceManager() {}
	std::shared_ptr<Sprite> LoadSprite(const std::string& path);
	std::shared_ptr<Sprite> LoadSprite(const std::string& name, Image& imgdata);
	std::shared_ptr<Sprite> LoadSprite(Image& imgdata);
	std::shared_ptr<ImageStream> OpenImageStream(const std::string& name);
	std::shared_ptr<Sound> LoadSound(const std::string& path);
	std::shared_ptr<SpritePackage> LoadSpritePackage(const std::string& path);
	int LoadSoundFrame(std::shared_ptr<Sound> sound, int framesize = 5);
	int LoadImageFrame(std::shared_ptr<ImageStream> image, int framesize = 0);

	void UnloadSprite(const std::string& path);
	void UnloadSound(const std::string& path);
	void UnloadSprite(std::shared_ptr<Sprite> sprite);
	void UnloadSound(std::shared_ptr<Sound> sound);
};