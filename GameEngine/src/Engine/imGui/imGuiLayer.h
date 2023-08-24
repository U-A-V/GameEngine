#pragma once
#include "Engine/Core/Layer.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/ApplicatonEvent.h"

namespace Engine {

	class ENGINE_API imGuiLayer : public Layer{
	public:
		imGuiLayer();
		~imGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();
	private:
		float m_Time=0.0f;
	};
}


