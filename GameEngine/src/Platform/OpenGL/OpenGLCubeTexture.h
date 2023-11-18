#pragma once

#include "Engine/Renderer/Texture.h"

namespace Engine {
	class OpenGLCubeTexture:public CubeTexture
	{
	public:
		OpenGLCubeTexture(const std::vector<std::string>& faces);
		OpenGLCubeTexture(uint32_t size);
		~OpenGLCubeTexture();


		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual uint32_t GetSize() const override { return m_Size; }

		virtual void Bind(uint32_t slot = 0) const override;

		virtual void SetData(const std::string& filePath, uint8_t index) override;
		virtual std::vector<std::string> GetFilePath() override;
		virtual bool operator==(const Texture& other) const override {
			return m_RendererID == ((OpenGLCubeTexture&)other).m_RendererID;
		}
	private:
		uint32_t m_RendererID;
		std::vector<std::string> m_FilePath = {"","","","","",""};
		uint32_t m_Size;
	};
}