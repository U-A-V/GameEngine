#pragma once
#include "Engine/Layer.h"

namespace Engine {

	class ENGINE_API imGuiLayer : public Layer{
	public:
		imGuiLayer();
		~imGuiLayer();

		void OnAttach();
		void OnDettach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		float m_Time=0.0f;
	};
}


