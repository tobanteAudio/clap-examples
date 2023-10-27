#include "opengl_shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Shader::Shader() = default;

void Shader::init(std::string const& vertex_code, std::string const& fragment_code)
{
    vertex_code_   = vertex_code;
    fragment_code_ = fragment_code;
    compile();
    link();
}

void Shader::compile()
{
    char const* vcode = vertex_code_.c_str();
    vertex_id_        = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_id_, 1, &vcode, nullptr);
    glCompileShader(vertex_id_);

    char const* fcode = fragment_code_.c_str();
    fragment_id_      = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id_, 1, &fcode, nullptr);
    glCompileShader(fragment_id_);
    checkCompileErr();
}

void Shader::link()
{
    id_ = glCreateProgram();
    glAttachShader(id_, vertex_id_);
    glAttachShader(id_, fragment_id_);
    glLinkProgram(id_);
    checkLinkingErr();
    glDeleteShader(vertex_id_);
    glDeleteShader(fragment_id_);
}

void Shader::use() const { glUseProgram(id_); }

template<>
void Shader::setUniform<int>(std::string const& name, int val)
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), val);
}

template<>
void Shader::setUniform<bool>(std::string const& name, bool val)
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<GLint>(val));
}

template<>
void Shader::setUniform<float>(std::string const& name, float val)
{
    glUniform1f(glGetUniformLocation(id_, name.c_str()), val);
}

template<>
void Shader::setUniform<float>(std::string const& name, float val1, float val2)
{
    glUniform2f(glGetUniformLocation(id_, name.c_str()), val1, val2);
}

template<>
void Shader::setUniform<float>(std::string const& name, float val1, float val2, float val3)
{
    glUniform3f(glGetUniformLocation(id_, name.c_str()), val1, val2, val3);
}

template<>
void Shader::setUniform<float*>(std::string const& name, float* val)
{
    glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, val);
}

void Shader::checkCompileErr() const
{
    int success = 0;
    char infoLog[1024];
    glGetShaderiv(vertex_id_, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        glGetShaderInfoLog(vertex_id_, 1024, nullptr, infoLog);
        std::cout << "Error compiling Vertex Shader:\n" << infoLog << std::endl;
    }
    glGetShaderiv(fragment_id_, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        glGetShaderInfoLog(fragment_id_, 1024, nullptr, infoLog);
        std::cout << "Error compiling Fragment Shader:\n" << infoLog << std::endl;
    }
}

void Shader::checkLinkingErr() const
{
    int success = 0;
    char infoLog[1024];
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(id_, 1024, nullptr, infoLog);
        std::cout << "Error Linking Shader Program:\n" << infoLog << std::endl;
    }
}
