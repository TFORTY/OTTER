#pragma once
#include <string>
#include <vector>
#include <GLM/glm.hpp>
#include "VertexArrayObject.h"

class ObjLoader
{
public:
    static bool LoadFromFile(const std::string& filename, std::vector<glm::vec3>& outV, std::vector<glm::vec2>& outUV, std::vector<glm::vec3>& outN);
     
    ObjLoader(const std::string& f);
    //VertexArrayObject::sptr loadObj();
    VertexArrayObject::sptr makeVAO();
    VertexArrayObject::sptr vao = VertexArrayObject::Create();
    void Render();

protected:
    //std::string fileName; //Stores the filename
    std::vector <glm::vec3> out_Vertices; //Stores the verticies
    std::vector<glm::vec2> out_Uvs; //Stores the UVs
    std::vector<glm::vec3> out_Normals; //Stores the normals
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices; //Stores the indices
    static const std::vector<BufferAttribute> buffAttibs; //Stores the buffers
};