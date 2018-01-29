#pragma once

class gfxGlobal
{
private:
	gfxGlobal() {};

	GLuint VertexArrayBuffer;
	GLuint UVArrayBuffer;
	GLuint ColorArrayBuffer;
	GLuint MatrixArrayBuffer;
	GLuint TextureSamplerId;
public:
	~gfxGlobal();
	static gfxGlobal* Instance()
	{
		static gfxGlobal* instance = nullptr;
		if (instance == nullptr)
			instance = new gfxGlobal();
		return instance;
	}

	void initGlobalArrayBuffer(GLuint programID);
	GLuint getGlobalVertexArrayBuffer();
	GLuint getGlobalUVArrayBuffer();
	GLuint getGlobalColorArrayBuffer();
	GLuint getGlobalMatrixArrayBuffer();
	GLuint getGlobalTextureSamplerId();
};