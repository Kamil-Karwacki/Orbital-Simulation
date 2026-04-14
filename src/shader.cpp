#include <glad/glad.h>
#include <iostream>
#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

void Shader::Init(const char *vertexPath, const char *fragmentPath) {
    std::string vertexShaderSrc = ReadFile(vertexPath);
    std::string fragmentShaderSrc = ReadFile(fragmentPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertexSrc = vertexShaderSrc.c_str();
    const char* fragmentSrc = fragmentShaderSrc.c_str();

    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);

    int vertexSuccess, fragmentSuccess;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!vertexSuccess) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        throw std::runtime_error(std::string("Vertex shader compilation error: ") + infoLog);
    }
    if (!fragmentSuccess) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        throw std::runtime_error(std::string("Fragment shader compilation error: ") + infoLog);
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    int success;
    char programLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, programLog);
        throw std::runtime_error(std::string("Shader program linking error: ") + programLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Successfully created and linked shader program\n\n";
}

void Shader::Use() {
    glUseProgram(program);
}

std::string Shader::ReadFile(const char* path) {
    std::cout << "Loading shader " << path << "\n";
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file)
        throw std::runtime_error(std::string("Couldn't read shader file:") + path);

    std::string contents;
    file.seekg(0, std::ios::end);
    contents.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&contents[0], contents.size());
    file.close();

    file.close();
    return contents;
}