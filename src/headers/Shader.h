#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>

class Shader
{
public:
    GLuint program;

    Shader(std::string vertPath, std::string fragPath) {
        std::string vertSource, fragSource;
        std::ifstream vertFile, fragFile;

        vertFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fragFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try {
            vertFile.open(vertPath);
            fragFile.open(fragPath);

            std::stringstream vertStream, fragStream;

            vertStream << vertFile.rdbuf();
            fragStream << fragFile.rdbuf();

            vertFile.close();
            fragFile.close();

            vertSource = vertStream.str();
            fragSource = fragStream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "SHADER FILE NOT SUCCESFULLY READ" << std::endl;
        }

        const GLchar* vertCode = vertSource.c_str();
        const GLchar * fragCode = fragSource.c_str();

        GLuint vert, frag;
        GLint success;
        GLchar infoLog[512];

        // Vert Shader
        vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &vertCode, NULL);
        glCompileShader(vert);
        glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vert, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::vert::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Frag Shader
        frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &fragCode, NULL);
        glCompileShader(frag);
        glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(frag, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::frag::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Shader Program
        this->program = glCreateProgram();
        glAttachShader(this->program, vert);
        glAttachShader(this->program, frag);
        glLinkProgram(this->program);
        glGetProgramiv(this->program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vert);
        glDeleteShader(frag);

    }

    void Use()
    {
        glUseProgram(this->program);
    }
};
