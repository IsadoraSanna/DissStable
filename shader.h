#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <GL/glew.h>

#include "camera.h"
#include "transform.h"
#include "material.h"

class Shader
{
public:
    Shader(const std::string& fileName);
    void Bind();
    void Update( Transform& transform_camera, const Camera& camera, Transform& transform_mesh, Material material);
    void UpdateWOBB(const Transform& transform_camera, const Camera& camera, const Transform& transform_OBB);
    virtual ~Shader();
private:
    static const unsigned int NUM_SHADERS = 2;
    static const unsigned int NUM_UNIFORMS = 8;

    enum {
        PROJECTION,
        VIEW,
        TRANSFORM_C,
        TRANSFORM_OBB,
        TRANSFORM_M,
        VIEW_POS
    };

    GLuint m_program;
    GLuint m_shaders[NUM_SHADERS];
    GLuint m_uniforms[NUM_UNIFORMS];

};


#endif // SHADER_H
