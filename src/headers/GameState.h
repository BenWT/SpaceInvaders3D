#pragma once

struct Inputs {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

struct Interface {
    std::array<Plane, 3> lives;
    std::array<Plane, 10> score;
    Plane scoreText;
};

class GameState {
public:
    Shader mainShader, skyboxShader, hudShader;
    Camera camera;
    Cubemap skybox;
    Model lBarricade, rBarricade;
    Player player;
    Invader invader;
    Bullet bullet;
    Barricade barricade;
    std::vector<Invader> invaders;
    std::vector<Bullet> bullets;
    std::vector<Barricade> barricades;
    Inputs input;
    Interface interface;

    float screenEdge, fireTimer = 0.45f;

    int playerScore = 0;
    float pitchAmount = 0.0f, maxP = 5.0f, minP = -5.0f;

    GameState() {
        this->camera = Camera(glm::vec3(0.0f, 0.0, 10.0f));
    }

    void Update(GLfloat &deltaTime) {
        fireTimer += deltaTime;

        if (input.up && !input.down) pitchAmount -= deltaTime;
        if (input.down && !input.up) pitchAmount += deltaTime;
        if (input.left && !input.right) player.MoveLeft(deltaTime);
        if (input.right && !input.left) player.MoveRight(deltaTime);
        if (pitchAmount > maxP) pitchAmount = maxP;
        if (pitchAmount < minP) pitchAmount = minP;

        camera.position.x = 0.1f * player.getX();
        camera.position.y = -pitchAmount * 0.95f;
        camera.yaw = 270 - (1.5f * player.getX());
        camera.pitch = pitchAmount * 5.0f;
        camera.updateCameraVectors();

        // EnemyShoot(rand() % 1200);

        if (CheckMoveDown()) {
            for (int i = 0; i < invaders.size(); i++) {
                invaders[i].MoveDown();
            }
        } else {
            for (int i = 0; i < invaders.size();) {
                if (invaders[i].DoMove(deltaTime)) invaders.erase(invaders.begin() + i);
                else i++;
            }
        }

        for (int i = 0; i < bullets.size(); i++) bullets[i].Update(deltaTime);

        GLfloat invaderSize = 0.25f;
        GLfloat playerWidth = 0.35f, playerHeight = 0.25f;
        GLfloat bulletWidth = 0.05f, bulletHeight = 0.05f;
        GLfloat barricadeWidth = 0.45f, barricadeHeight = 0.45f;

        for (int i = 0; i < bullets.size();) {
            bool hitBarricade = false;

            for (int j = 0; j < barricades.size(); j++) {
                if (!hitBarricade) {
                    if (bullets[i].getX() + bulletWidth > barricades[j].getX() && bullets[i].getX() - bulletWidth < barricades[j].getX() + barricadeWidth) {
                        if (bullets[i].getY() + bulletHeight > barricades[j].getY() - barricadeHeight && bullets[i].getY() - bulletHeight < barricades[j].getY() + barricadeHeight) {
                            hitBarricade = true;
                            bullets[i].doDestroy = true;
                            barricades[j].hits++;
                            barricades[j].Rotate(180.0f, 0.0f, 0.0f);
                            if (barricades[j].hits > 1) barricades.erase(barricades.begin() + j);
                        }
                    }
                }
            }

            if (!hitBarricade) {
                if (bullets[i].isPlayer) {
                    for (int j = 0; j < invaders.size(); j++) {
                        if (bullets[i].getX() + bulletWidth > invaders[j].getX() - invaderSize && bullets[i].getX() - bulletWidth < invaders[j].getX() + invaderSize) {
                            if (bullets[i].getY() + bulletHeight > invaders[j].getY() - invaderSize && bullets[i].getY() - bulletHeight < invaders[j].getY() + invaderSize) {
                                if (invaders[j].isAlive) {
                                    bullets[i].doDestroy = true;
                                    invaders[j].isAlive = false;
                                    playerScore += 10;
                                }
                            }
                        }
                    }
                } else {
                    if (bullets[i].getX() + bulletWidth > player.getX() - playerWidth && bullets[i].getX() - bulletWidth < player.getX() + playerWidth) {
                        if (bullets[i].getY() + bulletHeight > player.getY() - playerHeight && bullets[i].getY() - bulletHeight < player.getY() + playerHeight) {
                            bullets[i].doDestroy = true;
                            player.lives--;
                        }
                    }
                }
            }


            if (bullets[i].doDestroy) bullets.erase(bullets.begin() + i);
            else i++;
        }
    }

    void Render(glm::mat4 &projection) {
    	skybox.Render(skyboxShader, projection, camera.GetViewMatrix());
        lBarricade.Render(mainShader, camera, player.meshes[0].position, projection);
        rBarricade.Render(mainShader, camera, player.meshes[0].position, projection);

        for (int i = 0; i < bullets.size(); i++) bullets[i].Render(mainShader, camera, player.meshes[0].position, projection);
        for (int i = 0; i < invaders.size(); i++) invaders[i].Render(mainShader, camera, player.meshes[0].position, projection);
        for (int i = 0; i < barricades.size(); i++) barricades[i].Render(mainShader, camera, player.meshes[0].position, projection);

        player.Render(mainShader, camera, player.meshes[0].position, projection);
    }

    void RenderHUD(glm::mat4 &projection) {
        GLfloat startX = screenEdge - 0.05f, posY = -0.45f, width = 0.05f;
        int n = playerScore, i = 0;
        if (n == 0) {
            interface.score[0].SetPosition(startX, posY, 0.0f);
            interface.score[0].Render(hudShader, camera, projection);
            i++;
        } else {
            while (n > 0) {
                int digit = n % 10;
                n /= 10;

                interface.score[digit].SetPosition(startX - (i * width), posY, 0.0f);
                interface.score[digit].Render(hudShader, camera, projection);
                i++;
            }
        }

        interface.scoreText.SetPosition(startX - ((i + 1) * width), posY, 0.0f);
        interface.scoreText.Render(hudShader, camera, projection);

        for (int i = 0; i < player.lives; i++) {
            interface.lives[i].SetPosition((0.05f - screenEdge) + (i * (width + 0.005f)), posY, 0.0f);
            interface.lives[i].Render(hudShader, camera, projection);
        }
    }

    bool CheckMoveDown() {
        for (int i = 0; i < invaders.size(); i++) {
            if (invaders[i].PastPlayer(player.getY())) player.lives = 0;
            if (invaders[i].PastEdge()) return true;
        }

        return false;
    }

    void PlayerShoot() {
        if (fireTimer > 0.25f) {
            fireTimer = 0.0f;

            Bullet b = bullet;
            b.SetBullet(player.getPosition(), true);
            b.Move(0.0f, 0.15f, 0.0f);
            bullets.push_back(b);
        }
    }

    void EnemyShoot(int i) {
        if (i < invaders.size()) {
            Bullet b = bullet;
            b.SetBullet(invaders[i].getPosition(), false);
            b.Move(0.0f, -0.25f, 0.0f);
            bullets.push_back(b);
        }
    }
};
