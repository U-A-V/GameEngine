#include <Engine.h>

#include <glm/gtc/matrix_transform.hpp>
class ExampleLayer : public Engine::Layer {
public:
	ExampleLayer()
		:Layer("Example"),m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f){

		m_VertexArray.reset(Engine::VertexArray::Create());

		float vertices[3 * 7] = {
			//Position						//Color
		-0.5f,	-0.5f,	0.0f,		0.8f,	0.2f,	0.8f,	1.0f,
		0.5f,	-0.5f,	0.0f,		0.2f,	0.3f,	0.8f,	1.0f,
		0.0f,	0.5f,	0.0f,		0.8f,	0.8f,	0.2f,	1.0f,
		};
		std::shared_ptr<Engine::VertexBuffer> m_VertexBuffer;
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
		std::shared_ptr<Engine::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA.reset(Engine::VertexArray::Create());
		float squareVertices[4 * 3] = {
			-0.5f,	-0.5f,	0.0f,
			0.5f,	-0.5f,	0.0f,
			0.5f,	0.5f,	0.0f,
			-0.5f,	0.5f,	0.0f,
		};
		std::shared_ptr<Engine::VertexBuffer> squareVB;
		squareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		Engine::BufferLayout squareLayout = {
			{Engine::ShaderDataType::Float3, "a_Position"}
		};

		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);


		unsigned int squareIndices[3 * 2] = {
			0,	1,	2,
			0,	2,	3,
		};

		std::shared_ptr<Engine::IndexBuffer> squareIB;
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

		m_Shader.reset(new Engine::Shader(vertexSrc, fragmentSrc));

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
			void main(){
				color=vec4(v_Position*0.5+0.5,1.0);
			}
		)";

		m_SquareShader.reset(new Engine::Shader(vertexSrcSq, fragmentSrcSq));

	}
	void OnUpdate(Engine::TimeStamp ts) override {


		if (Engine::Input::IsKeyPressed(EG_KEY_LEFT))
			m_CameraPosition.x -= m_CameraSpeed*ts;
		else if (Engine::Input::IsKeyPressed(EG_KEY_RIGHT))
			m_CameraPosition.x += m_CameraSpeed*ts;
		if (Engine::Input::IsKeyPressed(EG_KEY_UP))
			m_CameraPosition.y += m_CameraSpeed*ts;
		else if (Engine::Input::IsKeyPressed(EG_KEY_DOWN))
			m_CameraPosition.y -= m_CameraSpeed*ts;

		if (Engine::Input::IsKeyPressed(EG_KEY_Q))
			m_CameraRotation += m_CameraRotationSpeed*ts;
		else if (Engine::Input::IsKeyPressed(EG_KEY_E))
			m_CameraRotation -= m_CameraRotationSpeed*ts;


		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::RenderCommand::Clear();

		m_Camera.SetPosition({ m_CameraPosition });
		m_Camera.SetRotation(m_CameraRotation);

		Engine::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		for (int j = 0; j < 20; j++) {

			for (int i = 0; i < 20; i++) {
				glm::vec3 pos(i * 0.11f,j* 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f),pos)*scale;
				Engine::Renderer::Submit(m_SquareShader, m_SquareVA, transform);
			}
		}

		//Engine::Renderer::Submit(m_Shader, m_VertexArray);

		Engine::Renderer::EndScene();
	}
	void OnEvent(Engine::Event& event) override {

	}
 
private:

	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::VertexArray> m_VertexArray;

	std::shared_ptr<Engine::Shader> m_SquareShader;
	std::shared_ptr<Engine::VertexArray> m_SquareVA;

	Engine::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraSpeed = 5.0f; 

	float m_CameraRotation=0.0f;
	float m_CameraRotationSpeed=45.0f;


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