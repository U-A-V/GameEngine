#pragma once


namespace Engine {
	class Shader;
	class Texture{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetRendererID() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	private:

	};
	class CubeTexture :public Texture {
	public:
		static Ref<CubeTexture> Create(const std::vector<std::string>& faces);
		static Ref<CubeTexture> Create(uint32_t size);
		virtual void SetData(const std::string& filePath, uint8_t index) = 0;
		virtual bool CanRender() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual std::vector<std::string> GetFilePath() = 0;
	};

	class Texture2D :public Texture{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path, int size =8);

		virtual void ComputeCubeMap(Ref<CubeTexture>& target, Ref<Shader>& shader) = 0;
		virtual uint32_t GetInternalFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;

	};


}