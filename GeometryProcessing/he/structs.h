#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>

namespace he
{

    struct Vec3f
    {
        float x, y, z;
    };

    struct Vertex
    {
        float x, y, z;
    };

    struct Face
    {
        int idx1, idx2, idx3;
    };

    struct Mesh_Data
    {
        std::vector <glm::vec3*> * vertices;
        std::vector<glm::vec3*>* faces;
    };

    //he::Mesh_Data MakeMeshData(Mesh* meshSource)
    //{
    //    he::Mesh_Data meshData;
    //    int vertNum = meshSource->vertices.size();
    //    for (int i = 0; i < vertNum; i++)
    //    {
    //        meshData.vertices->push_back(&meshSource->vertices[i]);
    //        meshData.faces->push_back(&meshSource->indices[i]);
    //    }
    //    return meshData;
    //}
}
#endif
