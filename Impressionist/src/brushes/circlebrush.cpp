#include "circlebrush.h"
#include <paintview.h>
#include <math.h>

CircleBrush::CircleBrush(const std::string& name) :
    Brush(name)
{
}

void CircleBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void CircleBrush::BrushMove(const glm::vec2 pos) {

    float size = GetSize() * 1.0;
    int numTriangles = 90;
    float radius = size / 2.0;
    float opacityRatio = 0.01 * GetOpacity();

    // Set the color
    glm::vec4 color = GetColor(pos);
    color.a = opacityRatio;
    UseColor(color);

    std::vector<GLfloat> vertex = {pos.x, pos.y};
    for(int i = 0; i <= numTriangles; i++) {
        float endPosX = pos.x + (radius * cos(i * 2 * M_PI / numTriangles));
        float endPosY = pos.y + (radius * sin(i * 2 * M_PI / numTriangles));
        // Vertex of circle
        vertex.push_back(endPosX);
        vertex.push_back(endPosY);
    }

    // Upload the vertex data to GPU buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);
    // draw triangles to represent the circle
    glDrawArrays(GL_TRIANGLE_FAN, 0, numTriangles + 2);

}

void CircleBrush::BrushEnd(const glm::vec2 pos) {

}
