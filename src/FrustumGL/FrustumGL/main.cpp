#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <SDL_opengl.h>
#include <filesystem>

#include "Defines.h"
#include "SceneLoader.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "cCamera.h"
#include "Frustum.h"
#include "RenderObject.h"
#include "BVHNode.h"

/*! \mainpage View Frustum Culling using BVH
 *
 * \section Intro
 *
 * This project implements simple frustum culling accelerated using a BVH data structure. Completed for the DPG course at CTU FEL.
 *
 * \section running_sec Installation
 *
 * Make sure you have the binary file and shaders in the same folder as an Assets folder where susanne.obj and cube.obj is located
 * If you want to test more complex scenes, copy the additional OBJ files to the Assets folder.
 *
 * \section controls_sec Controls
 *
 * Controls are:
 *  - Mouse for camera rotation
 *  - Arrow keys/WSAD for movement
 *  - Q and E for up/down movement
 *  - holding left shift for faster speed
 *  - P and O for visualising the BVH structure
 *  - C to switch the controls between the visualised frustum and camera (and to swap whether animation is controlling the frustum or the view camera)
 *  - X to switch the culling from the camera frustum to the visualised frustum
 *  - Z(Y) to teleport view camera to the position of the visualised frustum
 *  - R to reset animation
 */

#define WIDTH 800
#define HEIGHT 600
#define BACKGROUND_COLOR 0.2f, 0.3f, 0.3f, 1.0f

glm::vec3 bezier(float t, std::array<glm::vec3, 4> Pts)
{
	float B0, B1, B2, B3;
	static glm::vec3 hlp;

	B0 = (1 - t) * (1 - t) * (1 - t);
	B1 = 3 * t * (1 - t) * (1 - t);
	B2 = 3 * t * t * (1 - t);
	B3 = t * t * t;

	hlp = Pts[0] * B0 + Pts[1] * B1 + Pts[2] * B2 + Pts[3] * B3;
	return hlp;
}

int main(int argc, char* argv[])
{
	/////////////////////////////
	//PARSE THE CMDLINE ARGUMENTS
	std::string scenefilepath = "../../Assets/lotsOfMonkeys.txt";
	if (argc < 3 || std::string(argv[1]) != "--scene")
	{
		std::cerr << "Usage " << argv[0] << " --scene [PATH TO SCENE .txt FILE]" << std::endl;
		return EXIT_FAILURE;
	}
	scenefilepath = argv[2];
	std::cout << "Will load scene " << scenefilepath << std::endl;

	///////////////////////////
	//INITIALIZE SDL AND OPENGL
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	//Create the SDL window
	SDL_Window* window = SDL_CreateWindow
	("An SDL2 window", // window's title
	 500, 100, // coordinates on the screen, in pixels, of the window's upper left corner
	 WIDTH, HEIGHT, // window's length and height in pixels  
	 SDL_WINDOW_OPENGL);
	SDL_ShowCursor(false);
	SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to init glew!" << std::endl;
		return EXIT_FAILURE;
	}
	glViewport(0, 0, WIDTH, HEIGHT);

	// Z-buffer rendering
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// Backface culling
	glEnable(GL_CULL_FACE);

	// Create the current VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//////////////////////////
	//LOAD AND COMPILE SHADERS
	std::cout << "loading shaders" << std::endl;
	ShaderProgram shaders("vertex.glsl", "fragment.glsl");
	ShaderProgram AABBshaders("vertex-AABB.glsl", "fragment-AABB.glsl");
	glUseProgram(shaders.programID);


	std::cout << "loading scene" << std::endl;
	////////////////
	//CREATE A SCENE
	SceneLoader scene(scenefilepath);

	////////////////
	//CREATE THE BVH
	std::cout << std::endl << "Loading " << scene.Objects.size() << " RenderObjects to the data structure" << std::endl;
	BVHNode BVHroot = BVHNode(scene.Objects);

#ifdef FRUSTUM_CULL
	std::cout << "Starting the top down Midpoint split construction" << std::endl;
	Uint64 bvhStart = SDL_GetPerformanceCounter();
	BVHroot.MidpointSplit();
	Uint64 bvhDone = SDL_GetPerformanceCounter();
	float bvhTime = (float)((bvhDone - bvhStart) / (float)SDL_GetPerformanceFrequency());
	std::cout << "Finished building BVH in " << bvhTime << "s" << std::endl;
#endif // FRUSTUM_CULL

	SDL_GL_SetSwapInterval(0); //disable Vsync, needs to happen after context creation

