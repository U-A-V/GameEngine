#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Application.h"
#ifdef EG_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication(ApplicationCommandLineArgs args);
int main(int argc, char** argv) {

	Engine::Log::init();
	
	EG_PROFILE_BEGIN_SESSION("Startup","EngineProfile.Startup.json");
	auto app = Engine::CreateApplication({ argc, argv });
	EG_PROFILE_END_SESSION();

	EG_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile.Runtime.json");
	app->Run();
	EG_PROFILE_END_SESSION();

	EG_PROFILE_BEGIN_SESSION("Shutdown","EngineProfile.Shutdown.json");
	delete app;
	EG_PROFILE_END_SESSION();
}

#endif