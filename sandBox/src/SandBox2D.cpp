#include "SandBox2D.h"
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"


SandBox2D::SandBox2D()
	:Layer("SandBox2D"), m_CameraController(1280.0f/720.0f){
}

void SandBox2D::OnAttach()
{
	EG_PROFILE_SCOPE("CameraController::Onupdate");

	m_Texture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
	 
}

void SandBox2D::OnDetach()
{
	EG_PROFILE_SCOPE("CameraController::Onupdate");

}

void SandBox2D::OnUpdate(Engine::TimeStamp ts)
{
	EG_PROFILE_FUNCTION();
	{
		m_CameraController.OnUpdate(ts);
		

	}

	{
		EG_PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::RenderCommand::Clear();
	}

	{
		EG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({ 0.0f,0.0f }, {1.0f,1.0f},m_SquareColor);
		Engine::Renderer2D::DrawRotatedQuad({ 0.0f,0.0f }, {1.5f,1.5f},glm::radians(45.0f),m_Texture);
		Engine::Renderer2D::DrawRotatedQuad({ 0.0f,0.0f }, {1.5f,1.5f},glm::radians(30.0f),m_SquareColor);
		Engine::Renderer2D::DrawQuad({ 0.0f,0.0f, -0.2f }, {10.0f,10.0f},m_Texture, 10.0f);

		Engine::Renderer2D::EndScene();
	}
}

void SandBox2D::OnImGuiRender()
{
	EG_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void SandBox2D::OnEvent(Engine::Event& e)
{
	m_CameraController.OnEvent(e);
}
