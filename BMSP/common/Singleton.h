#pragma once

template<T>
class Singleton<T>
{
private:
	Singleton();
public:
	static T* getInstance()
	{
		static T* instance = nullptr;
		if (instance == nullptr)
			instance = new T();
		return instance;
	}
};