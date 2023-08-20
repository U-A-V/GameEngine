#include "hzpch.h"
#include "Application.h"
#include "Input.h"

#include "Engine/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Engine {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		EG_CORE_ASSERT(s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new imGuiLayer();
		PushOverlay(m_ImGuiLayer);

		

	}
	Application::~Application() {

	}
	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();

	}
	void Application::PushOverlay(Layer* overlay) {
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();

	}
	void Application::OnEvent(Event& e) {

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled)	break;
		}
	}

	void Application::Run() {

		while (m_Running) {

			float time = (float)glfwGetTime(); // Platform::GetTime()
			TimeStamp timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			for (Layer* layer : m_LayerStack)	layer->OnUpdate(timestep);

			auto [x, y] = Input::GetMousePosition();
			//EG_CORE_TRACE("{0}, {1}", x, y);
			
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)	layer->OnImGuiRender();
			m_ImGuiLayer->End();
			
			
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
}