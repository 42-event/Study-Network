#pragma once

template <typename T>
class Singleton
{
private:
	static T t;

public:
	static inline T& GetInstance()
	{
		return t;
	}
};

template <typename T>
T Singleton<T>::t;
