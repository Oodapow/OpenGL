#pragma once
#include <random>
# include <cstdint>
# include <numeric>
# include <algorithm>

#include <Core/GPU/Mesh.h>
#include <glm/glm.hpp>

class PerlinNoise
{
private:

	std::int32_t p[512];

	static double Fade(double t) noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	};

	static double Lerp(double t, double a, double b) noexcept
	{
		return a + t * (b - a);
	};

	static double Grad(std::int32_t hash, double x, double y, double z) noexcept
	{
		const std::int32_t h = hash & 15;
		const double u = h < 8 ? x : y;
		const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	};

public:

	explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed)
	{
		reseed(seed);
	};

	void reseed(std::uint32_t seed)
	{
		for (size_t i = 0; i < 256; ++i)
		{
			p[i] = i;
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

		for (size_t i = 0; i < 256; ++i)
		{
			p[256 + i] = p[i];
		}
	};

	double noise(double x) const
	{
		return noise(x, 0.0, 0.0);
	};

	double noise(double x, double y) const
	{
		return noise(x, y, 0.0);
	};

	double noise(double x, double y, double z) const
	{
		const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
		const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
		const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		const double u = Fade(x);
		const double v = Fade(y);
		const double w = Fade(z);

		const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
			Grad(p[BA], x - 1, y, z)),
			Lerp(u, Grad(p[AB], x, y - 1, z),
				Grad(p[BB], x - 1, y - 1, z))),
			Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
				Grad(p[BA + 1], x - 1, y, z - 1)),
				Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
					Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	};

	double octaveNoise(double x, std::int32_t octaves) const
	{
		double result = 0.0;
		double amp = 1.0;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += noise(x) * amp;
			x *= 2.0;
			amp *= 0.5;
		}

		return result;
	};

	double octaveNoise(double x, double y, std::int32_t octaves) const
	{
		double result = 0.0;
		double amp = 1.0;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += noise(x, y) * amp;
			x *= 2.0;
			y *= 2.0;
			amp *= 0.5;
		}

		return result;
	};

	double octaveNoise(double x, double y, double z, std::int32_t octaves) const
	{
		double result = 0.0;
		double amp = 1.0;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += noise(x, y, z) * amp;
			x *= 2.0;
			y *= 2.0;
			z *= 2.0;
			amp *= 0.5;
		}

		return result;
	};

	double noise0_1(double x) const
	{
		return noise(x) * 0.5 + 0.5;
	};

	double noise0_1(double x, double y) const
	{
		return noise(x, y) * 0.5 + 0.5;
	};

	double noise0_1(double x, double y, double z) const
	{
		return noise(x, y, z) * 0.5 + 0.5;
	};

	double octaveNoise0_1(double x, std::int32_t octaves) const
	{
		return octaveNoise(x, octaves) * 0.5 + 0.5;
	};

	double octaveNoise0_1(double x, double y, std::int32_t octaves) const
	{
		return octaveNoise(x, y, octaves) * 0.5 + 0.5;
	};

	double octaveNoise0_1(double x, double y, double z, std::int32_t octaves) const
	{
		return octaveNoise(x, y, z, octaves) * 0.5 + 0.5;
	};
};

class Builder 
{
public:
	Builder(glm::vec2 resolution, glm::vec2 position, double lineLength, long unsigned int seed) : resolution(resolution), position(position), lineLength(lineLength){
		noise = new PerlinNoise(seed);
	};

	~Builder() {
		delete noise;
	}

	Mesh* GetMesh(std::string name) {
		int width = resolution.x;
		int length = resolution.y;
		
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned short> indices;

		unsigned short index = 0;

		for (int i = 0; i < length; i++) {
			for (int j = 0; j < width; j++) {
				float jj = (j - width / 2);
				float ii = (i - length / 2);
				float jj1 = jj + 1;
				float ii1 = ii + 1;

				ii *= lineLength;
				ii1 *= lineLength;
				jj *= lineLength;
				jj1 *= lineLength;

				glm::vec3 p1 = glm::vec3(jj, getHValue((ii + position.y) / length, (jj + position.x) / width), ii);
				glm::vec3 p2 = glm::vec3(jj1, getHValue((ii + position.y) / length, (jj1 + position.x) / width), ii);
				glm::vec3 p3 = glm::vec3(jj, getHValue((ii1 + position.y) / length, (jj + position.x) / width), ii1);
				glm::vec3 p4 = glm::vec3(jj1, getHValue((ii1 + position.y) / length, (jj1 + position.x) / width), ii1);

				positions.push_back(p1);
				positions.push_back(p2);
				positions.push_back(p3);
				positions.push_back(p2);
				positions.push_back(p4);
				positions.push_back(p3);

				glm::vec3 n1 = glm::normalize(glm::cross(p3 - p1, p2 - p1));
				glm::vec3 n2 = glm::normalize(glm::cross(p2 - p4, p3 - p4));

				normals.push_back(n1);
				normals.push_back(n1);
				normals.push_back(n1);
				normals.push_back(n2);
				normals.push_back(n2);
				normals.push_back(n2);

				float w = (width - 1)*lineLength;
				float l = (length - 1)*lineLength;

				glm::vec2 t1 = glm::vec2(j*lineLength / w, i*lineLength / l);
				glm::vec2 t2 = glm::vec2((j + 1)*lineLength / w, i*lineLength / l);
				glm::vec2 t3 = glm::vec2(j*lineLength / w, (i + 1)*lineLength / l);
				glm::vec2 t4 = glm::vec2((j + 1)*lineLength / w, (i + 1)*lineLength / l);

				texCoords.push_back(t1);
				texCoords.push_back(t2);
				texCoords.push_back(t3);
				texCoords.push_back(t2);
				texCoords.push_back(t4);
				texCoords.push_back(t3);

				indices.push_back(index++);
				indices.push_back(index++);
				indices.push_back(index++);
				indices.push_back(index++);
				indices.push_back(index++);
				indices.push_back(index++);
			}
		}

		auto mesh = new Mesh(name);
		mesh->InitFromData(positions, normals, texCoords, indices);
		positions.clear();
		normals.clear();
		texCoords.clear();
		indices.clear();
		return mesh;
	}

	glm::vec2 position;

private:

	float getHValue(float y, float x) {
		return noise->octaveNoise(x, y, 8) * 20;
	}

	double lineLength;
	glm::vec2 resolution;
	PerlinNoise* noise;
};