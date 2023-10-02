#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Core/Window.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicatonEvent.h"

#include "Engine/imGui/imGuiLayer.h"

#include "Engine/Core/TimeStamp.h"

namespace Engine {
	struct ApplicationCommandLineArgs {
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const {
			EG_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};
	class ENGINE_API Application{
	public:
		Application(const std::string& name = "App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();


		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		void Close();

		imGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		static Application& Get() { return *s_Instance; }
		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
		void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ApplicationCommandLineArgs m_CommandLineArgs;
		std::unique_ptr<Window> m_Window;
		imGuiLayer* m_ImGuiLayer;
		bool m_Running= true;
		bool m_Minimized=false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};
	//To be defined in client
	Application* CreateApplication();
	Application* CreateApplication(ApplicationCommandLineArgs args);
}

