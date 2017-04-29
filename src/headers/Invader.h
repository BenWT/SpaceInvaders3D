#pragma once
#include "Model.h"

class Invader : public Model {
public:
    std::function<void()> moveDownCallback;
    float speed = 1.25f;
    float minX, maxX;
    bool moveRight = true;

    Invader(GLchar* path, GLchar* texture, float minX, std::function<void()> moveDownCallback) {
        this->loadModel(path, texture);

        this->moveDownCallback = moveDownCallback;
        this->minX = minX;
        this->maxX = -minX;
    }

    void DoMove(double &deltaTime) {
        if (moveRight) Move(speed * deltaTime, 0.0f, 0.0f);
        else Move(-speed * deltaTime, 0.0f, 0.0f);
    }

    void MoveDown() {
        Move(0.0f, -0.25f, 0.0f);
        moveRight = !moveRight;
    }

    bool PastEdge() {
        if ((moveRight && meshes[0].position.x >= maxX) || (!moveRight && meshes[0].position.x <= minX)) return true;
        else return false;
    }
};
