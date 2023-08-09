#include "hzpch.h"

#include "Engine/Log.h"
#include "WindowsWindow.h"

namespace Engine {
	static bool s_GLFWinitialized = false;

	Window* Window::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		Init(props);
	}

	WindowsWindow::~WindowsWindow(){}

	void WindowsWindow::Init(const WindowProps& props) {
		m_Data.Title = props.Title;
		m_Data.Width = props.Height;
		m_Data.Height = props.Height;

		EG_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWinitialized) {
			int success = glfwInit();
			EG_CORE_ASSERT(success, "Could not initialize GLFW!!");
			s_GLFWinitialized = true;
		}
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	void WindowsWindow::Shutdown() {
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
	
	void WindowsWindow::SetVSync(bool enabled) {
		if (enabled)	glfwSwapInterval(1);
		else			glfwSwapInterval(1);

		m_Data.VSync = enabled;
	}
	bool WindowsWindow::IsVSync() const {
		return m_Data.VSync;
	}
}