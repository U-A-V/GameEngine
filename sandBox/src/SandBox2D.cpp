#include "SandBox2D.h"
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles = 
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWDDDDWWWWWWWWWWDDDDWWW"
"WWWWDDDDWWWWWWWWDDDDWWWW"
"WWWWWDDDDWWWWWWDDDDWWWWW"
"WWWWWWDDDDWWWWDDDDWWWWWW"
"WWWWWWWDDDDWWDDDDWWWWWWW"
"WWWWWWWWDDDDDDDDWWWWWWWW"
"WWWWWWWWWDDDDDDWWWWWWWWW"
"WWWWWWWWWWDDDDWWWWWWWWWW"
"WWWWWWWWWWDDDDWWWWWWWWWW"
"WWWWWWWWWWDDDDWWWWWWWWWW"
"WWWWWWWWWWDDDDWWWWWWWWWW"
"WWWWWWWWWWDDDDWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
;



SandBox2D::SandBox2D()
	:Layer("SandBox2D"), m_CameraController(1280.0f/720.0f){
}

void SandBox2D::OnAttach()
{	
	EG_PROFILE_FUNCTION();

	m_Texture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
	m_SpriteSheet = Engine::Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");

	m_TextureStairs = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2,3 }, { 128,128 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / m_MapWidth;
	s_TextureMap['D'] = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6,11 }, { 128,128 });
	s_TextureMap['W'] = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11,11 }, { 128,128 });

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
	
	m_CameraController.SetZoomLevel(5.0f);
}

void SandBox2D::OnDetach()
{
	EG_PROFILE_FUNCTION();

}

void SandBox2D::OnUpdate(Engine::TimeStamp ts)
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
	if (Engine::Input::IsMouseButtonPressed(EG_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Engine::Input::GetMousePosition();
		auto width = Engine::Application::Get().GetWindow().GetWidth();
		auto height = Engine::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
#if 0
	Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());
	for (uint32_t y = 0; y < m_MapHeight; y++) {
		for (uint32_t x = 0; x < m_MapWidth; x++) {
			char tile = s_MapTiles[x + y * m_MapWidth];
			Engine::Ref<Engine::SubTexture2D> texture;
			if (s_TextureMap.find(tile) != s_TextureMap.end())
				texture = s_TextureMap[tile];
			else
				texture = m_TextureStairs;

			Engine::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f ,m_MapHeight - y - m_MapHeight / 2.0f, 0.5f }, { 1.0f,1.0f }, texture);
		}
	}
	//Engine::Renderer2D::DrawQuad({ 0.0f,0.0f,0.5f }, { 1.0f,1.0f }, m_TextureStairs);
	//Engine::Renderer2D::DrawQuad({ 1.0f,0.0f,0.5f }, { 1.0f,1.0f }, m_TextureBarrel);
	//Engine::Renderer2D::DrawQuad({ 0.5f,1.5f,0.5f }, { 1.0f,2.0f }, m_TextureTree);
	Engine::Renderer2D::EndScene();
#endif 
}

void SandBox2D::OnImGuiRender()
{
	EG_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	auto stats = Engine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertex Count: %d", stats.GetTotalVertexCount());
	ImGui::Text("Index Count: %d", stats.GetTotalIndexCount());
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
	
}

void SandBox2D::OnEvent(Engine::Event& e)
{
	m_CameraController.OnEvent(e);
}