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

    class Mesh_Data
    {
    public:

        std::vector <glm::vec3*>* vertices;
        std::vector<glm::vec3*>* faces;

        Mesh_Data()
        {
            vertices = new std::vector<glm::vec3*>;
            faces = new std::vector<glm::vec3*>;
        }

    };
}
#endif
