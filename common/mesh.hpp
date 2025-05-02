#pragma once

#include <vector>
#include <string>

#include <common/shader.hpp>
#include <common/maths.hpp>

using namespace std;

struct vec2
{
    float x, y;
};

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;
};

struct Texture
{
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;
    
    // Constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices,
         vector <Texture> textures);
};
