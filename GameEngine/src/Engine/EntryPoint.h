#pragma once
#ifdef EG_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();
int main(int argc, char** argv) {

	Engine::Log::init();
	EG_CORE_WARN("Initialize Log!");
	int a = 5;
	EG_CLIENT_INFO("Hp :{0}",a);

	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}

#endif