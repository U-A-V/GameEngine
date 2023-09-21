#include "EditorLayer.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Ulit/PlatformUtil.h"

#include "Engine/Math/Math.h"

#include <imgui.h>
#include <ImGuizmo.h>

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
		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER,  FrameBufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;

		m_FrameBuffer = FrameBuffer::Create(fbSpec);
	
		m_ActiveScene = CreateRef<Scene>();
		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x , (uint32_t)m_ViewportSize.y);
		}

		if (m_ViewportFocused) {
			m_CameraController.OnUpdate(ts);
			m_EditorCamera.OnUpdate(ts);
		}
		
		Renderer2D::ResetStats();
		m_FrameBuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		//clear our entityID attachement to -1
		m_FrameBuffer->ClearColorAttachment(1, -1);

	#if 1
		//update scene
		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
		//m_ActiveScene->OnUpdateRuntime(ts);

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;

		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;

		int mouseX = int(mx);
		int mouseY = int(my);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
			int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
			if (pixelData == -1 )
				m_HoveredEntity = Entity();
			else {
				//EG_CORE_WARN("pixelData: {0}", pixelData);
				m_HoveredEntity = Entity({ (entt::entity)pixelData, m_ActiveScene.get() });
			}

		}


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
		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s",name.c_str());
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertex Count: %d", stats.GetTotalVertexCount());
		ImGui::Text("Index Count: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("ViewPort");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };

		uint64_t textureID = m_FrameBuffer->GetColorAttachmentRendererID(0);
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x,m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });


		//EG_CORE_WARN("Min Bounds = {0}, {1}", m_ViewportBounds[0].x, m_ViewportBounds[0].y);
		//EG_CORE_WARN("Max Bounds = {0}, {1}", m_ViewportBounds[1].x, m_ViewportBounds[1].y);

		//Gizmo
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			//camera

			//auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			//const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			//const  glm::mat4& cameraProjection = camera.GetProjection();
			//glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			//entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform));
			if (ImGuizmo::IsUsing()) {

				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);
				tc.Translation = glm::vec3(transform[3]);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();


		ImGui::End();
	
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);
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
			else
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		//gizmo shortcuts

		case EG_KEY_R:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case EG_KEY_G:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case EG_KEY_Q:
			m_GizmoType = -1;
			break;
		
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