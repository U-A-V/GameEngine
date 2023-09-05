#include "EditorLayer.h"
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {

EditorLayer::EditorLayer()
	:Layer("EditorLayer"), m_CameraController(1280.0f/720.0f){
}

void EditorLayer::OnAttach()
{	
	EG_PROFILE_FUNCTION();

	m_Texture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");

	Engine::FrameBufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;

	m_FrameBuffer = Engine::FrameBuffer::Create(fbSpec);
	
	m_CameraController.SetZoomLevel(5.0f);
}

void EditorLayer::OnDetach()
{
	EG_PROFILE_FUNCTION();

}

void EditorLayer::OnUpdate(Engine::TimeStamp ts)
{
	EG_PROFILE_FUNCTION();
	{
		m_CameraController.OnUpdate(ts);
		

	}
	Engine::Renderer2D::ResetStats();
	{
		EG_PROFILE_SCOPE("Renderer Prep");
		m_FrameBuffer->Bind();
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::RenderCommand::Clear();
	}
#if 1
	{
		static float rotation = 0.0f;
		rotation += ts * 2.0f;

		EG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({ 0.0f,0.0f }, { 1.0f,1.0f }, m_SquareColor);
		Engine::Renderer2D::DrawRotatedQuad({ 0.0f,0.0f }, { 1.5f,1.5f }, 45.0f +rotation, m_Texture);
		Engine::Renderer2D::DrawRotatedQuad({ 0.0f,0.0f }, { 1.5f,1.5f }, 30.0f -rotation, m_SquareColor);
		Engine::Renderer2D::DrawQuad({ 0.0f,0.0f, -0.2f }, { 20.0f,20.0f }, m_Texture, 10.0f);

		Engine::Renderer2D::EndScene();


		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f) {
				glm::vec4 color = { (x + 5.0f) / 10.0f,0.4f, (y + 5.0f) / 10.0f, 0.8f };
				Engine::Renderer2D::DrawQuad({ x,y }, { 0.45f,0.45f }, color);
			}
		}
		Engine::Renderer2D::EndScene();
		m_FrameBuffer->Unbind();
	}
#endif
}

void EditorLayer::OnImGuiRender()
{
	EG_PROFILE_FUNCTION();
	static bool dockSpacOpen = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen){
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockSpacOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

		// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Exit")) Engine::Application::Get().Close();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::Begin("Settings");
	auto stats = Engine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertex Count: %d", stats.GetTotalVertexCount());
	ImGui::Text("Index Count: %d", stats.GetTotalIndexCount());
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();

	ImGui::Image((void*)textureID, ImVec2{ 1280.0f,720.0f }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
	ImGui::End();

	ImGui::End();
	
}

void EditorLayer::OnEvent(Engine::Event& e)
{
	m_CameraController.OnEvent(e);
}
}