#include "../stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath, unsigned int& height, unsigned int& width) {
	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fopen_s(&fp, imagepath, "rb");
	if (fp == NULL) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	height = *(unsigned int*)&(header[8]);
	width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	free(buffer);

	return textureID;
}

std::shared_ptr<Sprite> ResourceManager::LoadSprite(std::string path)
{
	if (sprites.find(path) != sprites.end())
		return sprites[path];
	auto ext = path.substr(path.length() - 3, 3);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	unsigned int height, width, textrueId;
	if (ext == "bmp")
	{
		textrueId = loadBMP_custom(path.c_str(), height, width);
	}
	else if (ext == "dds")
	{
		textrueId = loadDDS(path.c_str(), height, width);
	}
	if (textrueId == 0)
		return nullptr;

	std::shared_ptr<Sprite> res = std::shared_ptr<Sprite>(new Sprite);
	res->textureId = textrueId;

	res->vertex[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	res->vertex[1] = glm::vec3(0.0f, height, 0.0f);
	res->vertex[2] = glm::vec3(width, 0.0f, 0.0f);

	res->vertex[3] = glm::vec3(0.0f, height, 0.0f);
	res->vertex[4] = glm::vec3(width, height, 0.0f);
	res->vertex[5] = glm::vec3(width, 0.0f, 0.0f);

	res->uv[0] = glm::vec2(0.0f, 1.0f);
	res->uv[1] = glm::vec2(0.0f, 0.0f);
	res->uv[2] = glm::vec2(1.0f, 1.0f);

	res->uv[3] = glm::vec2(0.0f, 0.0f);
	res->uv[4] = glm::vec2(1.0f, 0.0f);
	res->uv[5] = glm::vec2(1.0f, 1.0f);

	sprites[path] = res;
	return res;
}

std::shared_ptr<Sound> ResourceManager::LoadSound(std::string path)
{
	AVFormatContext* container = avformat_alloc_context();
	if (avformat_open_input(&container, path.c_str(), NULL, NULL) < 0) {
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

	auto swr_ctx = swr_alloc_set_opts(nullptr, ctxp->channel_layout ? ctxp->channel_layout : 4, AV_SAMPLE_FMT_S16, 44100,
		ctxp->channel_layout ? ctxp->channel_layout : 4, static_cast<AVSampleFormat>(ctxp->format), ctxp->sample_rate, 0, nullptr);
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
	AVFrame *frame = av_frame_alloc();
	for (int i = 0; i < 5; i++)
	{
		std::vector<uint8_t> bufferdata;
		int packetcnt = 0;
		while (packetcnt < 6)
		{
			int res_read_freame = av_read_frame(sound->container, &packet);
			if (res_read_freame < 0)
			{
				av_packet_unref(&packet);
				break;
			}
			if (packet.stream_index == sound->stream_id) {
				int len = avcodec_send_packet(sound->codec, &packet);
				int frameFinished = avcodec_receive_frame(sound->codec, frame);
				if (frameFinished == 0)
				{
					uint8_t *output;
					if (sound->swr_ctx != nullptr)
					{
						av_samples_alloc(&output, nullptr, sound->codec->channel_layout ? 2 : 1, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
						int bufferSize = av_samples_get_buffer_size(NULL, sound->codec->channels, frame->nb_samples,
							AV_SAMPLE_FMT_S16, 0);
						auto out_samples = swr_convert(sound->swr_ctx, &output, frame->nb_samples, const_cast<const uint8_t**>(frame->extended_data), frame->nb_samples);
						bufferdata.insert(bufferdata.end(), output, output + (bufferSize));
						av_freep(&output);
					}
					else
					{
						bufferdata.insert(bufferdata.end(), frame->data[0], frame->data[0] + frame->linesize[0]);
					}
					packetcnt++;
				}
			}
			av_packet_unref(&packet);
		}

		if (packetcnt > 0) {
			ALuint g_buffer;
			alGenBuffers(1, &g_buffer);
			alBufferData(g_buffer, sound->codec->channel_layout ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, bufferdata.data(), bufferdata.size(), 44100);
			sound->buffers.push_back(g_buffer);
		}
		else
		{
			sound->is_load_complete = true;
			break;
		}
	}
	av_freep(&frame);
}

Sprite::~Sprite()
{
	glDeleteTextures(1, &textureId);
}

Sound::Sound() :codec(nullptr), container(nullptr), is_load_complete(false), stream_id(-1)
{
}

Sound::~Sound()
{
	if (codec != nullptr)
	{
		avcodec_free_context(&codec);
	}
	if (container != nullptr)
	{
		avformat_close_input(&container);
		avformat_free_context(container);
	}
}
