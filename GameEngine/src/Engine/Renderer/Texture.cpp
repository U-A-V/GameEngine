#include "hzpch.h"

#include "Texture.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLCubeTexture.h"

namespace Engine{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(width,height);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path, int size) {
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(path, size);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Ref<CubeTexture> CubeTexture::Create(const std::vector<std::string>& faces) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLCubeTexture>(faces);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Ref<CubeTexture> CubeTexture::Create(uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:		EG_CORE_ASSERT(flase, "RendererAPI::None is currently not supported!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLCubeTexture>(size);
		}
		EG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}