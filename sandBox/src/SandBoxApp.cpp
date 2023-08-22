#include <Engine.h>
#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Renderer/Shader.h"


class ExampleLayer : public Engine::Layer {
public:
	ExampleLayer()
		:Layer("Example"),m_CameraController(16.0/9.0f ){

		m_VertexArray.reset(Engine::VertexArray::Create());

		float vertices[3 * 7] = {
			//Position						//Color
		-0.5f,	-0.5f,	0.0f,		0.8f,	0.2f,	0.8f,	1.0f,
		0.5f,	-0.5f,	0.0f,		0.2f,	0.3f,	0.8f,	1.0f,
		0.0f,	0.5f,	0.0f,		0.8f,	0.8f,	0.2f,	1.0f,
		};
		Engine::Ref<Engine::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));

		Engine::BufferLayout layout = {
			{Engine::ShaderDataType::Float3, "a_Position" },
			{Engine::ShaderDataType::Float4, "a_Color"},

		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		unsigned int indices[3] = {
			0,	1,	2
		};
		Engine::Ref<Engine::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA.reset(Engine::VertexArray::Create());
		float squareVertices[4 * 5] = {
				//Vertices					//UV
			-0.5f,	-0.5f,	0.0f,		0.0f,	0.0f,
			0.5f,	-0.5f,	0.0f,		1.0f,	0.0f,
			0.5f,	0.5f,	0.0f,		1.0f,	1.0f,
			-0.5f,	0.5f,	0.0f,		0.0f,	1.0f
		};
		Engine::Ref<Engine::VertexBuffer> squareVB;
		squareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		Engine::BufferLayout squareLayout = {
			{Engine::ShaderDataType::Float3, "a_Position"},
			{Engine::ShaderDataType::Float2, "a_TexCoord"}
		};

		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);


		unsigned int squareIndices[3 * 2] = {
			0,	1,	2,
			0,	2,	3,
		};

		Engine::Ref<Engine::IndexBuffer> squareIB;
		squareIB.reset(Engine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			void main(){
				v_Position=a_Position;
				v_Color=a_Color;
				gl_Position = u_ViewProjection * u_Transform* vec4(a_Position,1.0);
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main(){
				color=v_Color;
			}
		)";

		m_Shader = Engine::Shader::Create("BasicShader",vertexSrc, fragmentSrc);

		std::string vertexSrcSq = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			void main(){
				v_Position=a_Position;
				gl_Position = u_ViewProjection *u_Transform* vec4(a_Position,1.0);
			}
		)";
		std::string fragmentSrcSq = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;

			uniform vec4 u_Color;
 
			void main(){
				color=u_Color;
			}
		)";

		m_FlatColorShader = Engine::Shader::Create("SquareShader", vertexSrcSq, fragmentSrcSq);



		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
		m_YoutubeLogo = Engine::Texture2D::Create("assets/textures/youtube-logo.png");

		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt(0, "u_Texture");
	}
	void OnUpdate(Engine::TimeStamp ts) override {
		//update
		m_CameraController.OnUpdate(ts);

		//render
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_CameraController.GetCamera());

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4(m_squareColor,"u_Color");
		for (int j = 0; j < 20; j++) {

			for (int i = 0; i < 20; i++) {
				glm::vec3 pos(i * 0.11f,j* 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f),pos)*scale;
				Engine::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind(0);
		Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_YoutubeLogo->Bind(0);
		Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Engine::Renderer::Submit(m_Shader, m_VertexArray); 

		Engine::Renderer::EndScene();
	}
	virtual void OnImGuiRender() override {
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor));
		ImGui::End();
	}
	void OnEvent(Engine::Event& e) override {
		m_CameraController.OnEvent(e);
	}
 
private:
	Engine::ShaderLibrary m_ShaderLibrary;

	Engine::Ref<Engine::Shader> m_Shader;
	Engine::Ref<Engine::VertexArray> m_VertexArray;

	Engine::Ref<Engine::Shader> m_FlatColorShader;
	Engine::Ref<Engine::VertexArray> m_SquareVA;


	Engine::Ref<Engine::Texture2D> m_Texture, m_YoutubeLogo;

	Engine::OrthographicCameraController m_CameraController;

	glm::vec4 m_squareColor={ 0.2f,0.3f,0.8f,1.0f };
};

class Sandbox : public Engine::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}
	~Sandbox() {

	}
};

Engine::Application* Engine::CreateApplication() {
	return new Sandbox();
}