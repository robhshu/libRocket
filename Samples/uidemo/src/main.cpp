#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include <Shell.h>
#include <Input.h>

#include "EventInstancer.h"
#include "EventManager.h"

#if defined ROCKET_PLATFORM_WIN32
#include <windows.h>
#define ROCKET_MAIN			int APIENTRY WinMain(HINSTANCE, HINSTANCE, char*, int)
#else
#define ROCKET_MAIN			int main(int, char**)
#endif

Rocket::Core::Context* context = NULL;

float update_time = 0.0f;
const float capped_framerate = 1.0f / 60.0f;

void GameLoop()
{
	const float tick_time = Rocket::Core::GetSystemInterface()->GetElapsedTime();
	
	// Cap the time between frames to our desired framerate (60FPS)
	const float dt = Rocket::Core::Math::ClampUpper(tick_time-update_time, capped_framerate);
	
	// Animate with our delta time
	context->UpdateWithAnimation(dt);

	glClear(GL_COLOR_BUFFER_BIT);
	context->Render();
	Shell::FlipBuffers();

	// Set the last update time
	update_time = tick_time;
}

ROCKET_MAIN
{
	// Generic OS initialisation, creates a window and attaches OpenGL.
	if (!Shell::Initialise("../Samples/uidemo/assets/") ||
		!Shell::OpenWindow("libRocket Demo", true))
	{
		Shell::Shutdown();
		return -1;
	}

	// Rocket initialisation.
	ShellRenderInterfaceOpenGL opengl_renderer;
	Rocket::Core::SetRenderInterface(&opengl_renderer);
    opengl_renderer.SetViewport(1024,768);

	ShellSystemInterface system_interface;
	Rocket::Core::SetSystemInterface(&system_interface);

	Rocket::Core::Initialise();
	// Initialise the Rocket Controls library.
	Rocket::Controls::Initialise();

	// Create the main Rocket context and set it on the shell's input layer.
	context = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(1024, 768));
	if (context == NULL)
	{
		Rocket::Core::Shutdown();
		Shell::Shutdown();
		return -1;
	}

	// Initialise the Rocket debugger.
	Rocket::Debugger::Initialise(context);
	Input::SetContext(context);

	// Load the font faces required for Invaders.

	char buf[256];
	GetCurrentDirectory(256, buf);

	Shell::LoadFonts("assets/");

	// Initialise the event instancer and handlers.
	EventInstancer* event_instancer = new EventInstancer();
	Rocket::Core::Factory::RegisterEventListenerInstancer(event_instancer);
	event_instancer->RemoveReference();

	if( EventManager::LoadWindow("animation") )
	{
#if defined(ANIM_DEBUG)
		update_time = Rocket::Core::GetSystemInterface()->GetElapsedTime();
#endif

		Shell::EventLoop(GameLoop);
	}
	else
	{
		ROCKET_BREAK;
	}
	
	// Release the event handlers.
	EventManager::Shutdown();

	// Shutdown Rocket.
	context->RemoveReference();
	Rocket::Core::Shutdown();

	Shell::CloseWindow();
	Shell::Shutdown();
	
	return 0;
}
