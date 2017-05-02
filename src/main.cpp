// Includes
#define PI 3.14159265
#define GLM_FORCE_RADIANS

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <iterator>
#include <chrono>
#include <random>
#include <cmath>
#include <string>
#include <functional>

#include "SDL.h"
#include "SDL_image.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "headers/Vector.h"
#include "headers/Vertex.h"
#include "headers/Shader.h"
#include "headers/Mesh.h"
#include "headers/Model.h"
#include "headers/Camera.h"
#include "headers/Bullet.h"
#include "headers/Cubemap.h"
#include "headers/Invader.h"
#include "headers/Plane.h"
#include "headers/Player.h"
#include "headers/Barricade.h"
#include "headers/GameState.h"

// Namespaces
using namespace std;
using namespace chrono;

// Global Variables
SDL_Window* window;
SDL_GLContext context;
bool running = false;
glm::mat4 projectionMat, overlayMat, overlayView;
GLfloat viewportHeight, viewportWidth;

GameState game = GameState();

// Support Functions
high_resolution_clock::time_point NowTime() {
	return chrono::high_resolution_clock::now();
}
GLfloat TimeSinceLastFrame(high_resolution_clock::time_point frameTime) {
	return (GLfloat)(duration_cast<microseconds>(NowTime() - frameTime).count()) / 1000000.0f;
}
void toggleFullScreen() {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	Uint32 toggle = SDL_GetWindowFlags(window) & FullscreenFlag;
	SDL_SetWindowFullscreen(window, toggle ? 0 : FullscreenFlag);
}
void SizeWindow() {
	int w, h;
	w = SDL_GetWindowSurface(window)->w;
	h = SDL_GetWindowSurface(window)->h;

	glViewport(0, 0, w, h);
	projectionMat = glm::perspective(glm::radians(45.0f), float(w) / float(h), 0.1f, 100.0f);

	overlayMat = glm::ortho(0.0f, float(w) / float(h), 0.0f, 1.0f, -1.0f, 100.0f);
	overlayView = glm::translate(glm::mat4(), glm::vec3((float(w) / float(h)) / 2, 0.5f, 0.0f));

	game.screenEdge = (float(w) / float(h)) / 2;
}
string basePath;
string GetPathFromFullPath(const string& str) {
	size_t found = str.find_last_of("/\\");
	if (found == numeric_limits<size_t>::max()) return "";
	else return (str.substr(0, found) + "\\");
}
string AddBase(string path) {
	#ifdef _WIN32 // Windows reverses X and Y axis of right joystick
		return basePath + path;
	#else
		return path;
	#endif
}

// Main Functions
void ProcessInput();
void Update(GLfloat &deltaTime);
void Render();
void LoadAssets();
void GenerateGame(bool playerDied);

int main(int argc, char *argv[]) {
	// Get Base Path
	basePath = argv[0];
	basePath = GetPathFromFullPath(basePath).c_str();
	cout << basePath << endl;

    // Initialise SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL failed to initialise. \n");
		return 1;
	}

	// Initialise SDL_image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "SDL_image failed to initialise. \n");
		return 1;
	}

	// Get Display Info
	SDL_DisplayMode display;
	SDL_GetCurrentDisplayMode(0, &display);
	int x = display.w, y = display.h;

	// Create Window
	window = SDL_CreateWindow("Ben Townshend - CGP2012M - 13480634", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, x / 2, y / 2, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "SDL failed to create the window. \n");
		return 1;
	}

	// OpenGL Attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Create Context
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "SDL failed to create the OpenGL context. \n");
		return 1;
	}

	// Initialise GLEW
	GLenum rev;
	glewExperimental = GL_TRUE;
	rev = glewInit();
	if (GLEW_OK != rev) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "SDL failed to initialise GLEW. \n");
		return 1;
	}

	// Create shader program
	game.mainShader = Shader("assets/shaders/vert.vs", "assets/shaders/frag.fs");
	game.skyboxShader = Shader("assets/shaders/vert_cube.vs", "assets/shaders/frag_cube.fs");
	game.hudShader = Shader("assets/shaders/vert_hud.vs", "assets/shaders/frag_hud.fs");

    // Preserve Aspect
    SizeWindow();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	LoadAssets();
	GenerateGame(true);

    // Game Loop
	running = true;
	high_resolution_clock::time_point frameTime = NowTime();
	GLfloat deltaTime = 0;

	while (running) {
		deltaTime =  TimeSinceLastFrame(frameTime);
		frameTime = NowTime();

		ProcessInput();
		Update(deltaTime);
		Render();
	}

	// Cleanup on Close
    SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		SDL_Keycode k = event.key.keysym.sym;

		switch (event.type) {
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					SizeWindow();
				}
			break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) game.PlayerShoot();
				// if (event.button.button == SDL_BUTTON_RIGHT) gameState.EnemyFire(0);
			break;
			case SDL_KEYUP:
				if (k == SDLK_w) game.input.up = false;
				if (k == SDLK_s) game.input.down = false;
				if (k == SDLK_a) game.input.left = false;
				if (k == SDLK_d) game.input.right = false;
			break;
			case SDL_KEYDOWN:
				if (k == SDLK_ESCAPE) running = false;
				if (k == SDLK_RETURN) toggleFullScreen();
				if (k == SDLK_w) game.input.up = true;
				if (k == SDLK_s) game.input.down = true;
				if (k == SDLK_a) game.input.left = true;
				if (k == SDLK_d) game.input.right = true;
			break;

			case SDL_QUIT:
				SDL_Log("Program quit.");
				running = false;
			break;
		}
	}
}

