#include "hzpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine {
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexBuffer>(size);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size) {
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLIndexBuffer>(indices, size);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}