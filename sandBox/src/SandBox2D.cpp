#include "SandBox2D.h"
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

#include <chrono>

template<typename Fn>
class Timer {
public:
	Timer(const char* name, Fn&& func)
		:m_Name(name),m_Func(func), m_Stopped(false) {
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		if (!m_Stopped)	Stop();
	}

	void Stop() {
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;
		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	}

private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) {m_ProfileResults.push_back(profileResult);})

SandBox2D::SandBox2D()
	:Layer("SandBox2D"), m_CameraController(1280.0f/720.0f){
}

void SandBox2D::OnAttach()
{
	m_Texture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
	 
}

void SandBox2D::OnDetach()
{
}

void SandBox2D::OnUpdate(Engine::TimeStamp ts)
{
	PROFILE_SCOPE("SandBox2D::Onupdate");
	{
		PROFILE_SCOPE("CameraController::Onupdate");
		m_CameraController.OnUpdate(ts);
		

	}

	{
		PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({ 0.0f,0.0f }, {1.0f,1.0f},m_SquareColor);
		Engine::Renderer2D::DrawQuad({ 0.0f,0.0f, -0.2f }, {10.0f,10.0f},m_Texture);

		Engine::Renderer2D::EndScene();
	}
}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	for (auto& result : m_ProfileResults) {
		char label[50];
		strcpy(label, " %.3fms ");
		strcat(label, result.Name);
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();
	ImGui::End();
}

void SandBox2D::OnEvent(Engine::Event& e)
{
	m_CameraController.OnEvent(e);
}
