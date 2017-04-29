#pragma once

class GameState {
public:
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f));
    Cubemap skybox;
    Model player;
    std::vector<Invader> invaders;

    GameState() {
        this->camera = Camera(glm::vec3(0.0f, 0.0, 10.0f));
    }

    void Update(double &deltaTime) {
        if (CheckMoveDown()) {
            for (int i = 0; i < invaders.size(); i++) {
                invaders[i].MoveDown();
            }
        } else {
            for (int i = 0; i < invaders.size(); i++) {
                invaders[i].DoMove(deltaTime);
            }
        }
    }

    void Render(Shader main, Shader sky, glm::mat4 projection) {
    	skybox.Render(sky, projection, camera.GetViewMatrix());
        player.Render(main, camera, projection);
        for (int i = 0; i < invaders.size(); i++) invaders[i].Render(main, camera, projection);
    }

    bool CheckMoveDown() {
        for (int i = 0; i < invaders.size(); i++) {
            if (invaders[i].PastEdge()) return true;
        }
        return false;
    }
};
