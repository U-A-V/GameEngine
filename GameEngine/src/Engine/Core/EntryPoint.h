#pragma once
#ifdef EG_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();
int main(int argc, char** argv) {

	Engine::Log::init();
	
	EG_PROFILE_BEGIN_SESSION("Startup","EngineProfile.Startup.json");
	auto app = Engine::CreateApplication();
	EG_PROFILE_END_SESSION();

	EG_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile.Runtime.json");
	app->Run();
	EG_PROFILE_END_SESSION();

	EG_PROFILE_BEGIN_SESSION("Shutdown","EngineProfile.Shutdown.json");
	delete app;
	EG_PROFILE_END_SESSION();
}

#endif