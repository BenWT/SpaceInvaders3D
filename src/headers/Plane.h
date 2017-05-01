#pragma once
#include "Model.h"

class Plane : public Model {
public:
    Plane() {}
    Plane(GLchar* path, GLchar* texture) {
        this->loadModel(path, texture);
    }

    void Render(Shader &shader, Camera &camera, glm::mat4 &projectionMat) {
        for(GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].RenderPlane(shader, camera, projectionMat);
        }
    }
};
