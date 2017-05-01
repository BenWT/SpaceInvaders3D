#pragma once
#include "Model.h"

class Player : public Model {
public:
    int lives = 3;
    float speed = 2.0f;
    float minX, maxX, xPos;

    Player() {}
    Player(GLchar* path, GLchar* texture, float minX) {
        this->loadModel(path, texture);

        this->minX = minX;
        this->maxX = -minX;
    }

    void MoveLeft(GLfloat &deltaTime) {
        if (meshes[0].position.x > minX) Move(-speed * deltaTime, 0.0f, 0.0f);
    }
    void MoveRight(GLfloat &deltaTime) {
        if (meshes[0].position.x < maxX) Move(speed * deltaTime, 0.0f, 0.0f);
    }

    void Reset() {
        this->lives = 3;
        this->meshes[0].position.x = 0.0;
        this->meshes[0].position.y = -3.0;
        this->meshes[0].position.z = 0.0;
    }
};
