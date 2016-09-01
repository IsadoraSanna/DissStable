#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "obj_loader.h"

class Vertex
{
public:
    Vertex(const glm::vec3& pos, const glm::vec3& normal = glm::vec3(0,0,0))
    {
        this->pos = pos;
        this->normal = normal;
    }
    glm::vec3* GetPos() { return &pos; }
    glm::vec3* GetNormal() { return &normal; }
private:
    glm::vec3 pos;
    glm::vec3 normal;
};

class Mesh
{
public:
    glm::vec3 centre;
    float radius;
    IndexedModel modelData;

    Mesh(const std::string& fileName);
    Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);

    virtual ~Mesh();
    void Draw();
private:
    static const int NUM_BUFFERS = 3;
    enum {
        POSITION_VB,
        NORMAL_VB,
        INDEX_VB
    };

    void InitMesh(const IndexedModel& model);
    //refers mesh data to the GPU
    GLuint m_vertexArrayObject;
    GLuint m_vertexArrayBuffers[NUM_BUFFERS];

    //how many verteces we want to draw (we need to keep track how big it is)
    unsigned int m_drawCount;
};


#endif // MESH_H
