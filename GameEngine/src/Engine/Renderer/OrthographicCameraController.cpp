#include "hzpch.h"
#include "OrthographicCameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine {
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio),m_Camera(-m_AspectRatio*m_Zoomlevel, m_AspectRatio*m_Zoomlevel, -m_Zoomlevel, m_Zoomlevel), m_Rotation(rotation){


	}




	void OrthographicCameraController::OnUpdate(TimeStamp ts){

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

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(EG_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(EG_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));


	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e){
		m_Zoomlevel -= e.GetYOffset()*0.25;
		m_Zoomlevel = std::max(m_Zoomlevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_Zoomlevel, m_AspectRatio * m_Zoomlevel, -m_Zoomlevel, m_Zoomlevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e){
		m_AspectRatio = e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_Zoomlevel, m_AspectRatio * m_Zoomlevel, -m_Zoomlevel, m_Zoomlevel);
		return false;
	}
}