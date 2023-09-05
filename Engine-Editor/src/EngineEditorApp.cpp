#include <Engine.h>
#include "Engine/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Engine {

	class EngineEditor : public Application {
	public:
		EngineEditor() 
			:Application("Engine Editor") {

			PushLayer(new EditorLayer());
		}
		~EngineEditor() {

		}
	};

	Application* CreateApplication() {
		return new EngineEditor();
	}
}