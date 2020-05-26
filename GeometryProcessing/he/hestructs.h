#ifndef HESTRUCTS_H
#define HESTRUCTS_H

#include <vector>

namespace he
{
    /* Halfedge structs */

    struct HE // HE for halfedge
    {
        // the vertex that this halfedge comes out off
        struct HEV *vertex;
        // the face adjacent to this halfedge
        struct HEF *face;
        // the flip and next halfedge as described in the lecture notes
        struct HE *flip, *next;

        // we omit the pointer to the adjacent edge (as well as a "halfedge edge"
        // struct) because it is not necessary for the assignment
    };

    struct HEF // HEF for halfedge face
    {
        // the halfedge associated with this face
        struct HE *edge;
        // this variable is used to help orientate the halfedge when building it;
        // you don't have to worry about this
        bool oriented;
    };

    struct HEV // HEV for halfedge vertex
    {
        // the coordinates of the vertex in the mesh
        glm::vec3 position;
        // the halfedge going out off this vertex
        struct HE *out;
        // use this to store your index for this vertex when you index the vertices
        // before building the operator for implicit fairing
        int index;
        // you can use this to store the normal vector for the vertex
        glm::vec3 normal;
    };

    class Mesh_Data
    {
    public:

        std::vector <glm::vec3*>* m_vertices;
        std::vector<glm::vec3*>* m_faces;

        Mesh_Data(Mesh* mesh)
        {
            m_vertices = new std::vector<glm::vec3*>;
            m_faces = new std::vector<glm::vec3*>;

            int indexNum = mesh->indices.size();
            for (int i = 0; i < indexNum; i++)
                m_faces->push_back(&(mesh->indices[i]));
            int vertexNum = mesh->vertices.size();
            for (int i = 0; i < vertexNum; i++)
                m_vertices->push_back(&(mesh->vertices[i]));
        }

        ~Mesh_Data()
        {
            int indexNum = m_vertices->size();
            for (int i = 0; i < indexNum; i++)
                delete m_vertices->at(i);

            int vertexNum = m_faces->size();
            for (int i = 0; i < vertexNum; i++)
                delete m_faces->at(i);

            delete m_vertices;
            delete m_faces;
        }
    };
}
#endif
