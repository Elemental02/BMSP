#include "../stdafx.h"
#include <GL/glew.h>

#include "gfxGlobal.h"

gfx::gfxGlobal::~gfxGlobal()
{
	for (auto programs : shader_map)
	{
		glDeleteProgram(programs.second);
	}
	if (ft_lib != nullptr)
		FT_Done_FreeType(ft_lib);
}

void gfx::gfxGlobal::setGlobalMatrix(const glm::mat4 & matrix)
{
	GlobalMatrix = matrix;
}

void gfx::gfxGlobal::setUniformMatrix(const glm::mat4 & matrix)
{
	int ProgramID;
	glGetIntegerv(GL_CURRENT_PROGRAM, &ProgramID);
	GLuint MatrixID = glGetUniformLocation(ProgramID, "GlobalUniform");
	glm::mat4 uniform = GlobalMatrix * matrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &uniform[0][0]);
}

void gfx::gfxGlobal::initGlobalArrayBuffer()
{
	if (!VertexArrayBuffer)
	{
		glGenBuffers(1, &VertexArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexArrayBuffer);
		std::array<glm::vec3, 6> vertex;
		vertex[0] = glm::vec3(0.0f, 0.0f, 0.0f);
		vertex[1] = glm::vec3(0.0f, 1.0f, 0.0f);
		vertex[2] = glm::vec3(1.0f, 0.0f, 0.0f);

		vertex[3] = glm::vec3(0.0f, 1.0f, 0.0f);
		vertex[4] = glm::vec3(1.0f, 1.0f, 0.0f);
		vertex[5] = glm::vec3(1.0f, 0.0f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertex.size(), static_cast<void*>(vertex.data()), GL_STATIC_DRAW);
	}
	if (!UVArrayBuffer) 
	{
		glGenBuffers(1, &UVArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, UVArrayBuffer);
		std::array<glm::vec2, 6> uv;
		uv[0] = glm::vec2(0.0f, 0.0f);
		uv[1] = glm::vec2(0.0f, 1.0f);
		uv[2] = glm::vec2(1.0f, 0.0f);

		uv[3] = glm::vec2(0.0f, 1.0f);
		uv[4] = glm::vec2(1.0f, 1.0f);
		uv[5] = glm::vec2(1.0f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uv.size(), static_cast<void*>(uv.data()), GL_STATIC_DRAW);
	}
	if (!ColorArrayBuffer)
	{
		glGenBuffers(1, &ColorArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, ColorArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW);
	}
	if (!MatrixArrayBuffer)
	{
		glGenBuffers(1, &MatrixArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, MatrixArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), 0, GL_DYNAMIC_DRAW);
	}
	if (!SizeArrayBuffer)
	{
		glGenBuffers(1, &SizeArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, SizeArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), 0, GL_DYNAMIC_DRAW);
	}

	if (!TexPositionArrayBuffer)
	{
		glGenBuffers(1, &TexPositionArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, TexPositionArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW);
	}
}

GLuint gfx::gfxGlobal::getGlobalVertexArrayBuffer()
{
	return VertexArrayBuffer;
}

GLuint gfx::gfxGlobal::getGlobalUVArrayBuffer()
{
	return UVArrayBuffer;
}

GLuint gfx::gfxGlobal::getGlobalColorArrayBuffer()
{
	return ColorArrayBuffer;
}

GLuint gfx::gfxGlobal::getGlobalMatrixArrayBuffer()
{
	return MatrixArrayBuffer;
}

GLuint gfx::gfxGlobal::getGlobalSizeArrayBuffer()
{
	return SizeArrayBuffer;
}

GLuint gfx::gfxGlobal::getGlobalTexPosArrayBuffer()
{
	return TexPositionArrayBuffer;
}

GLuint gfx::gfxGlobal::getGlobalTextureSamplerId()
{
	return TextureSamplerId;
}

FT_Library gfx::gfxGlobal::getFreeTypeLib()
{
	if (ft_lib == nullptr)
	{
		FT_Init_FreeType(&ft_lib);
	}
	return ft_lib;
}

GLuint gfx::gfxGlobal::LoadShaders(const std::string& shader_name, char * vertex_file_path, const char * fragment_file_path)
{
	if (shader_map.find(shader_name) != shader_map.end())
		return 0;
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

	shader_map[shader_name] = ProgramID;
	return ProgramID;
}

GLuint gfx::gfxGlobal::UseShaders(const std::string & shader_name)
{
	if (shader_map.find(shader_name) == shader_map.end())
		return 0;
	auto ProgramID = shader_map[shader_name];
	glUseProgram(ProgramID);

	TextureSamplerId = glGetUniformLocation(ProgramID, "myTextureSampler");
	GLuint MatrixID = glGetUniformLocation(ProgramID, "GlobalUniform");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &GlobalMatrix[0][0]);

	return ProgramID;
}
