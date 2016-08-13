#include "camera.h"

//for perspective proj
Camera::Camera(const Mesh& mesh, float fov, float aspect, float zNear, float zFar)
{
    view_distance = mesh.radius * 4.f;
    m_projection = glm::perspective(fov, aspect, zNear, zFar);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);

    m_position = glm::vec3(0.0f, 0.0f, -view_distance);
    m_forward = glm::vec3(0.0f);

}

//for orthographic proj
Camera::Camera(glm::vec3 pos,const Mesh& mesh, float aspect, glm::vec3 up)
{
    view_distance = mesh.radius + 1.f;
    m_projection = glm::ortho(-aspect*view_distance, aspect*view_distance, -view_distance, view_distance,-1000.0f, 1000.0f);
    m_up = up;

    m_position = pos;
    m_forward = mesh.centre;

}

glm::mat4 Camera::GetViewProjection() const {
    return m_projection * glm::lookAt(m_position, m_forward, m_up);
}


glm::mat4 Camera::GetProjection() const{
    return m_projection;
}
glm::mat4 Camera::GetView() const{
    return glm::lookAt(m_position, m_forward, m_up);
}

Camera::~Camera()
{
}

void Camera::RotateY(float angle)
{
    static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

    glm::mat4 rotation = glm::rotate(angle, UP);

    m_forward = glm::vec3(glm::normalize(rotation * glm::vec4(m_forward, 0.0)));
    m_up = glm::vec3(glm::normalize(rotation * glm::vec4(m_up, 0.0)));
}
