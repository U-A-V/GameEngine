#include "hzpch.h"
#include "Application.h"
#include "Input.h"


#include <glad/glad.h>


namespace Engine {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		EG_CORE_ASSERT(s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new imGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
				//Position						//Color
			-0.5f,	-0.5f,	0.0f,		0.8f,	0.2f,	0.8f,	1.0f,
			0.5f,	-0.5f,	0.0f,		0.2f,	0.3f,	0.8f,	1.0f,
			0.0f,	0.5f,	0.0f,		0.8f,	0.8f,	0.2f,	1.0f,
		};
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(vertices,sizeof(vertices)));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position" },
			{ShaderDataType::Float4, "a_Color"},

		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		unsigned int indices[3] = {
			0,	1,	2
		};
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA.reset(VertexArray::Create());
		float squareVertices[4 * 3] = {
			-0.5f,	-0.5f,	0.0f,
			0.5f,	-0.5f,	0.0f,
			0.5f,	0.5f,	0.0f,
			-0.5f,	0.5f,	0.0f,
		};
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		BufferLayout squareLayout = {
			{ShaderDataType::Float3, "a_Position"}
		};

		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);


		unsigned int squareIndices[3 * 2] = {
			0,	1,	2,
			0,	2,	3,
		};

		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSrc =R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec4 a_Color;
			out vec3 v_Position;
			out vec4 v_Color;
			void main(){
				v_Position=a_Position;
				v_Color=a_Color;
				gl_Position = vec4(a_Position,1.0);
			}
		)";
		std::string fragmentSrc =R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main(){
				color=v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc,fragmentSrc));

		std::string vertexSrcSq =R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			out vec3 v_Position;
			void main(){
				v_Position=a_Position;
				gl_Position = vec4(a_Position,1.0);
			}
		)";
		std::string fragmentSrcSq =R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			void main(){
				color=vec4(v_Position*0.5+0.5,1.0);
			}
		)";

		m_SquareShader.reset(new Shader(vertexSrcSq,fragmentSrcSq));

	}
	Application::~Application() {

	}
	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();

	}
	void Application::PushOverlay(Layer* overlay) {
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();

	}
	void Application::OnEvent(Event& e) {

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled)	break;
		}
	}

	void Application::Run() {

		while (m_Running) {
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			//glUseProgram(0);
			m_SquareShader->Bind();
			m_SquareVA->Bind();
			glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffers()->GetCount(), GL_UNSIGNED_INT, nullptr);

			m_Shader->Bind();
			m_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffers()->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)	layer->OnUpdate();

			auto [x, y] = Input::GetMousePosition();
			//EG_CORE_TRACE("{0}, {1}", x, y);
			
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)	layer->OnImGuiRender();
			m_ImGuiLayer->End();
			
			
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
}