#pragma once

#include <string>
#include <vector>

class Shader
{
public:
    Shader();
    void init(std::string const& vertex_code, std::string const& fragment_code);
    void use() const;
    template<typename T>
    void setUniform(std::string const& name, T val);
    template<typename T>
    void setUniform(std::string const& name, T val1, T val2);
    template<typename T>
    void setUniform(std::string const& name, T val1, T val2, T val3);

private:
    void checkCompileErr() const;
    void checkLinkingErr() const;
    void compile();
    void link();
    unsigned int vertex_id_{}, fragment_id_{}, id_{};
    std::string vertex_code_;
    std::string fragment_code_;
};
