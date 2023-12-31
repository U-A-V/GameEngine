#pragma once
#include "Engine.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/Renderer3DSettingsPanel.h"

#include "Engine/Renderer/EditorCamera.h"

namespace Engine {


	class EditorLayer:public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(TimeStamp ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		//UI Panels
		void UI_Toolbar();
	private:
		OrthographicCameraController m_CameraController;


		Ref<Shader> m_FlatColorShader;
		Ref<VertexArray> m_SquareVA;
		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Texture2D> m_Texture;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondaryCamera;
		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;

		bool m_PrimaryCamera = true;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };

		glm::vec2 m_ViewportSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

		enum class SceneState {
			Edit = 0, Play = 1,
		};

		SceneState m_SceneState = SceneState::Edit;
		enum class ProjectionType {
			Orthographic = 0, Perspective = 1,

		};

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
		//panel
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		Renderer3DSettingsPanel m_Renderer3DSettingsPanel;
		//Editor Resources
		Ref<Texture2D> m_IconPlay, m_IconStop;

	};
}