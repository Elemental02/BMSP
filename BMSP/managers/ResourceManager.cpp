#include "../stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "ResourceManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLuint loadSprite_ffmpeg(const char * imagepath, unsigned int& height, unsigned int& width)
{
	AVFormatContext* container = avformat_alloc_context();
	if (avformat_open_input(&container, imagepath, NULL, NULL) < 0) {
		return 0;
	}

	if (avformat_find_stream_info(container, nullptr) < 0) {
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}

	AVCodec *codec = nullptr;
	int stream_id = av_find_best_stream(container, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	if (stream_id == -1) {
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}

	AVCodecParameters *ctxp = container->streams[stream_id]->codecpar;
	if (codec == nullptr) {
		codec = avcodec_find_decoder(ctxp->codec_id);
	}
	AVCodecContext* ctx = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(ctx, ctxp);

	height = ctx->height;
	width = ctx->width;
	
	int height_conv = height;
	int width_conv = width;

	if (codec == NULL) {
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}

	int res = avcodec_open2(ctx, codec, nullptr);
	if (res < 0) {
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}
	AVPixelFormat pixFormat;
	switch (ctx->pix_fmt) {
	case AV_PIX_FMT_YUVJ420P:
		pixFormat = AV_PIX_FMT_YUV420P;
		break;
	case AV_PIX_FMT_YUVJ422P:
		pixFormat = AV_PIX_FMT_YUV422P;
		break;
	case AV_PIX_FMT_YUVJ444P:
		pixFormat = AV_PIX_FMT_YUV444P;
		break;
	case AV_PIX_FMT_YUVJ440P:
		pixFormat = AV_PIX_FMT_YUV440P;
		break;
	default:
		pixFormat = ctx->pix_fmt;
		break;
	}

	struct SwsContext *sws_ctx = sws_getContext(ctx->width, ctx->height, pixFormat,
		width_conv, height_conv, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);

	if (!sws_ctx) {
		fprintf(stderr, "Could not allocate resampler context\n");
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}

	AVPacket packet;
	av_init_packet(&packet);

	GLuint textureID = 0;

	while (av_read_frame(container, &packet) >= 0)
	{
		if (packet.stream_index != stream_id) {
			av_packet_unref(&packet);
			continue;
		}

		AVFrame *frame = av_frame_alloc();
		int len = avcodec_send_packet(ctx, &packet);
		int frameFinished = avcodec_receive_frame(ctx, frame);
		if (frameFinished == 0)
		{
			AVFrame *frameRGBA = av_frame_alloc();
			av_image_alloc(frameRGBA->data, frameRGBA->linesize, width_conv, height_conv, AV_PIX_FMT_RGBA, 1);
			sws_scale(sws_ctx, &frame->data[0], frame->linesize, 0, ctx->height, &frameRGBA->data[0], frameRGBA->linesize);
			
			glGenTextures(1, &textureID);

			glBindTexture(GL_TEXTURE_2D, textureID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_conv, height_conv, 0, GL_RGBA, GL_UNSIGNED_BYTE, frameRGBA->data[0]);
			auto err = glGetError();
			av_freep(&frameRGBA->data[0]);
			av_frame_free(&frameRGBA);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		av_packet_unref(&packet);
		av_frame_unref(frame);
		av_frame_free(&frame);
		if (textureID != 0)
			break;
	}
	sws_freeContext(sws_ctx);

	avcodec_free_context(&ctx);
	avformat_close_input(&container);
	avformat_free_context(container);

	return textureID;
}

std::shared_ptr<Sprite> ResourceManager::LoadSprite(const std::string& path)
{
	if (sprites.find(path) != sprites.end())
		return sprites[path];

	unsigned int height, width, texture_id;
	texture_id = loadSprite_ffmpeg(path.c_str(), height, width);
	if (texture_id == 0)
		return nullptr;

	std::shared_ptr<Sprite> res = std::shared_ptr<Sprite>(
		new Sprite(texture_id, glm::vec2(width, height), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
	sprites[path] = res;
	return res;
}

std::shared_ptr<Sprite> ResourceManager::LoadSprite(const std::string & name, Image& imgdata)
{
	if (sprites.find(name) != sprites.end())
		return 0;

	std::shared_ptr<Sprite> res = LoadSprite(imgdata);
	sprites[name] = res;
	return res;
}

std::shared_ptr<Sprite> ResourceManager::LoadSprite(Image& imgdata)
{

	if (imgdata.data == nullptr)
		return 0;
	
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgdata.width, imgdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata.data);
	auto err = glGetError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	std::shared_ptr<Sprite> res = std::shared_ptr<Sprite>(
		new Sprite(texture_id, glm::vec2(imgdata.width, imgdata.height), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
	
	return res;
}

std::shared_ptr<ImageStream> ResourceManager::OpenImageStream(const std::string & path)
{
	AVFormatContext* container = avformat_alloc_context();
	if (avformat_open_input(&container, path.c_str(), NULL, NULL) < 0) {
		return 0;
	}

	if (avformat_find_stream_info(container, nullptr) < 0) {
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}

	AVCodec *codec = nullptr;
	int stream_id = av_find_best_stream(container, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	if (stream_id == -1) {
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}

	AVCodecParameters *ctxp = container->streams[stream_id]->codecpar;
	if (codec == nullptr) {
		codec = avcodec_find_decoder(ctxp->codec_id);
	}
	AVCodecContext* ctx = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(ctx, ctxp);

	int height_conv = ctx->height;
	int width_conv = ctx->width;

	if (codec == NULL) {
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}

	int res = avcodec_open2(ctx, codec, nullptr);
	if (res < 0) {
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}
	AVPixelFormat pixFormat;
	switch (ctx->pix_fmt) {
	case AV_PIX_FMT_YUVJ420P:
		pixFormat = AV_PIX_FMT_YUV420P;
		break;
	case AV_PIX_FMT_YUVJ422P:
		pixFormat = AV_PIX_FMT_YUV422P;
		break;
	case AV_PIX_FMT_YUVJ444P:
		pixFormat = AV_PIX_FMT_YUV444P;
		break;
	case AV_PIX_FMT_YUVJ440P:
		pixFormat = AV_PIX_FMT_YUV440P;
		break;
	default:
		pixFormat = ctx->pix_fmt;
		break;
	}

	struct SwsContext *sws_ctx = sws_getContext(ctx->width, ctx->height, pixFormat,
		width_conv, height_conv, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);

	if (!sws_ctx) {
		fprintf(stderr, "Could not allocate resampler context\n");
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return 0;
	}
	auto imagestream = std::shared_ptr<ImageStream>(new ImageStream);
	imagestream->codec = ctx;
	imagestream->container = container;
	imagestream->sws_ctx = sws_ctx;

	imagestream->stream_id = stream_id;

	LoadImageFrame(imagestream);

	return imagestream;
}

std::shared_ptr<Sound> ResourceManager::LoadSound(const std::string& path)
{
	if (sounds.find(path) != sounds.end())
		return sounds[path];
	AVFormatContext* container = avformat_alloc_context();
	if (avformat_open_input(&container, path.c_str(), NULL, NULL) < 0) {
		avformat_free_context(container);
		return nullptr;
	}

	if (avformat_find_stream_info(container, nullptr) < 0) {
		avformat_close_input(&container);
		avformat_free_context(container);
		return nullptr;
	}

	AVCodec *codec = nullptr;
	int stream_id = av_find_best_stream(container, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
	if (stream_id == -1) {
		avformat_close_input(&container);
		avformat_free_context(container);
		return nullptr;
	}

	AVCodecParameters *ctxp = container->streams[stream_id]->codecpar;
	if (codec == nullptr) {
		codec = avcodec_find_decoder(ctxp->codec_id);
	}
	
	AVCodecContext* ctx = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(ctx, ctxp);

	if (codec == NULL) {
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return nullptr;
	}
	
	int res = avcodec_open2(ctx, codec, nullptr);
	if (res < 0) {
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return nullptr;
	}

	auto swr_ctx = swr_alloc_set_opts(nullptr, ctxp->channels > 1 ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, ctxp->sample_rate,
		ctxp->channel_layout ? ctxp->channel_layout : (ctxp->channels > 1 ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO), static_cast<AVSampleFormat>(ctxp->format), ctxp->sample_rate, 0, nullptr);
	if (!swr_ctx) {
		fprintf(stderr, "Could not allocate resampler context\n");
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return nullptr;
	}

	/* initialize the resampling context */
	if (swr_init(swr_ctx) < 0) {
		fprintf(stderr, "Failed to initialize the resampling context\n");
		swr_free(&swr_ctx);
		avcodec_free_context(&ctx);
		avformat_close_input(&container);
		avformat_free_context(container);
		return nullptr;
	}

	std::shared_ptr<Sound> sound(new Sound);
	sound->container = container;
	sound->codec = ctx;
	sound->swr_ctx = swr_ctx;
	sound->stream_id = stream_id;
	LoadSoundFrame(sound, 5);

	sounds[path] = sound;
	return sound;
}

std::shared_ptr<SpritePackage> ResourceManager::LoadSpritePackage(const std::string & path)
{
	if (sprite_packs.find(path) != sprite_packs.end())
		return sprite_packs[path];

	std::string filename = (path == "skin1" ? "resources/skin1.bmp" : (path == "skin2" ? "resources/skin2.bmp" : path));
	unsigned int height, width, texture_id;
	texture_id = loadSprite_ffmpeg(filename.c_str(), height, width);

	std::shared_ptr<SpritePackage> pack(new SpritePackage(texture_id, glm::vec2(width, height)));
	pack->addSprites(path);
	sprite_packs[path] = pack;

	return sprite_packs[path];
}

int ResourceManager::LoadSoundFrame(std::shared_ptr<Sound> sound, int framesize)
{
	if (sound->container == nullptr || sound->codec == nullptr)
		return 0;
	if (sound->is_load_complete)
		return 0;
	AVPacket packet;
	av_init_packet(&packet);
	int read_frame = 0;
	for (int i = 0; i < framesize; i++)
	{
		std::vector<uint8_t> bufferdata;
		int packetcnt = 0;
		while (packetcnt < 10)
		{
			int res_read_frame = av_read_frame(sound->container, &packet);
			if (res_read_frame < 0)
			{
				av_packet_unref(&packet);
				break;
			}
			if (packet.stream_index == sound->stream_id) 
			{
				AVFrame *frame = av_frame_alloc();
				int len = avcodec_send_packet(sound->codec, &packet);
				int frameFinished = avcodec_receive_frame(sound->codec, frame);
				if (frameFinished == 0)
				{
					uint8_t *output;
					if (sound->swr_ctx != nullptr)
					{
						int bufferSize = av_samples_alloc(&output, nullptr, sound->codec->channels > 1 ? 2 : 1, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
						auto out_samples = swr_convert(sound->swr_ctx, &output, frame->nb_samples, const_cast<const uint8_t**>(&frame->data[0]), frame->nb_samples);
						bufferdata.insert(bufferdata.end(), output, output + (bufferSize));
						av_freep(&output);
					}
					else
					{
						bufferdata.insert(bufferdata.end(), frame->data[0], frame->data[0] + frame->linesize[0]);
					}
					packetcnt++;
				}
				av_frame_unref(frame);
				av_frame_free(&frame);
			}
			av_packet_unref(&packet);
		}

		if (packetcnt > 0) {
			ALuint g_buffer;
			alGenBuffers(1, &g_buffer);
			alBufferData(g_buffer, sound->codec->channels>1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, bufferdata.data(), bufferdata.size(), sound->codec->sample_rate);
			sound->buffers.push_back(g_buffer);
			read_frame++;
		}
		else
		{
			sound->is_load_complete = true;
			sound->unload_ffmpeg();
			break;
		}
	}
	return read_frame;
}

int ResourceManager::LoadImageFrame(std::shared_ptr<ImageStream> image_stream, int framesize)
{
	if (image_stream->is_load_complete)
		return 0;
	AVPacket packet;
	av_init_packet(&packet);

	int read_frame = 0;
	int max_buffer = framesize && image_stream->image_buffer.size() > framesize ? framesize : image_stream->image_buffer.size();
	//int max_buffer = image_stream->image_buffer.size();
	while (read_frame < max_buffer && av_read_frame(image_stream->container, &packet) >= 0)
	{
		if (packet.stream_index != image_stream->stream_id)
		{
			av_packet_unref(&packet);
			continue;
		}
		int len = avcodec_send_packet(image_stream->codec, &packet);
		if (len < 0)
		{
			av_packet_unref(&packet);
			continue;
		}

		AVFrame *frame = av_frame_alloc();
		int frameFinished = avcodec_receive_frame(image_stream->codec, frame);
		if (frameFinished == 0)
		{
			AVFrame *frameRGBA = av_frame_alloc();
			//uint8_t *buffer = (uint8_t *)av_malloc(size);
			int byte_size = av_image_alloc(frameRGBA->data, frameRGBA->linesize, image_stream->codec->width, image_stream->codec->height, AV_PIX_FMT_RGBA, 1);
			sws_scale(image_stream->sws_ctx, &frame->data[0], frame->linesize, 0, image_stream->codec->height, &frameRGBA->data[0], frameRGBA->linesize);

			//input data to buffer
			image_stream->image_buffer[read_frame].width = image_stream->codec->width;
			image_stream->image_buffer[read_frame].height = image_stream->codec->height;
			if (!image_stream->image_buffer[read_frame].data)
				image_stream->image_buffer[read_frame].data = new uint8_t[byte_size];
			memcpy(image_stream->image_buffer[read_frame].data, frameRGBA->data[0], byte_size);

			av_freep(&frameRGBA->data[0]);
			av_frame_free(&frameRGBA);
			read_frame++;
		}
		else
		{
			std::cout << "decode image frame failed" << std::endl;
		}
		av_packet_unref(&packet);
		av_frame_unref(frame);
		av_frame_free(&frame);
	}

	if (read_frame == 0)
	{
		image_stream->is_load_complete = true;
		image_stream->unload_ffmpeg();
	}

	return read_frame;
}

void ResourceManager::UnloadSprite(const std::string & path)
{
	auto& it = sprites.find(path);
	if (it != sprites.end())
	{
		sprites.erase(it);
	}
}

void ResourceManager::UnloadSound(const std::string & path)
{
	auto& it = sounds.find(path);
	if (it != sounds.end())
	{
		sounds.erase(it);
	}
}

void ResourceManager::UnloadSprite(std::shared_ptr<Sprite> sprite)
{
	for (const auto& it : sprites)
	{
		if (it.second == sprite)
		{
			sprites.erase(it.first);
			break;
		}
	}
}

void ResourceManager::UnloadSound(std::shared_ptr<Sound> sound)
{
	for (const auto& it : sounds)
	{
		if (it.second == sound)
		{
			sounds.erase(it.first);
			break;
		}
	}
}

Sound::Sound() :codec(nullptr), container(nullptr), is_load_complete(false), stream_id(-1)
{
}

Sound::~Sound()
{
	unload_ffmpeg();
	if (!buffers.empty())
	{
		alDeleteBuffers(buffers.size(), buffers.data());
	}
}

void Sound::unload_ffmpeg()
{
	if (swr_ctx != nullptr)
	{
		swr_free(&swr_ctx);
		swr_ctx = nullptr;
	}
	if (codec != nullptr)
	{
		avcodec_free_context(&codec);
		codec = nullptr;
	}
	if (container != nullptr)
	{
		avformat_close_input(&container);
		avformat_free_context(container);
		container = nullptr;
	}
}

void ImageStream::unload_ffmpeg()
{
	if (sws_ctx != nullptr)
	{
		sws_freeContext(sws_ctx);
		sws_ctx = nullptr;
	}
	if (codec != nullptr)
	{
		avcodec_free_context(&codec);
		codec = nullptr;
	}
	if (container != nullptr)
	{
		avformat_close_input(&container);
		avformat_free_context(container);
		container = nullptr;
	}
}

Image::~Image()
{
	if (data)
		delete[] data;
}

int ResourceManager::ffmpeg_stream::open_media_stream(std::string path, AVMediaType media_type)
{
	this->media_type = media_type;
	container = avformat_alloc_context();
	if (avformat_open_input(&container, path.c_str(), NULL, NULL) < 0) {
		unload_ffmpeg();
		return 0;
	}

	if (avformat_find_stream_info(container, nullptr) < 0) {
		unload_ffmpeg();
		return 0;
	}

	AVCodec *codec = nullptr;
	int stream_id = av_find_best_stream(container, media_type, -1, -1, &codec, 0);
	if (stream_id == -1) {
		unload_ffmpeg();
		return 0;
	}

	AVCodecParameters *ctxp = container->streams[stream_id]->codecpar;
	if (codec == nullptr) {
		codec = avcodec_find_decoder(ctxp->codec_id);
	}
	
	ctx = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(ctx, ctxp);

	if (codec == NULL) {
		unload_ffmpeg();
		return 0;
	}

	int res = avcodec_open2(ctx, codec, nullptr);
	if (res < 0) {
		unload_ffmpeg();
		return 0;
	}

	return 1;
}

void ResourceManager::ffmpeg_stream::read_image_frame()
{
	if (media_type != AVMEDIA_TYPE_VIDEO)
		return;
	
	if (is_load_complete)
		return;

	AVPacket packet;
	av_init_packet(&packet);

	int read_frame = 0;
	//int max_buffer = image_stream->image_buffer.size();
	while (av_read_frame(container, &packet) >= 0)
	{
		if (packet.stream_index != stream_id)
		{
			av_packet_unref(&packet);
			continue;
		}
		int len = avcodec_send_packet(ctx, &packet);
		if (len < 0)
		{
			av_packet_unref(&packet);
			continue;
		}

		AVFrame *frame = av_frame_alloc();
		int frameFinished = avcodec_receive_frame(ctx, frame);
		if (frameFinished == 0)
		{
			AVFrame *frameRGBA = av_frame_alloc();
			//uint8_t *buffer = (uint8_t *)av_malloc(size);
			int byte_size = av_image_alloc(frameRGBA->data, frameRGBA->linesize, ctx->width, ctx->height, AV_PIX_FMT_RGBA, 1);
			sws_scale(sws_ctx, &frame->data[0], frame->linesize, 0, ctx->height, &frameRGBA->data[0], frameRGBA->linesize);

			//input data to buffer
			//image_stream->image_buffer[read_frame].width = image_stream->codec->width;
			//image_stream->image_buffer[read_frame].height = image_stream->codec->height;
			//if (!image_stream->image_buffer[read_frame].data)
				//image_stream->image_buffer[read_frame].data = new uint8_t[byte_size];
			//memcpy(image_stream->image_buffer[read_frame].data, frameRGBA->data[0], byte_size);

			av_freep(&frameRGBA->data[0]);
			av_frame_free(&frameRGBA);
			read_frame++;
		}
		else
		{
			std::cout << "decode image frame failed" << std::endl;
		}
		av_packet_unref(&packet);
		av_frame_unref(frame);
		av_frame_free(&frame);
	}

	if (read_frame == 0)
	{
		is_load_complete = true;
		unload_ffmpeg();
	}

	//return read_frame;
}

void ResourceManager::ffmpeg_stream::read_audio_frame()
{
	if (media_type != AVMEDIA_TYPE_AUDIO)
		return;
}

void ResourceManager::ffmpeg_stream::unload_ffmpeg()
{
	if (swr_ctx != nullptr)
	{
		swr_free(&swr_ctx);
		swr_ctx = nullptr;
	}
	if (sws_ctx != nullptr)
	{
		sws_freeContext(sws_ctx);
		sws_ctx = nullptr;
	}
	if (ctx != nullptr)
	{
		avcodec_free_context(&ctx);
		ctx = nullptr;
	}
	if (container != nullptr)
	{
		avformat_close_input(&container);
		avformat_free_context(container);
		container = nullptr;
	}
}

ResourceManager::ffmpeg_stream::~ffmpeg_stream()
{
	unload_ffmpeg();
}
