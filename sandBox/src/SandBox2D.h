#pragma once
#include "Engine.h"

#include "ParticleSystem.h"

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
	Engine::Ref<Engine::FrameBuffer> m_FrameBuffer;

	Engine::Ref<Engine::Texture2D> m_Texture, m_SpriteSheet;
	Engine::Ref<Engine::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;

	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, Engine::Ref<Engine::SubTexture2D>> s_TextureMap;
};