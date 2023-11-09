#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

namespace Engine {
	class Renderer3DSettingsPanel {
	public:
		Renderer3DSettingsPanel() = default;


		void OnImGuiRender();

	private:
		void DrawElements();
	};
}