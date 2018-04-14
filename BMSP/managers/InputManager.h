#pragma once

enum KeyState
{
	State_Up, State_Press, State_Down, State_Release
};
enum KeyIndex
{
	Up = 8, Down, Enter, Tab
};

struct GLFWwindow;

class InputManager
{
private:
	GLFWwindow* window;
	std::vector<int> keys_to_update;
	std::map<int, int> prev_state;
	std::map<int, int> curr_state;
public:
	InputManager() {}

	void Init(GLFWwindow* window);
	void Update();
	KeyState getKeyState(int key);
};