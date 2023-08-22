#pragma once
#include "Engine/Renderer/OrthographicCamera.h"
#include "Engine/Core/TimeStamp.h"

#include "Engine/Events/ApplicatonEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine {
	class OrthographicCameraController {
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false );

		void OnUpdate(TimeStamp ts);
		void OnEvent(Event& e);

		const OrthographicCamera& GetCamera() const { return m_Camera; }
		OrthographicCamera& GetCamera() { return m_Camera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_Zoomlevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
		float m_CameraRotation = 0.0f;

		float m_CameraSpeed = 5.0f, m_CameraRotationSpeed = 45.0f;
	};
}