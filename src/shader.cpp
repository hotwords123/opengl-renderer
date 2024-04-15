#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Shader::Shader(GLenum type) {
    id_ = glCreateShader(type);
}

Shader::~Shader() {
    glDeleteShader(id_);
}

bool Shader::compile(const char *source) {
    glShaderSource(id_, 1, &source, nullptr);
    glCompileShader(id_);

    int success;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        int len;
        glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &len);

        string info_log(len, '\0');
        glGetShaderInfoLog(id_, len, nullptr, info_log.data());
        cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << endl;
    }

    return success;
}

bool Shader::compile_from(const char *filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "ERROR::SHADER::FILE_NOT_FOUND\nFILE: " << filename << endl;
        return false;
    }

    file.seekg(0, ios::end);
    size_t len = file.tellg();
    file.seekg(0, ios::beg);

    string source(len, '\0');
    file.read(source.data(), len);
    file.close();

    bool ret = compile(source.c_str());
    if (!ret) {
        cerr << "FILE: " << filename << endl;
    }
    return ret;
}

ShaderProgram::ShaderProgram() {
    id_ = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id_);
}

void ShaderProgram::attach_shader(const Shader &shader) {
    glAttachShader(id_, shader.id());
}

bool ShaderProgram::link() {
    glLinkProgram(id_);

    int success;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        int len;
        glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &len);

        string info_log(len, '\0');
        glGetProgramInfoLog(id_, len, nullptr, info_log.data());
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << endl;
    }

    return success;
}

bool ShaderProgram::build_from(initializer_list<pair<GLenum, const char *>> shaders) {
    for (const auto &[type, filename] : shaders) {
        Shader shader(type);
        if (!shader.compile_from(filename)) {
            return false;
        }
        attach_shader(shader);
    }

    return link();
}

bool ShaderProgram::build_from_vf(const char *prefix) {
    return build_from({
        {GL_VERTEX_SHADER, (string(prefix) + ".vs").c_str()},
        {GL_FRAGMENT_SHADER, (string(prefix) + ".fs").c_str()}
    });
}

bool ShaderProgram::build_from_vgf(const char *prefix) {
    return build_from({
        {GL_VERTEX_SHADER, (string(prefix) + ".vs").c_str()},
        {GL_GEOMETRY_SHADER, (string(prefix) + ".gs").c_str()},
        {GL_FRAGMENT_SHADER, (string(prefix) + ".fs").c_str()}
    });
}

void ShaderProgram::use() const {
    glUseProgram(id_);
}

int ShaderProgram::uniform_location(const char *name) const {
    return glGetUniformLocation(id_, name);
}

void ShaderProgram::set_bool(const char *name, bool value) const {
    glUniform1i(uniform_location(name), value);
}

void ShaderProgram::set_int(const char *name, int value) const {
    glUniform1i(uniform_location(name), value);
}

void ShaderProgram::set_float(const char *name, float value) const {
    glUniform1f(uniform_location(name), value);
}

void ShaderProgram::set_vec3(const char *name, glm::vec3 value) const {
    glUniform3f(uniform_location(name), value.x, value.y, value.z);
}

void ShaderProgram::set_mat3(const char *name, const glm::mat3 &value) const {
    glUniformMatrix3fv(uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::set_mat4(const char *name, const glm::mat4 &value) const {
    glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
}
