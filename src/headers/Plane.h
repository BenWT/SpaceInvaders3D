#pragma once
#include "Model.h"

class Plane : public Model {
public:
    Plane() {}
    Plane(GLchar* path, GLchar* texture, GLfloat w, GLfloat h) {
        this->loadModel(path, texture);
    	this->Rotate(0.0f, 180.0f, 0.0f);
        this->Scale(0.05);
        this->Scale(w, h);
    }

    void Render(Shader &shader, Camera &camera, glm::mat4 &projectionMat) {
        for(GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].RenderPlane(shader, camera, projectionMat);
        }
    }

    void Scale(GLfloat v) {
        for(GLuint i = 0; i < this->meshes.size(); i++) this->meshes[i].scale.Set(v);
    }
    void Scale(GLfloat w, GLfloat h) {
        for(GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].scale.Set(this->meshes[i].scale.x * w, this->meshes[i].scale.y * h, 1.0f );
        }
    }
};
