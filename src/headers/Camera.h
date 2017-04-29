#pragma once

const GLfloat YAW = -90.0F;
const GLfloat PITCH = 0.0F;
const GLfloat ZOOM = 45.0F;

class Camera {
public:
    glm::vec3 position, front, up, right, worldUp;
    GLfloat yaw, pitch, zoom;

    Camera() {}
    Camera(glm::vec3 position) {
        this->position = position;
        this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = YAW;
        this->pitch = PITCH;
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }

private:
    void updateCameraVectors() {
        glm::vec3 f;
        f.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        f.y = sin(glm::radians(this->pitch));
        f.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

        this->front = glm::normalize(f);
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};
