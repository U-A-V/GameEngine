#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Core/Window.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicatonEvent.h"

#include "Engine/imGui/imGuiLayer.h"

#include "Engine/Core/TimeStamp.h"

namespace Engine {

	class ENGINE_API Application{
	public:
		Application(const std::string& name = "App");
		virtual ~Application();


		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		void Close();

		imGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		static Application& Get() { return *s_Instance; }

		void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
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
}