void Update(GLfloat &deltaTime) {
	if (game.player.lives <= 0) {
		GenerateGame(true);
	}
	if (game.invaders.size() <= 0) {
		GenerateGame(false);
	}

	game.Update(deltaTime);
}

void Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	game.Render(projectionMat);
	game.RenderHUD(overlayMat * overlayView);

	SDL_GL_SwapWindow(window);
}

void LoadAssets() {
	game.skybox = Cubemap(true);

	game.lBarricade = Model("assets/models/cube.FBX", "assets/textures/rick.png");
	game.lBarricade.Move(-5.5f, 0.0f, 0.0f);
	game.lBarricade.Scale(0.05f, 0.8f, 0.05f);
	game.rBarricade = Model("assets/models/cube.FBX", "assets/textures/rick.png");
	game.rBarricade.Move(5.5f, 0.0f, 0.0f);
	game.rBarricade.Scale(0.05f, 0.8f, 0.05f);

	game.player = Player("assets/models/player_ship.obj", "assets/textures/player_ship.png", -4.5f);
	game.player.Scale(0.02f);

	game.invader = Invader("assets/models/cube.FBX", "assets/textures/ainsley.png");
	game.invader.Scale(0.05f);

	game.bullet = Bullet("assets/models/cube.FBX", "assets/textures/rick.png");
	game.bullet.Scale(0.02f);

	game.barricade = Barricade("assets/models/barricade.FBX", "assets/textures/rick.png");
	game.barricade.Rotate(90.0f, 0.0f, 0.0f);
	game.barricade.Scale(0.05f, 0.05f, 0.035f);

	game.interface.scoreText = Plane("assets/models/plane.FBX", "assets/textures/numbers/score.png", 0.2f, 0.05f);
	game.interface.scoreText.Move(0.05f, 0.45f, 0.0f);

	game.interface.score[0] = Plane("assets/models/plane.FBX", "assets/textures/numbers/0.png", 0.05f, 0.05f);
	game.interface.score[1] = Plane("assets/models/plane.FBX", "assets/textures/numbers/1.png", 0.05f, 0.05f);
	game.interface.score[2] = Plane("assets/models/plane.FBX", "assets/textures/numbers/2.png", 0.05f, 0.05f);
	game.interface.score[3] = Plane("assets/models/plane.FBX", "assets/textures/numbers/3.png", 0.05f, 0.05f);
	game.interface.score[4] = Plane("assets/models/plane.FBX", "assets/textures/numbers/4.png", 0.05f, 0.05f);
	game.interface.score[5] = Plane("assets/models/plane.FBX", "assets/textures/numbers/5.png", 0.05f, 0.05f);
	game.interface.score[6] = Plane("assets/models/plane.FBX", "assets/textures/numbers/6.png", 0.05f, 0.05f);
	game.interface.score[7] = Plane("assets/models/plane.FBX", "assets/textures/numbers/7.png", 0.05f, 0.05f);
	game.interface.score[8] = Plane("assets/models/plane.FBX", "assets/textures/numbers/8.png", 0.05f, 0.05f);
	game.interface.score[9] = Plane("assets/models/plane.FBX", "assets/textures/numbers/9.png", 0.05f, 0.05f);

	game.interface.lives[0] = Plane("assets/models/plane.FBX", "assets/textures/heart.png", 0.05f, 0.05f);
	game.interface.lives[1] = Plane("assets/models/plane.FBX", "assets/textures/heart.png", 0.05f, 0.05f);
	game.interface.lives[2] = Plane("assets/models/plane.FBX", "assets/textures/heart.png", 0.05f, 0.05f);

	GenerateGame(false);
}

void GenerateGame(bool playerDied) {
	int rows = 4, columns = 10;
	float startX = -5.0f, startY = 3.5f, diffX = 0.6f, diffY = -0.6f;

	float barricadeX = -4.5f, barricadeY = -2.0f, barricadeWidth = 0.55f, barricadeHeight = 0.4f;

	game.invaders.clear();
	game.bullets.clear();

	if (playerDied) {
		game.playerScore = 0;
		game.barricades.clear();
		game.player.Reset();

		for (int i = 0; i < columns; i++) {
			for (int j = 0; j < rows; j++) {
				Invader inv = game.invader;
				inv.Move(startX + i * diffX, startY + j * diffY, 0.0f);
				inv.SetEdge(startX);
				game.invaders.push_back(inv);
			}
		}

		for (int i = 0; i < 17; i++) {
			for (int j = 0; j < 3; j++) {
				if (i != 3 && i != 8 && i != 13) {
					Barricade bar = game.barricade;
					bar.Move(barricadeX + i * barricadeWidth, barricadeY + j * barricadeHeight, 0.0f);
					game.barricades.push_back(bar);
				}
			}
		}
	} else {
		for (int i = 0; i < columns; i++) {
			for (int j = 0; j < rows; j++) {
				Invader inv = game.invader;
				inv.Move(startX + i * diffX, startY + j * diffY, 0.0f);
				inv.SetEdge(startX);
				game.invaders.push_back(inv);
			}
		}
	}
}
