#pragma once
#include "Engine.h"

class SandBox2D:public Engine::Layer {
public:
	SandBox2D();
	virtual ~SandBox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(Engine::TimeStamp ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Engine::Event& e) override;
private:
	Engine::OrthographicCameraController m_CameraController;


	Engine::Ref<Engine::Shader> m_FlatColorShader;
	Engine::Ref<Engine::VertexArray> m_SquareVA;
	Engine::Ref<Engine::Texture2D> m_Texture;

	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };


};