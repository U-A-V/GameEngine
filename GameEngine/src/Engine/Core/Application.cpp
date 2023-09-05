#include "hzpch.h"
#include "Application.h"
#include "Input.h"
#include "Engine/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Engine {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		EG_PROFILE_FUNCTION();
		EG_CORE_ASSERT(s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		m_ImGuiLayer = new imGuiLayer();
		PushOverlay(m_ImGuiLayer);

		

	}
	Application::~Application() {
		EG_PROFILE_FUNCTION();
		Renderer::Shutdown();
	}
	void Application::PushLayer(Layer* layer) {
		EG_PROFILE_FUNCTION();


		m_LayerStack.PushLayer(layer);
		layer->OnAttach();

	}
	void Application::PushOverlay(Layer* overlay) {
		EG_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();

	}
	void Application::Close() {
		m_Running = false;
	}

	void Application::OnEvent(Event& e) {
		EG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled)	break;
		}
	}

	void Application::Run() {
		EG_PROFILE_FUNCTION();


		while (m_Running) {
			EG_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime(); // Platform::GetTime()
			TimeStamp timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) {
				{
					EG_PROFILE_SCOPE("LayerStack::OnUpdate");

					for (Layer* layer : m_LayerStack)	layer->OnUpdate(timestep);
				}
				m_ImGuiLayer->Begin();
				{
					EG_PROFILE_SCOPE("LayerStack::OnImGuiRender");
					for (Layer* layer : m_LayerStack)	layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}
			
			
			
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}	
	
	bool Application::OnWindowResize(WindowResizeEvent& e) {
		EG_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

		return false;
	}
}