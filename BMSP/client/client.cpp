#include "../stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "../gfx/gfxGlobal.h"
#include "../gfx/gfxSprite.h"
#include "../gfx/gfxPanel.h"
#include "../managers/ResourceManager.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#ifndef AL_FORMAT_MONO_FLOAT32

#define AL_FORMAT_MONO_FLOAT32 0x10010

#endif

#ifndef AL_FORMAT_STEREO_FLOAT32

#define AL_FORMAT_STEREO_FLOAT32 0x10011

#endif

const int MAX_AUDIO_FRAME_SIZE = 192000;

GLFWwindow* window;

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void soundtest(std::string path)
{
	av_register_all();
	avcodec_register_all();

	AVFormatContext* container = avformat_alloc_context();
	if (avformat_open_input(&container, path.c_str(), NULL, NULL) < 0) {
		return;
	}

	if (avformat_find_stream_info(container, nullptr) < 0) {
		return;
	}
	//av_dump_format(container, 0, path.c_str(), false);
	int stream_id = -1;
	int i;
	for (i = 0; i < container->nb_streams; i++) {
		if (container->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			stream_id = i;
			break;
		}
	}
	AVCodec *codec = nullptr;
	stream_id = av_find_best_stream(container, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
	if (stream_id == -1) {
		return;
	}

	AVCodecParameters *ctxp = container->streams[stream_id]->codecpar;
	if (codec == nullptr) {
		codec = avcodec_find_decoder(ctxp->codec_id);
	}
	AVCodecContext* ctx = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(ctx, ctxp);

	if (codec == NULL) {
		return;
	}

	int res = avcodec_open2(ctx, codec, nullptr);
	if (res < 0) {
		
		return;
	}

	ALCcontext* alContext;
	auto alDevice = alcOpenDevice(nullptr);

	if (alDevice)
	{
		alContext = alcCreateContext(alDevice, nullptr);
		alcMakeContextCurrent(alContext);
	}

	ALuint g_source;
	std::vector<ALuint> g_buffers;

	alGenSources(1, &g_source);

	auto swr_ctx = swr_alloc_set_opts(nullptr, ctxp->channel_layout ? ctxp->channel_layout : 4, AV_SAMPLE_FMT_S16, 44100,
		ctxp->channel_layout ? ctxp->channel_layout : 4, static_cast<AVSampleFormat>(ctxp->format), ctxp->sample_rate, 0, nullptr);
	if (!swr_ctx) {
		fprintf(stderr, "Could not allocate resampler context\n");
		return;
	}

	/* initialize the resampling context */
	if (swr_init(swr_ctx) < 0) {
		fprintf(stderr, "Failed to initialize the resampling context\n");
		return;
	}

	AVPacket packet;
	av_init_packet(&packet);

	AVFrame *frame = av_frame_alloc();

	int len;
	int frameFinished = 0;
	std::vector<uint8_t> bufferdata;
	int cnt = 1;
	while(av_read_frame(container, &packet) >= 0)
	{
		if (packet.stream_index == stream_id) {
			int len = avcodec_send_packet(ctx, &packet);
			frameFinished = avcodec_receive_frame(ctx, frame);
			if (frameFinished==0) {
				uint8_t *output;
				av_samples_alloc(&output, nullptr, ctxp->channel_layout ? 2 : 1, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
				int bufferSize = av_samples_get_buffer_size(NULL, ctxp->channels, frame->nb_samples,
					AV_SAMPLE_FMT_S16, 0);
				auto out_samples = swr_convert(swr_ctx, &output, frame->nb_samples, const_cast<const uint8_t**>(frame->extended_data), frame->nb_samples);
				
				bufferdata.insert(bufferdata.end(), output, output + (bufferSize));
				cnt++;
				av_freep(&output);
			}
			else {
				break;
			}
			if (cnt > 5) {
				cnt = 0;
				ALuint g_buffer;
				alGenBuffers(1, &g_buffer);
				alBufferData(g_buffer, ctxp->channel_layout ? AL_FORMAT_STEREO16: AL_FORMAT_MONO16, bufferdata.data(), bufferdata.size(), 44100);
				alSourceQueueBuffers(g_source, 1, &g_buffer);
				ALint state;
				alGetSourcei(g_source, AL_SOURCE_STATE, &state);
				if (state != AL_PLAYING) {
					alSourcePlay(g_source);
				}
				bufferdata.clear();
			}
		}
	}
	//alBufferData(g_buffer, ctxp->channel_layout ? AL_FORMAT_STEREO16: AL_FORMAT_MONO16, bufferdata.data(), bufferdata.size(), 44100);
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 01", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLFW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	GLuint programID = LoadShaders("shaders/vertex_shader.shader", "shaders/fragment_shader.shader");

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glUseProgram(programID);

	gfxGlobal::Instance()->initGlobalArrayBuffer(programID);

	glm::mat4 Ortho = glm::ortho(0.0f, 1024.0f, 768.0f, 0.0f);
	glm::mat4 mvp = Ortho;

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	const int spritecnt = 1;
	gfx::gfxSprite sprite[spritecnt];

	gfx::gfxPanel panel;
	panel.setPosition(glm::vec3(100.0f, 10.0f, 0.0f));
	soundtest("resource/kobayashisanchinmd-1.mp4");//BGM1-1.wav");//kobayashisanchinmd-1.mp4");//

	for (int i = 0; i < spritecnt; i++)
	{
		sprite[i].setSprite(ResourceManager::Instance()->LoadSprite("resource/action1_01.bmp"));
		sprite[i].setPosition(glm::vec3(rand() % 1024, rand() % 768, 0.0f));
		sprite[i].setColor(glm::vec4((rand()%1000)/1000.0f, (rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f, 1.0f));
		panel.addSprite(sprite[i]);
	}
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	do {
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		panel.Render();
		/*for (int i = 0; i < spritecnt; i++)
		{
			sprite[i].Render();
		}*/
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

