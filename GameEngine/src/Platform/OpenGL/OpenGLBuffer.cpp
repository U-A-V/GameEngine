#include "hzpch.h"
#include "OpenGLBuffer.h"

#include "glad/glad.h"

namespace Engine {
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size){
		EG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER,size, vertices, GL_STATIC_DRAW);
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		EG_PROFILE_FUNCTION();

		glDeleteBuffers(0, &m_RendererID);
	}
	void OpenGLVertexBuffer::Bind() const{
		EG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLVertexBuffer::Unbind() const{
		EG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	/// <summary>
	/// INDEXBUFFER IMPLEMENTATION
	/// </summary>
	/// 
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		:m_Count(count){
		EG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,count*sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		EG_PROFILE_FUNCTION();

		glDeleteBuffers(0, &m_RendererID);
	}
	void OpenGLIndexBuffer::Bind() const{
		EG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLIndexBuffer::Unbind() const{
		EG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
}