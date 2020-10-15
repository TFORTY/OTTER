#include "ObjLoader.h"
#include <string>
#include <sstream>
#include <fstream>
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

// Load the .obj file
bool ObjLoader::LoadFromFile(const std::string& filename, std::vector<glm::vec3>& outV, std::vector<glm::vec2>& outUV, std::vector<glm::vec3>& outN)
{
    std::vector<GLuint> indicesV, indicesN, indicesUV;
    std::vector<glm::vec3> verticies, normals;
    std::vector<glm::vec2> uvs;

    // Open our file in binary mode
    std::ifstream file;
    file.open(filename, std::ios::binary);

    // If our file fails to open, we will throw an error
    if (!file) {
        throw std::runtime_error("File failed to open");
    }

    std::string line;

    //To load from the file
    while (std::getline(file, line)) {
        trim(line);
        if (line.substr(0, 1) == "#")
        {
            //Do nothing becasue # is a comment
        }
        //For Vertices
        else if (line.substr(0, 2) == "v ")
        {

            //Reads from the line and gets each value of the vertex
            std::istringstream ss = std::istringstream(line.substr(2));
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            //Adds vertex to the vector
            verticies.push_back(pos);
        }
        //For UVs
        else if (line.substr(0, 2) == "vt")
        {
            //Reads from the line and gets each value of the UV
            std::istringstream ss = std::istringstream(line.substr(2));
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            //Adds uv to the vector
            uvs.push_back(uv);
        }
        //For Normals
        else if (line.substr(0, 2) == "vn")
        {
            //Reads from the line and gets each value of the Normal
            std::istringstream ss = std::istringstream(line.substr(2));
            glm::vec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            //Adds normal to the vector
            normals.push_back(norm);
        }
        //For Faces
        else if (line.substr(0, 2) == "f ")
        {
            std::istringstream ss = std::istringstream(line.substr(2));
            char s1;
            int v, uv, n;

            for (int i = 0; i < 3; i++)
            {
                ss >> v >> s1 >> uv >> s1 >> n;
                indicesV.push_back(v);
                indicesUV.push_back(uv);
                indicesN.push_back(n);
            }
        }

    }

    //Gets the attributes and arranges the index of the faces
    for (unsigned int i = 0; i < indicesV.size(); i++) {

        unsigned int vertexIndex = indicesV[i];
        unsigned int uvIndex = indicesUV[i];
        unsigned int normalIndex = indicesN[i];
        glm::vec3 vertex = verticies[vertexIndex - 1];
        glm::vec2 uv = uvs[uvIndex - 1];
        glm::vec3 normal = normals[normalIndex - 1];
        outV.push_back(vertex);
        outUV.push_back(uv);
        outN.push_back(normal);
    }

    return true;
}