//////////////////////////
//CAMERA AND FRUSTUM SETUP
	cCamera viewcam(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		45, 4.0f / 3.0f, 0.5f, 2000.0f);
	Frustum viewfrust(viewcam);

	cCamera frustcam(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		45, 4.0f / 3.0f, 0.5f, 500.0f);
	Frustum frust(frustcam);

	bool animate = scene.animlen > 0;
	bool controlView = !animate; // Which camera is controlled by the user
	bool clipView = true; // Which camera frustum does the clipping

////////////////////////////
//VARIABLES USED IN MAINLOOP
	SDL_Event windowEvent;
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 START = NOW;
	Uint64 LAST = 0;
	Uint64 LASTSECOND = NOW;
	float deltaTime = 0;

	GLuint query;
	glGenQueries(1, &query);


	int BoundsLevel = -1;


	std::cout << "////////////// STARTING APPLICATION //////////////" << std::endl <<
		"Controls: WSAD/Arrow keys: Movement" << std::endl <<
		"Q+E: Movement up and down" << std::endl <<
		"P+O: Display the BVH tree structure" << std::endl <<
		"C: Switch which frustum is being controlled (Also enables animation)" << std::endl <<
		"R: Restarts animation from the beginning" << std::endl <<
		"X: Switch which frustrum is being used for culling" << std::endl <<
		"Z/Y: Snap camera to the other frustum" << std::endl;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	//////////
	//MAINLOOP
	std::stringstream csvdata;
	int measurementcounter = 0;

	std::cout << "fps,frametime,drawcalls,GLTimerQuery,cputime,traversaltime" << std::endl;
	csvdata << "fps,frametime,drawcalls,GLTimerQuery,cputime,traversaltime" << std::endl;
	bool quit = false;
	unsigned int FPS = 0;
	while (!quit)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		deltaTime = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency());

		/////////////
		//FPS COUNTER
		bool display = false;
		if ((float)((NOW - LASTSECOND) * 1000 / (float)SDL_GetPerformanceFrequency() > 1000))
		{
			LASTSECOND = NOW;
			display = true;
		}



		cCamera& controlcam = controlView ? viewcam : frustcam;


		///////////////
		//HANDLE EVENTS
		SDL_PumpEvents();
		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		float speed = 1.0;
		if (keys[SDL_SCANCODE_ESCAPE])
			quit = true;
		if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT])
			speed = 40.0;
		if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
			controlcam.move(glm::vec3(0, 0, -1) * deltaTime * speed);
		if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
			controlcam.move(glm::vec3(0, 0, 1) * deltaTime * speed);
		if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
			controlcam.move(glm::vec3(-1, 0, 0) * deltaTime * speed);
		if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
			controlcam.move(glm::vec3(1, 0, 0) * deltaTime * speed);
		if (keys[SDL_SCANCODE_Q])
			controlcam.move(glm::vec3(0, -1, 0) * deltaTime * speed);
		if (keys[SDL_SCANCODE_E])
			controlcam.move(glm::vec3(0, 1, 0) * deltaTime * speed);

		while (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT)
			{
				quit = true;
			}
			if (windowEvent.type == SDL_KEYDOWN)
			{
				if (windowEvent.key.keysym.sym == SDLK_p)
					BoundsLevel++;
				if (windowEvent.key.keysym.sym == SDLK_o && BoundsLevel >= 0)
					BoundsLevel--;
				if (windowEvent.key.keysym.sym == SDLK_c)
				{
					std::cout << "Switching control" << std::endl;
					controlView = !controlView;
				}
				if (windowEvent.key.keysym.sym == SDLK_x)
				{
					std::cout << "Switching clip" << std::endl;
					clipView = !clipView;
				}
				if (windowEvent.key.keysym.sym == SDLK_m)
				{
					std::cout << "Coordinates to mark: " << controlcam.position.x << " " << controlcam.position.y << " " << controlcam.position.z << std::endl;
				}
				if (windowEvent.key.keysym.sym == SDLK_z || windowEvent.key.keysym.sym == SDLK_y)
				{
					std::cout << "Teleporting to view of frustum" << std::endl;
					viewcam.position = frustcam.position;
					viewcam.direction = frustcam.direction;
					viewcam.upVector = frustcam.upVector;
				}
				if (windowEvent.key.keysym.sym == SDLK_r)
				{
					std::cout << "Animation restarted" << std::endl;
					std::cout << "fps,frametime,drawcalls,GLTimerQuery,cputime,traversaltime" << std::endl;
					csvdata = std::stringstream();
					csvdata << "fps,frametime,drawcalls,GLTimerQuery,cputime,traversaltime" << std::endl;
					START = NOW;
				}
			}
			if (windowEvent.type == SDL_MOUSEMOTION)
			{
				if (!animate || !controlView)
					SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
				controlcam.rotate(windowEvent.motion.xrel * deltaTime * -2, windowEvent.motion.yrel * deltaTime * -2);
			}
		}


		float t = 0;
		if (animate)
		{
			float timeFromStart = (float)((NOW - START) / (float)SDL_GetPerformanceFrequency());
			t = (timeFromStart / scene.animlen);
			if (t > 1)
			{
				t -= floor(t);
				std::cout << "Animation restarted" << std::endl;
				std::cout << "fps,frametime,drawcalls,GLTimerQuery,cputime,traversaltime" << std::endl;

				////////////////////
				//SAVE CSV DATA FILE
				std::string fpath = scenefilepath + ".measures";
			#ifdef FRUSTUM_CULL
				#ifdef MASKING
					fpath += "_masked";
					#endif
				#ifdef COHERENCY
					fpath += "_coherency";
				#endif
			#else
				fpath += "_CullingDisabled";
			#endif
				std::string cmd = "mkdir \"" + fpath + "\"";
				system(cmd.c_str());
				std::ofstream f;
				f.open(fpath + "/measure" + std::to_string(measurementcounter++) + ".csv");
				f << csvdata.str();
				csvdata = std::stringstream();
				csvdata << "fps,frametime,drawcalls,GLTimerQuery,cputime,traversaltime" << std::endl;
				START = NOW;

				if (scene.animcount > 0 && --scene.animcount == 0)
					quit = true;
			}
			cCamera& animatecam = controlView ? frustcam : viewcam;
			animatecam.position = bezier(t, scene.bezier);
			animatecam.direction = -glm::normalize(animatecam.position - bezier(t + 0.01, scene.bezier));
		}


		viewfrust = Frustum(viewcam);
		frust = Frustum(frustcam);

		glClearColor(BACKGROUND_COLOR);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/////////////
		//DRAW OPENGL
		Uint64 cpuSTART, cpuEND;
		float cputime;
		if (display)
		{
			glBeginQuery(GL_TIME_ELAPSED, query);
			cpuSTART = SDL_GetPerformanceCounter();
		}

		BVHroot.Draw(viewcam, shaders, (clipView ? &viewfrust : &frust), 0b111111);

		if (display)
		{
			glEndQuery(GL_TIME_ELAPSED);
			cpuEND = SDL_GetPerformanceCounter();
			cputime = (float)((cpuEND - cpuSTART) * 1000.0 / (float)SDL_GetPerformanceFrequency());
		}

		if (BoundsLevel >= 0)
		{
			BVHroot.DrawBounds(viewcam, AABBshaders, BoundsLevel);
		}
		frust.Draw(viewcam, AABBshaders);

		SDL_GL_SwapWindow(window);

		/////////////
		//FPS COUNTER
		FPS++;
		if (display)
		{
			int done = 0;
			while (!done) //Wait until query result available
			{
				glGetQueryObjectiv(query,
								   GL_QUERY_RESULT_AVAILABLE,
								   &done);
			}
			GLuint64 GLTimerQuery;
			glGetQueryObjectui64v(query, GL_QUERY_RESULT, &GLTimerQuery);

			Uint64 traversalSTART, traversalEND, callcount;
			float traversaltime;
			traversalSTART = SDL_GetPerformanceCounter();
			callcount = BVHroot.CountCalls((clipView ? &viewfrust : &frust), 0b111111);
			traversalEND = SDL_GetPerformanceCounter();
			traversaltime = (float)((traversalEND - traversalSTART) * 1000.0 / (float)SDL_GetPerformanceFrequency());
			std::cout << FPS << "," << deltaTime * 1000.0 <<
				  "," << callcount << "," << GLTimerQuery / 1000000.0 <<
				  "," << cputime << "," << traversaltime << std::endl;
			csvdata << FPS << "," << deltaTime * 1000.0 <<
				"," << callcount << "," << GLTimerQuery / 1000000.0 <<
				"," << cputime << "," << traversaltime << std::endl;
			FPS = 0;
		}
	}

	/////////////
	//SDL CLEANUP
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}
