#pragma once

class gfxGlobal
{
private:
	gfxGlobal() {};

	GLuint VertexArrayBuffer = 0;
	GLuint UVArrayBuffer = 0;
	GLuint ColorArrayBuffer = 0;
	GLuint MatrixArrayBuffer = 0;
	GLuint SizeArrayBuffer = 0;
	GLuint TexPositionArrayBuffer = 0;
	GLuint TextureSamplerId = 0;

	glm::mat4 GlobalMatrix;

	std::map<std::string, GLuint> shader_map;

	FT_Library ft_lib=nullptr;
public:
	~gfxGlobal();
	static gfxGlobal* Instance()
	{
		static gfxGlobal* instance = nullptr;
		if (instance == nullptr)
			instance = new gfxGlobal();
		return instance;
	}
	inline void setGlobalMatrix(const glm::mat4& matrix)
	{
		GlobalMatrix = matrix;
	}
	const glm::mat4& getGlobalMatrix()
	{
		return GlobalMatrix;
	}

	void initGlobalArrayBuffer();
	GLuint getGlobalVertexArrayBuffer();
	GLuint getGlobalUVArrayBuffer();
	GLuint getGlobalColorArrayBuffer();
	GLuint getGlobalMatrixArrayBuffer();
	GLuint getGlobalSizeArrayBuffer();
	GLuint getGlobalTexPosArrayBuffer();
	GLuint getGlobalTextureSamplerId();

	GLuint LoadShaders(const std::string& shader_name, char * vertex_file_path, const char * fragment_file_path);
	GLuint UseShaders(const std::string& shader_name);

	FT_Library getFreeTypeLib();
};