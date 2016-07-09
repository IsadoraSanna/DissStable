#ifndef CAMERA_H
#define CAMERA_H


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "mesh.h"

class Camera
{
public:
    virtual ~Camera();

    enum projectionType{
        PERSPECTIVE_P,
        ORTHOGRAPHIC_P
    };

    Camera(glm::vec3 pos, const Mesh& mesh, float fov, float aspect, float zNear, float zFar, Camera::projectionType pType);
    glm::mat4 GetViewProjection() const;
    void RotateY(float angle);

private:
    glm::mat4 m_projection;
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;
    float view_distance;
};

#endif // CAMERA_H
