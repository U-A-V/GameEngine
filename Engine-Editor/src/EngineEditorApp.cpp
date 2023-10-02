#include <Engine.h>
#include "Engine/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Engine {

	class EngineEditor : public Application {
	public:
		EngineEditor(ApplicationCommandLineArgs args) 
			:Application("Engine Editor", args) {

			PushLayer(new EditorLayer());
		}
		~EngineEditor() {

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args) {
		return new EngineEditor(args);
	}
}