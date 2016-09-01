#ifndef CAMERA_H
#define CAMERA_H


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "mesh.h"

class Camera
{
public:
    virtual ~Camera();

    //Constructor for perspective projection
    Camera(const Mesh& mesh, float fov, float aspect, float zNear, float zFar);
    //Constructor for orthographic projection
    Camera(glm::vec3 pos, const Mesh& mesh, float aspect, glm::vec3 up, float zNear, float zFar);

    glm::mat4 GetViewProjection() const;
    glm::mat4 GetProjection() const;
    glm::mat4 GetView() const;

private:
    glm::mat4 m_projection;
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;
    float view_distance;
};

#endif // CAMERA_H
