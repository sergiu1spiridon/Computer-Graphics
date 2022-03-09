#include "tema1_object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* tema1_object2D::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    float lengthSmall = length / 4;
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color),
        VertexFormat(corner + glm::vec3(-lengthSmall, length + lengthSmall , 0), color),
        VertexFormat(corner + glm::vec3(-lengthSmall, length - lengthSmall, 0), color),
        VertexFormat(corner + glm::vec3(+lengthSmall, length + lengthSmall, 0), color),
        VertexFormat(corner + glm::vec3(+lengthSmall, length - lengthSmall, 0), color),
        VertexFormat(corner + glm::vec3(length - lengthSmall, length - lengthSmall, 0), color),
        VertexFormat(corner + glm::vec3(length - lengthSmall, length + lengthSmall, 0), color),
        VertexFormat(corner + glm::vec3(length + lengthSmall, length - lengthSmall, 0), color),
        VertexFormat(corner + glm::vec3(length + lengthSmall, length + lengthSmall, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 
        0, 1, 2,
        3, 0, 2,
        5, 7, 6,
        4, 5, 6,
        8, 10, 11,
        9, 8, 11,
    };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* tema1_object2D::CreateObstacle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* tema1_object2D::CreateLifebar(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;
    glm::vec3 color = glm::vec3(0, 1, 0);

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length / 100, 0, 0), color),
        VertexFormat(corner + glm::vec3(length / 100, 0.3f, 0), color),
        VertexFormat(corner + glm::vec3(0, 0.3f, 0), color),
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


