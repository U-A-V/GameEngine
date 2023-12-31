#include "hzpch.h"
#include "OrthographicCameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine {
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }),m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top), m_Rotation(rotation){


	}

	void OrthographicCameraController::OnUpdate(TimeStamp ts){
		EG_PROFILE_FUNCTION();


		if (Input::IsKeyPressed(EG_KEY_A))
			m_CameraPosition.x -= m_CameraSpeed * ts;
		else if (Input::IsKeyPressed(EG_KEY_D))
			m_CameraPosition.x += m_CameraSpeed * ts;
		if (Input::IsKeyPressed(EG_KEY_W))
			m_CameraPosition.y += m_CameraSpeed * ts;
		else if (Input::IsKeyPressed(EG_KEY_S))
			m_CameraPosition.y -= m_CameraSpeed * ts;

		if (m_Rotation) {
			if (Input::IsKeyPressed(EG_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			else if (Input::IsKeyPressed(EG_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}
		m_Camera.SetPosition(m_CameraPosition);
	}

	void OrthographicCameraController::OnEvent(Event& e){
		EG_PROFILE_FUNCTION();


		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(EG_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(EG_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));


	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		CalculateView();
	}

	void OrthographicCameraController::CalculateView() {
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e) {
		EG_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		CalculateView();
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e){
		EG_PROFILE_FUNCTION();

		m_AspectRatio = e.GetWidth() / (float)e.GetHeight();
		CalculateView();
		return false;
	}
}