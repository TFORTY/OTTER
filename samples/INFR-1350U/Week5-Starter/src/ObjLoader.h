#pragma once
#include <string>
#include <vector>
#include <GLM/glm.hpp>
#include "VertexArrayObject.h"

class ObjLoader
{
public:
    ObjLoader(const std::string& f, const glm::vec4& inColor = glm::vec4(1.f));
    VertexArrayObject::sptr makeVAO();

    /////////////////////////////////////////////////////////
    VertexArrayObject::sptr vao = VertexArrayObject::Create();
    void Render();

protected:
    std::vector<float> Interleaved; 
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices; //Stores the indices
    static const std::vector<BufferAttribute> buffAttibs; //Stores the buffers
};