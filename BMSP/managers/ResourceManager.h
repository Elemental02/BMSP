#pragma once
struct Sprite
{
public:
	std::array<glm::vec3, 6> vertex;
	std::array<glm::vec2, 6> uv;
	glm::uint textureId = 0;

	~Sprite();
};

class ResourceManager
{
private:
	ResourceManager() {}
	std::map<std::string, std::shared_ptr<Sprite>> sprites;
public:
	static ResourceManager* Instance()
	{
		static ResourceManager* instance = nullptr;
		if (instance == nullptr)
			instance = new ResourceManager();
		return instance;
	}
	std::shared_ptr<Sprite> LoadSprite(std::string path);
};