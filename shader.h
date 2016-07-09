#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <GL/glew.h>

#include "camera.h"
#include "transform.h"

class Shader
{
public:
    Shader(const std::string& fileName);
    void Bind();
    void Update(const Transform& transform, const Camera& camera);

    virtual ~Shader();
private:
    static const unsigned int NUM_SHADERS = 2;

    enum {
        TRANSFORM_U,
        NUM_UNIFORMS
    };

    GLuint m_program;
    GLuint m_shaders[NUM_SHADERS];
    GLuint m_uniforms[NUM_UNIFORMS];

};


#endif // SHADER_H
