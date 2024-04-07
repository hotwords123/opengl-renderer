#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(GLenum type);
    ~Shader();

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    GLuint id() const { return id_; }

    bool compile(const char *source);
    bool compile_from(const char *filename);

private:
    GLuint id_;
};

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;

    GLuint id() const { return id_; }

    void attach_shader(const Shader &shader);
    bool link();
    bool build_from(const char *vertex, const char *fragment);

    void use() const;

    int uniform_location(const char *name) const;

    void set_bool(const char *name, bool value) const;
    void set_float(const char *name, float value) const;
    void set_vec3(const char *name, glm::vec3 value) const;
    void set_mat3(const char *name, const glm::mat3 &value) const;
    void set_mat4(const char *name, const glm::mat4 &value) const;

private:
    GLuint id_;
};
