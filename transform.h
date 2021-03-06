#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Transform
{
public:
    Transform(const glm::vec3& pos = glm::vec3(), const glm::vec3& rot = glm::vec3(), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f)) : m_pos(pos), m_rot(rot), m_scale(scale)
    {
    }

    inline glm::mat4 GetModel() const
    {
        glm::mat4 posMat = glm::translate(m_pos);
        glm::mat4 scaleMat = glm::scale(m_scale);
        glm::mat4 rotX = glm::rotate(m_rot.x, glm::vec3(1.0, 0.0, 0.0));
        glm::mat4 rotY = glm::rotate(m_rot.y, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 rotZ = glm::rotate(m_rot.z, glm::vec3(0.0, 0.0, 1.0));
        glm::mat4 rotMat = rotX * rotY * rotZ;

        return posMat * rotMat * scaleMat;
    }

    void Reset()
    {
        m_pos = glm::vec3();
        m_rot = glm::vec3();
        m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    glm::vec3& GetPos() { return m_pos; }
    glm::vec3& GetRot() { return m_rot; }
    glm::vec3& GetScale() { return m_scale; }

    void SetPos(glm::vec3& pos) { m_pos = pos; }
    void SetRot(glm::vec3& rot) { m_rot = rot; }
    void SetScale(glm::vec3& scale) { m_scale = scale; }

    glm::vec3 m_pos;
    glm::vec3 m_rot;
    glm::vec3 m_scale;
protected:
private:

};


#endif // TRANSFORM_H
