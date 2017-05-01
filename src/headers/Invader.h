#pragma once
#include "Model.h"

class Invader : public Model {
public:
    float speed = 0.15f, speedMultiplier = 0.025f;
    float minX, maxX, deathTimer = 0.0f;
    bool moveRight = true, isAlive = true;

    Invader() {}
    Invader(GLchar* path, GLchar* texture) {
        this->loadModel(path, texture);

        this->minX = minX;
        this->maxX = -minX;
    }

    bool DoMove(GLfloat &deltaTime) {
        speed += speedMultiplier * deltaTime;

        if (isAlive) {
            if (moveRight) Move(speed * deltaTime, 0.0f, 0.0f);
            else Move(-speed * deltaTime, 0.0f, 0.0f);
        } else {
            deathTimer += deltaTime;

            if (moveRight) Move(speed * deltaTime, -speed * deltaTime, 0.0f);
            else Move(-speed * deltaTime, -speed * deltaTime, 0.0f);
            Rotate(0.0f, 0.0f, 35.0f * deltaTime);
            Grow(-0.025f * deltaTime);

            if (deathTimer > 2.0f) return true;
        }

        return false;
    }

    void MoveDown() {
        Move(0.0f, -0.25f, 0.0f);
        moveRight = !moveRight;
    }

    bool PastEdge() {
        if ((moveRight && meshes[0].position.x >= maxX) || (!moveRight && meshes[0].position.x <= minX)) return true;
        else return false;
    }

    bool PastPlayer(GLfloat playerY) {
        if (meshes[0].position.y <= playerY + 0.5f) return true;
        else return false;
    }

    void SetEdge(float x) {
        this->minX = x;
        this->maxX = -x;
    }
};
