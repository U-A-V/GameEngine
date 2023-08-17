#include "hzpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return new OpenGLVertexBuffer(vertices, size);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size) {
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return new OpenGLIndexBuffer(indices, size);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}