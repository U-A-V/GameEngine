#pragma once


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace Engine {
	class Sphere {
	public:
		Sphere() = default;
		Sphere(const float radius, const uint32_t sectorCount, const uint32_t stackCount,
		uint32_t& indexCount,
		std::vector<glm::vec4>& VertexPositions,
		std::vector<glm::vec3>& Normals,
		std::vector<glm::vec2>& TexCoords,
		std::vector<uint32_t>& indices);

		~Sphere() = default;

	private:

	};
}