#pragma once
#include "Core.h"

#include "Window.h"
#include "Engine/LayerStack.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicatonEvent.h"

#include "Engine/imGui/imGuiLayer.h"

#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/VertexArray.h"

#include "Engine/Renderer/OrthographicCamera.h"

#include "Engine/Core/TimeStamp.h"

namespace Engine {

	class ENGINE_API Application{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		imGuiLayer* m_ImGuiLayer;
		bool m_Running= true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};
	//To be defined in client
	Application* CreateApplication();
}

