#pragma once
#include "Engine/Renderer/GraphicContext.h"

struct GLFWwindow;

namespace Engine {
	class OpenGLContext: public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}
