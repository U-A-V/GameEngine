#pragma once

#include "Engine/Renderer/FrameBuffer.h"

namespace Engine {
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification specs);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index= 0) const override {EG_CORE_ASSERT(index< m_ColorAttachments.size(),"") return m_ColorAttachments[index]; }

		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		FrameBufferSpecification m_Specification;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecs;
		FrameBufferTextureSpecification m_DepthAttachmentSpecs = FrameBufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}