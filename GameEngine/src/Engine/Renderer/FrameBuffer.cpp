#include "hzpch.h"
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Engine {
	 
	Ref<FrameBuffer>  FrameBuffer::Create(const FrameBufferSpecification specs) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:	EG_CORE_ASSERT(false, "RendererAPI:None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLFrameBuffer>(specs);
		}

		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}