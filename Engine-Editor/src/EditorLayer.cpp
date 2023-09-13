#include "EditorLayer.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Ulit/PlatformUtil.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {

	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f/720.0f){
	}

	void EditorLayer::OnAttach()
	{	
		EG_PROFILE_FUNCTION();

		m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");

		FrameBufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;

		m_FrameBuffer = FrameBuffer::Create(fbSpec);
	
		m_ActiveScene = CreateRef<Scene>();
		auto square = m_ActiveScene->CreateEntity("Green Square");

		square.AddComponent<SpriteRendererComponent>( glm::vec4{ 0.0f,1.0f,0.0f,1.0f });

		auto redSquare = m_ActiveScene->CreateEntity("Red Square");

		redSquare.AddComponent<SpriteRendererComponent>( glm::vec4{ 1.0f,0.0f,0.0f,1.0f });



		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondaryCamera = m_ActiveScene->CreateEntity("Camera B");
		auto& cc = m_SecondaryCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity {
		public:
			void OnCreate() {
				//std::cout << "Camera:oncreate!!" << std::endl;
			}
			void OnDestroy(){}
			void OnUpdate(TimeStamp ts) {
				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;
				if (Input::IsKeyPressed(EG_KEY_A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(EG_KEY_D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(EG_KEY_W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(EG_KEY_S))
					translation.y -= speed * ts;
				//std::cout << "Timestep: " << ts << std::endl;

			}
		};
		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize("assets/scenes/example.engine");
	}

	void EditorLayer::OnDetach()
	{
		EG_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(TimeStamp ts)
	{
		EG_PROFILE_FUNCTION();
		
		//resize
		if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x , (uint32_t)m_ViewportSize.y);
		}

		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);
		
		Renderer2D::ResetStats();
		m_FrameBuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

	#if 1
		static float rotation = 0.0f;
		rotation += ts * 2.0f;

		Renderer2D::BeginScene(m_CameraController.GetCamera());

		m_ActiveScene->OnUpdate(ts);

		Renderer2D::EndScene();


		m_FrameBuffer->Unbind();
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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSize;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New","Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();
				if (ImGui::MenuItem("Exit", "Atl+F4")) Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Stats");
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertex Count: %d", stats.GetTotalVertexCount());
		ImGui::Text("Index Count: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("ViewPort");
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		if (m_ViewportSize != *(glm::vec2*)&viewportPanelSize && viewportPanelSize.x>0 && viewportPanelSize.y>0) {

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
		}

		uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x,m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
		ImGui::End();
		ImGui::PopStyleVar();


		ImGui::End();
	
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(EG_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}
	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e) {

		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(EG_KEY_LEFT_CONTROL) || Input::IsKeyPressed(EG_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(EG_KEY_LEFT_CONTROL) || Input::IsKeyPressed(EG_KEY_RIGHT_CONTROL);
		switch (e.GetKeyCode()) {
		case EG_KEY_N:
		{
			if (control)
				NewScene();
			break;
		}
		case EG_KEY_O:
		{
			if (control)
				OpenScene();
			break;
		}
		case EG_KEY_S:
		{
			if (control && shift)
				SaveSceneAs();
			break;
		}
		}
	}


	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Scene (*.engine)\0*.engine\0");
		if (!filepath.empty()) {
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Scene (*.engine)\0*.engine\0");
		if (!filepath.empty()) {

			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

}