#pragma once
#include <random>

#include <Core/GPU/Mesh.h>
#include <glm/glm.hpp>

class Map {
public:
	Map(unsigned int dx, unsigned int dz) : dz(dz), dx(dx) {
		map = new float*[dz];
		for (int i = 0; i < dz; i++) {
			map[i] = new float[dx];
			for (int j = 0; j < dx; j++) {
				map[i][j] = 0;
			}
		}
	};

	~Map() {
		for (int i = 0; i < dz; i++) {
			delete map[i];
		}
		delete map;
	};

	bool GetValue(unsigned int z, unsigned int x, float* value) {
		if (x < 0 || z < 0 || x >= dx || z >= dz) {
			return false;
		}
		(*value) = map[z][x];
		return true;
	}

	float** map;
	const unsigned int dx;
	const unsigned int dz;
};

class Builder 
{
public:
	Builder(unsigned int dx, unsigned int dz, long unsigned int seed) : _eng{ seed }, _seed(seed) {
		_map = new Map(dx, dz);
	};

	~Builder() {
		delete _map;
	}

	unsigned int Getdx() { return _map->dx; };
	unsigned int Getdz() { return _map->dz; };

	void AddMiddlePoint(float offset) {
		auto oldMap = _map;
	
		_map = new Map(2 * oldMap->dx - 1, 2 * oldMap->dz - 1);

		for (int i = 0; i < _map->dz; i++) {
			for (int j = 0; j < _map->dx; j++) {
				if (i % 2 == 0 && j % 2 == 0) {
					_map->map[i][j] = std::normal_distribution<float>(oldMap->map[i / 2][j / 2], offset)(_eng);
				}
				else
				if (i % 2 == 1 && j % 2 == 1) {
					float sum = 0;
					unsigned int total = 0;

					float current;

					if (oldMap->GetValue(i / 2, j / 2, &current)) {
						sum += current;
						total++;
					}

					if (oldMap->GetValue(i / 2 + 1, j / 2, &current)) {
						sum += current;
						total++;
					}

					if (oldMap->GetValue(i / 2, j / 2 + 1, &current)) {
						sum += current;
						total++;
					}

					if (oldMap->GetValue(i / 2 + 1, j / 2 + 1, &current)) {
						sum += current;
						total++;
					}

					_map->map[i][j] = std::normal_distribution<float>(sum / total, offset)(_eng);
				}
				else if (i % 2 == 1) {
					float sum = 0;
					unsigned int total = 0;

					float current;

					if (oldMap->GetValue(i / 2, j / 2, &current)) {
						sum += current;
						total++;
					}

					if (oldMap->GetValue(i / 2 + 1, j / 2, &current)) {
						sum += current;
						total++;
					}

					_map->map[i][j] = std::normal_distribution<float>(sum / total, offset)(_eng);
				}
				else {
					float sum = 0;
					unsigned int total = 0;

					float current;

					if (oldMap->GetValue(i / 2, j / 2, &current)) {
						sum += current;
						total++;
					}

					if (oldMap->GetValue(i / 2, j / 2 + 1, &current)) {
						sum += current;
						total++;
					}

					_map->map[i][j] = std::normal_distribution<float>(sum / total, offset)(_eng);
				}
			}
		}

		delete oldMap;
	}

	void Blur() {
		auto oldMap = _map;
		_map = new Map(oldMap->dx, oldMap->dz);

		float gaussianMat[3][3] = {
			{ 16, 8, 16},
			{ 8, 4, 8},
			{ 16,8, 16} };

		for (int i = 0; i < _map->dz; i++)
		{
			for (int j = 0; j < _map->dx; j++)
			{
				float sum = 0;
				for (int ii = -1; ii < 2; ii++)
				{
					for (int jj = -1; jj < 2; jj++)
					{
						int diffii = i + ii;
						int diffjj = j + jj;

						if (diffii < 0 || diffjj < 0 || diffii == _map->dz || diffjj == _map->dx)
						{
							continue;
						}
						sum += oldMap->map[diffii][diffjj] / gaussianMat[ii + 1][jj + 1];
					}
				}
				_map->map[i][j] = sum;
			}
		}

		delete oldMap;
	}

	void AddNoise(float offset) {
		for (int i = 0; i < _map->dz; i++)
		{
			for (int j = 0; j < _map->dx; j++)
			{
				_map->map[i][j] = std::normal_distribution<float>(_map->map[i][j], offset)(_eng);
			}
		}
	}

	void AddOffset(float offset) {
		for (int i = 0; i < _map->dz; i++)
		{
			for (int j = 0; j < _map->dx; j++)
			{
				_map->map[i][j] += offset;
			}
		}
	}


	Mesh* GetMesh() {
		int width = _map->dz;
		int length = _map->dx;
		float** heightMap = _map->map;

		double lineLength = 1;
		std::string name = "terrain";

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned short> indices;

		unsigned short index = 0;

		for (int i = 0; i < length - 1; i++) {
			for (int j = 0; j < width - 1; j++) {
				int jj = (j - width / 2);
				int ii = (i - length / 2);
				glm::vec3 p1 = glm::vec3(jj * lineLength, heightMap[i][j], ii*lineLength);
				glm::vec3 p2 = glm::vec3((jj + 1)*lineLength, heightMap[i][j + 1], ii*lineLength);
				glm::vec3 p3 = glm::vec3(jj*lineLength, heightMap[i + 1][j], (ii + 1)*lineLength);
				glm::vec3 p4 = glm::vec3((jj + 1)*lineLength, heightMap[i + 1][j + 1], (ii + 1)*lineLength);

				positions.push_back(p1);
				positions.push_back(p2);
				positions.push_back(p3);
				positions.push_back(p2);
				positions.push_back(p4);
				positions.push_back(p3);

				glm::vec3 n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
				glm::vec3 n2 = glm::normalize(glm::cross(p3 - p4, p2 - p4));

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
private:
	int _seed;
	std::default_random_engine _eng;
	Map* _map;
};