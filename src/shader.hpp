#pragma once
#include <string>

class Shader {
public:
	//Shader(const char* vertexPath, const char* fragmentPath);
	void Init(const char* vertexPath, const char* fragmentPath);
	void Use();
	unsigned int program;
private:
	std::string ReadFile(const char* path);
};

inline Shader shader1;
inline Shader shader2;
inline Shader shader3;
inline Shader shader4;
inline Shader shader5;
