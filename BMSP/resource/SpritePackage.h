#pragma once
class SpritePackage
{
private:
	glm::uint texture_id = 0;
	glm::vec2 size;
	std::map<std::string, std::shared_ptr<Sprite>> sprite_map;
public:
	void addSprite(const std::string& name, std::shared_ptr<Sprite> sprite);
	void addSprites(const std::string& list_path);
	std::shared_ptr<Sprite> getSprite(const std::string& name);

	SpritePackage(const glm::uint& texture_id, const glm::vec2& size);
	~SpritePackage();
};
