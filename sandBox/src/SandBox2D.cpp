#include "SandBox2D.h"
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"


SandBox2D::SandBox2D()
	:Layer("SandBox2D"), m_CameraController(1280.0f/720.0f){
}

void SandBox2D::OnAttach()
{
	m_SquareVA = Engine::VertexArray::Create();
	float squareVertices[4 * 5] = {
		//Vertices					//UV
	-0.5f,	-0.5f,	0.0f,		
	0.5f,	-0.5f,	0.0f,		
	0.5f,	0.5f,	0.0f,		
	-0.5f,	0.5f,	0.0f,		
	};
	Engine::Ref<Engine::VertexBuffer> squareVB;
	squareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

	Engine::BufferLayout squareLayout = {
		{Engine::ShaderDataType::Float3, "a_Position"},
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


	m_FlatColorShader =Engine::Shader::Create("assets/shaders/FlatColor.glsl");

}

void SandBox2D::OnDetach()
{
}

void SandBox2D::OnUpdate(Engine::TimeStamp ts)
{
	m_CameraController.OnUpdate(ts);

	Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Engine::RenderCommand::Clear();

	Engine::Renderer::BeginScene(m_CameraController.GetCamera());


	std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4(m_SquareColor, "u_Color");


	Engine::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	//Engine::Renderer::Submit(m_Shader, m_VertexArray); 

	Engine::Renderer::EndScene();
}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void SandBox2D::OnEvent(Engine::Event& e)
{
}
