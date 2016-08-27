#include "shader.h"

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>

static std::string LoadShader(const std::string& fileName);
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
static GLuint CreateShader(const std::string& text, GLenum type);

Shader::Shader(const std::string& fileName)
{
    //we want to create a new shader program
    m_program = glCreateProgram();
    m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
    m_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);

    //add the two shaders to our shader program
    for (unsigned int i = 0; i < NUM_SHADERS; i++)
        glAttachShader(m_program, m_shaders[i]);

    glBindAttribLocation(m_program, 0, "position");
    glBindAttribLocation(m_program, 1, "normal");

    glLinkProgram(m_program);
    CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

    glValidateProgram(m_program);
    CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Invalid shader program");

    m_uniforms[PROJECTION] = glGetUniformLocation(m_program, "projection");
    m_uniforms[VIEW] = glGetUniformLocation(m_program, "view");
    m_uniforms[TRANSFORM_C] = glGetUniformLocation(m_program, "model_camera");
    m_uniforms[TRANSFORM_OBB] = glGetUniformLocation(m_program, "model_OBB");
    m_uniforms[TRANSFORM_M] = glGetUniformLocation(m_program, "model_mesh");
    m_uniforms[VIEW_POS] = glGetUniformLocation(m_program, "viewPos");




}


Shader::~Shader()
{
    for (unsigned int i = 0; i < NUM_SHADERS; i++)
    {
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }
    glDeleteProgram(m_program);
}

static std::string LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if (file.is_open())
    {
        while (file.good())
        {
            getline(file, line);
            output.append(line + "\n");
        }
    }
    else
    {
        std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if (isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if (success == GL_FALSE)
    {
        if (isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

void Shader::Bind() {
    glUseProgram(m_program);
}

void Shader::Update( Transform& transform_camera, const Camera& camera,  Transform& transform_mesh, Material material) {

    glm::mat4 projection = camera.GetProjection();
    glUniformMatrix4fv(m_uniforms[PROJECTION], 1, GL_FALSE, &projection[0][0]);
    glm::mat4 view = camera.GetView();
    glUniformMatrix4fv(m_uniforms[VIEW], 1, GL_FALSE, &view[0][0]);
    glm::mat4 model_camera = transform_camera.GetModel();
    glUniformMatrix4fv(m_uniforms[TRANSFORM_C], 1, GL_FALSE, &model_camera[0][0]);

    glm::mat4 model_mesh = transform_mesh.GetModel();
    glUniformMatrix4fv(m_uniforms[TRANSFORM_M], 1, GL_FALSE, &model_mesh[0][0]);
    glm::vec3 viewPos = transform_camera.GetPos();
    glUniform3f(m_uniforms[VIEW_POS], viewPos.x, viewPos.y, viewPos.z);

    if(material.shininess == -1.0f){
         // Set material properties
        glUniform3f(glGetUniformLocation(m_program, "material.ambient"),   1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(m_program, "material.diffuse"),   1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(m_program, "material.specular"),  0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(m_program, "material.shininess"), 255.0f);
    }else{
        glUniform3f(glGetUniformLocation(m_program, "material.ambient"),   material.ambient.r, material.ambient.g, material.ambient.b);
        glUniform3f(glGetUniformLocation(m_program, "material.diffuse"),   material.diffuse.r, material.diffuse.g, material.diffuse.b);
        glUniform3f(glGetUniformLocation(m_program, "material.specular"),  material.specular.r, material.specular.g, material.specular.b);
        glUniform1f(glGetUniformLocation(m_program, "material.shininess"), material.shininess*1000);
    }


    // Set lights properties
    glm::vec3 lightColor = glm::vec3(0.8f, 0.8f, 0.8f);
    //glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f); // Decrease the influence
    //glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // Low influence
    glm::vec3 diffuseColor = lightColor; // Decrease the influence
    glm::vec3 ambientColor = diffuseColor; // Low influence
    glUniform3f(glGetUniformLocation(m_program, "light.position"), 1.2f, 1.0f, 2.0f);
    glUniform3f(glGetUniformLocation(m_program, "light.ambient"),  ambientColor.x, ambientColor.y, ambientColor.z);
    glUniform3f(glGetUniformLocation(m_program, "light.diffuse"),  diffuseColor.x, diffuseColor.y, diffuseColor.z);
    glUniform3f(glGetUniformLocation(m_program, "light.specular"), 1.0f, 1.0f, 1.0f);


}

void Shader::UpdateWOBB(const Transform& transform_camera, const Camera& camera, const Transform& transform_OBB) {
    glm::mat4 projection = camera.GetProjection();
    glUniformMatrix4fv(m_uniforms[PROJECTION], 1, GL_FALSE, &projection[0][0]);
    glm::mat4 view = camera.GetView();
    glUniformMatrix4fv(m_uniforms[VIEW], 1, GL_FALSE, &view[0][0]);
    glm::mat4 model_camera = transform_camera.GetModel();
    glUniformMatrix4fv(m_uniforms[TRANSFORM_C], 1, GL_FALSE, &model_camera[0][0]);

    glm::mat4 model_OBB = transform_OBB.GetModel();
    glUniformMatrix4fv(m_uniforms[TRANSFORM_OBB], 1, GL_FALSE, &model_OBB[0][0]);

}

static GLuint CreateShader(const std::string& text, GLenum type) {
    GLuint shader = glCreateShader(type);

    if (shader == 0)
        std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* shaderSourceStrings[1];
    GLint shaderSourceStringsLengths[1];
    shaderSourceStrings[0] = text.c_str();
    shaderSourceStringsLengths[0] = text.length();

    glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringsLengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}
