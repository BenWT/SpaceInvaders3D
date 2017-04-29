// Includes
#define PI 3.14159265
#define GLM_FORCE_RADIANS

#include <iostream>
#include <fstream>
#include <sstream>
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

#include "headers/Shader.h"
#include "headers/Mesh.h"
#include "headers/Model.h"
#include "headers/Camera.h"
#include "headers/Cubemap.h"
#include "headers/Invader.h"
#include "headers/GameState.h"

// Namespaces
using namespace std;
using namespace chrono;

// Global Variables
SDL_Window* window;
SDL_GLContext context;
bool running = false;
glm::mat4 projectionMat, overlayMat;
GLfloat viewportHeight, viewportWidth;
Shader mainShader, skyboxShader;

GameState game = GameState();

// Support Functions
high_resolution_clock::time_point NowTime() {
	return chrono::high_resolution_clock::now();
}
double TimeSinceLastFrame(high_resolution_clock::time_point frameTime) {
	return (duration_cast<microseconds>(NowTime() - frameTime).count()) / 1000000.0;
}
void toggleFullScreen() {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	bool toggle = SDL_GetWindowFlags(window) & FullscreenFlag;
	SDL_SetWindowFullscreen(window, toggle ? 0 : FullscreenFlag);
}
void SizeWindow() {
	int w, h;
	w = SDL_GetWindowSurface(window)->w;
	h = SDL_GetWindowSurface(window)->h;

	glViewport(0.0f, 0.0f, w, h);
	projectionMat = glm::perspective(glm::radians(45.0f), float(w) / float(h), 0.1f, 100.0f);
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
void Update(double deltaTime);
void Render();
void LoadAssets();
void GenerateGame(bool firstGenerate);

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
	mainShader = Shader("assets/shaders/vert.vs", "assets/shaders/frag.fs");
	skyboxShader = Shader("assets/shaders/vert_cube.vs", "assets/shaders/frag_cube.fs");

    // Preserve Aspect
    SizeWindow();
	overlayMat = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, -1.0f, 100.0f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
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
	double deltaTime = 0;

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
			case SDL_MOUSEBUTTONDOWN: break;
			case SDL_KEYUP: break;
			case SDL_KEYDOWN:
				if (k == SDLK_ESCAPE) running = false;
				if (k == SDLK_RETURN) toggleFullScreen();
			break;

			case SDL_QUIT:
				SDL_Log("Program quit.");
				running = false;
			break;
		}
	}
}

void Update(double deltaTime) {
	game.Update(deltaTime);
}

void Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	game.Render(mainShader, skyboxShader, projectionMat);

	SDL_GL_SwapWindow(window);
}

void CALLBACK_MoveDown() {
	for (int i = 0; i < game.invaders.size(); i++) {
		game.invaders[i].Move(0.0f, -0.25f, 0.0f);
		game.invaders[i].moveRight = !game.invaders[i].moveRight;
	}
}

void LoadAssets() {
	int rows = 5, columns = 10;
	float startX = -4.5f, startY = 3.5f, diffX = 0.6f, diffY = -0.6f;

	game.skybox = Cubemap(true);

	game.player = Model("assets/models/player_ship.FBX", "assets/textures/rick.png");
	game.player.Move(0.0f, -3.0f, 0.0f);
	game.player.Rotate(0.0f, 180.0f, 0.0f);
	game.player.Scale(0.02f);

	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			Invader* inv = new Invader("assets/models/cube.FBX", "assets/textures/ainsley.png", startX, CALLBACK_MoveDown);
			inv->Move(startX + i * diffX, startY + j * diffY, 0.0f);
			inv->Rotate(0.0f);
			inv->Scale(0.05f);
			game.invaders.push_back(*inv);
			delete inv;
		}
	}
}

void GenerateGame(bool firstGenerate) {}
