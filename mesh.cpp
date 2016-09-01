#include "mesh.h"
#include <fstream>
#include <iostream>

void displayData(Mesh* m, const std::string& fileName);

Mesh::Mesh(const std::string& fileName) {
    IndexedModel  model = OBJModel(fileName).ToIndexedModel();
    OBJModel objModel = OBJModel(fileName);
    this->centre = objModel.centre;
    this->radius = objModel.radius;
    this->modelData = model;
    displayData(this, fileName);
    InitMesh(model);
}

void displayData(Mesh* m, const std::string& fileName){
    std::cout << "******************FILE INFO******************" << std::endl;
    std::cout << "FILENAME: " << fileName << std::endl;
    std::cout << "MESH centre: x = " << m->centre.x << " y = " << m->centre.y << " z = " << m->centre.z << std::endl;
    std::cout << "MESH radius: " << m->radius << std::endl;
    //std::cout << "VERTICES: " << m->vertices.size() << std::endl;
    std::cout << "*********************************************" << std::endl;
}

void Mesh::InitMesh(const IndexedModel& model) {

    m_drawCount = model.indices.size();

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    //Allocate, assign and bind a Vertex Array Object
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    //Allocate and assign NUM_BUFFERS Vertex Buffer Objects
    //NUM_BUFFERS = 3 (position, normal, index)
    glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);

    //VERTICES
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
    //Copy vertex data to the first buffer (POSITION_VB = 1)
    glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW);
    //enable attribute (0)
    glEnableVertexAttribArray(0);
    //Specify is stored at index 0 and each position contains 3 float (x,y,z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //NORMALS
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //INDICES
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}


Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
    IndexedModel model;
    for (unsigned int i = 0; i < numVertices; i++) {
        model.positions.push_back(*vertices[i].GetPos());
        model.normals.push_back(*vertices[i].GetNormal());
    }

    for (unsigned int i = 0; i < numIndices; i++)
        model.indices.push_back(indices[i]);

    this->modelData = model;
    InitMesh(model);
}


Mesh::~Mesh()
{
    glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::Draw()
{
    glBindVertexArray(m_vertexArrayObject);
    glDrawElementsBaseVertex(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, 0);
    glBindVertexArray(0);

}
