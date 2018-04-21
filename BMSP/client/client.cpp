#include "../stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "../managers/GlobalManager.h"

#include "../gfx/gfxSprite.h"
#include "../gfx/gfxPanel.h"

#include "../sfx/sfxSound.h"

#include "../Game/PlayScene.h"
#include "../Game/TestScene.h"
#include "../Game/ListScene.h"

GLFWwindow* window;

int main(void)
{
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
	window = glfwCreateWindow(1600, 1200, "Tutorial 01", NULL, NULL);
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

	IgfxGlobal->LoadShaders("sprite", "shaders/vertex_shader.shader", "shaders/fragment_shader.shader");
	IgfxGlobal->LoadShaders("font", "shaders/vertex_shader.shader", "shaders/font_fragment_shader.shader");
	IgfxGlobal->LoadShaders("layer", "shaders/vertex_shader.shader", "shaders/bga_layer_fragment_shader.shader");
	
	IGlobalManager->setFPS(120);
	int fps = IGlobalManager->getFPS();

	IInputManager->Init(window);

	//sfx::sfxGlobal::Instance();
	IsfxGlobal->start();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	IgfxGlobal->initGlobalArrayBuffer();

	glm::mat4 Ortho = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f);
	IgfxGlobal->setGlobalMatrix(Ortho);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto playscene = std::shared_ptr<PlayScene>(new PlayScene);
	//playscene->SetBMSPath("resource/Cheetahmen 2/Cheetahmen 2 -7keys-.bme");
	playscene->SetBMSPath("resource/spackage/[Sakuzyo][catastrophe]Altale/Altale_A.bms");
	//Altale_A.bms
	IGlobalManager->Push_Scene(playscene);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	auto prev_time = std::chrono::system_clock::now();
	do {
		auto curr_time = std::chrono::system_clock::now();
		auto diff = curr_time - prev_time;
		std::chrono::milliseconds delta(std::chrono::duration_cast<std::chrono::milliseconds>(diff));
		prev_time = curr_time;

		IInputManager->Update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		IGlobalManager->Update(delta);
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		std::this_thread::sleep_until(curr_time + std::chrono::milliseconds(fps));
	} // Check if the ESC key was pressed or the window was closed
	while (IGlobalManager->StackSize() != 0 &&
		glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	IsfxGlobal->quit();
	return 0;
}

