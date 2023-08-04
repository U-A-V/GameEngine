#pragma once
#ifdef EG_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();
int main(int argc, char** argv) {
	printf("Engine started");
	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}

#endif