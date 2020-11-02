//Mesh class to laod our objects
/*
    Tyler Wong
    2020-10-24
*/

#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

// Trim from start (in place)
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
        }));
}

// Trim from end (in place)
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// Trim from both ends (in place)
static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

    Mesh::Mesh(const std::string& fileName)
    {
        std::ifstream file;
        file.open(fileName, std::ios::binary);

        if (!file) {
            throw std::runtime_error("Failed to open file");
        }

        std::vector< glm::fvec3 > temp_vertices;
        std::vector< glm::fvec2 > temp_uvs;
        std::vector< glm::fvec3 > temp_normals;

        std::string line, id;

        // Iterate as long as there is content to read
        while (std::getline(file, line))
        {
            trim(line);
            if (line.substr(0, 2) == "# ")
            {
                // Comment, no-op
            }
            else if (line.substr(0, 2) == "v ")
            {
                std::istringstream ss = std::istringstream(line.substr(2));
                glm::vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                temp_vertices.push_back(pos);
            }
            else if (line.substr(0, 3) == "vt ")
            {
                std::istringstream ss = std::istringstream(line.substr(3));
                glm::vec2 uv;
                ss >> uv.x >> uv.y;
                temp_uvs.push_back(uv);
            }
            else if (line.substr(0, 3) == "vn ")
            {
                std::istringstream ss = std::istringstream(line.substr(3));
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (line.substr(0, 2) == "f ")
            {
                std::istringstream ss = std::istringstream(line.substr(2));

                std::string v1, v2, v3;
                char tempIgnoreChar;

                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

                ss >> vertexIndex[0] >> tempIgnoreChar >> uvIndex[0] >> tempIgnoreChar >> normalIndex[0] >> vertexIndex[1] >> tempIgnoreChar >> uvIndex[1] >> tempIgnoreChar >> normalIndex[1] >> vertexIndex[2] >> tempIgnoreChar >> uvIndex[2] >> tempIgnoreChar >> normalIndex[2];

                vertIndices.push_back(vertexIndex[0]);
                vertIndices.push_back(vertexIndex[1]);
                vertIndices.push_back(vertexIndex[2]);
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[1]);
                uvIndices.push_back(uvIndex[2]);
                normIndices.push_back(normalIndex[0]);
                normIndices.push_back(normalIndex[1]);
                normIndices.push_back(normalIndex[2]);

            }
            else {}
        }

        for (unsigned int i = 0; i < vertIndices.size(); i++)
        {
            unsigned int vertexIndex = vertIndices[i];
            unsigned int uvIndex = uvIndices[i];
            unsigned int normIndex = normIndices[i];

            interleaved.push_back(temp_vertices[vertexIndex - 1].x);
            interleaved.push_back(temp_vertices[vertexIndex - 1].y);
            interleaved.push_back(temp_vertices[vertexIndex - 1].z);
            interleaved.push_back(1.0f);
            interleaved.push_back(1.0f);
            interleaved.push_back(1.0f);
            interleaved.push_back(temp_uvs[uvIndex - 1].x);
            interleaved.push_back(temp_uvs[uvIndex - 1].y);
            interleaved.push_back(temp_normals[normIndex - 1].x);
            interleaved.push_back(temp_normals[normIndex - 1].y);
            interleaved.push_back(temp_normals[normIndex - 1].z);
        }

        makeVAO();
    }


    VertexArrayObject::sptr Mesh::makeVAO()
    {
        vao = VertexArrayObject::Create();
        VertexBuffer::sptr vbo = VertexBuffer::Create();
        vbo->LoadData(interleaved.data(), interleaved.size());

        size_t stride = sizeof(float) * 11;

        vao->AddVertexBuffer(vbo, {
        BufferAttribute(0, 3, GL_FLOAT, false, stride, NULL),
        BufferAttribute(1, 3, GL_FLOAT, false, stride, sizeof(float) * 3),
        BufferAttribute(2, 2, GL_FLOAT, false, stride, sizeof(float) * 6),
        BufferAttribute(3, 3, GL_FLOAT, false, stride, sizeof(float) * 8)
            });

        return vao;
    }

    void Mesh::Render()
    {
        vao->Render();
    }
