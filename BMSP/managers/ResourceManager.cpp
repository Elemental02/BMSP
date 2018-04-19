#include "../stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "ResourceManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLuint loadBMP_custom(const char * imagepath, unsigned int& height, unsigned int& width) {

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	//unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file;
	fopen_s(&file, imagepath, "rb");
	if (!file) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}

	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    fclose(file); return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    fclose(file); return 0; }

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

										 // Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed.
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}

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
			//uint8_t *buffer = (uint8_t *)av_malloc(size);
			av_image_alloc(frameRGBA->data, frameRGBA->linesize, width_conv, height_conv, AV_PIX_FMT_RGBA, 1);
			sws_scale(sws_ctx, &frame->data[0], frame->linesize, 0, ctx->height, &frameRGBA->data[0], frameRGBA->linesize);
			
			// Create one OpenGL texture
			glGenTextures(1, &textureID);

			// "Bind" the newly created texture : all future texture functions will modify this texture
			glBindTexture(GL_TEXTURE_2D, textureID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			// Give the image to OpenGL
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

	std::shared_ptr<Sprite> res = std::shared_ptr<Sprite>(new Sprite);
	res->texture_id = texture_id;
	res->size = glm::vec2(width, height);
	res->texture_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	res->delete_itself = true;
	sprites[path] = res;
	return res;
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
	LoadSoundFrame(sound);

	sounds[path] = sound;
	return sound;
}

void ResourceManager::LoadSoundFrame(std::shared_ptr<Sound> sound)
{
	if (sound->container == nullptr || sound->codec == nullptr)
		return;
	if (sound->is_load_complete)
		return;
	AVPacket packet;
	av_init_packet(&packet);
	for (int i = 0; i < 5; i++)
	{
		std::vector<uint8_t> bufferdata;
		int packetcnt = 0;
		while (packetcnt < 3)
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
		}
		else
		{
			sound->is_load_complete = true;
			break;
		}
	}
	
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

Sprite::~Sprite()
{
	if(delete_itself)
		glDeleteTextures(1, &texture_id);
}

Sound::Sound() :codec(nullptr), container(nullptr), is_load_complete(false), stream_id(-1)
{
}

Sound::~Sound()
{
	if (swr_ctx != nullptr)
	{
		swr_free(&swr_ctx);
	}
	if (codec != nullptr)
	{
		avcodec_free_context(&codec);
	}
	if (container != nullptr)
	{
		avformat_close_input(&container);
		avformat_free_context(container);
	}

	if (!buffers.empty())
	{
		alDeleteBuffers(buffers.size(), buffers.data());
	}
}
