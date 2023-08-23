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
	m_Texture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");

}

void SandBox2D::OnDetach()
{
}

void SandBox2D::OnUpdate(Engine::TimeStamp ts)
{
	m_CameraController.OnUpdate(ts);

	Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Engine::RenderCommand::Clear();

	Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Engine::Renderer2D::DrawQuad({ 0.0f,0.0f }, {1.0,1.0},m_SquareColor);
	Engine::Renderer2D::DrawQuad({ -0.05f,0.05f, -0.2f }, {1.0,1.0},m_Texture);

	Engine::Renderer2D::EndScene();
}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void SandBox2D::OnEvent(Engine::Event& e)
{
	m_CameraController.OnEvent(e);
}
