#include "pointbrush.h"
#include <paintview.h>
#include <iostream>
PointBrush::PointBrush(const std::string& name) :
    Brush(name)
{
}

void PointBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void PointBrush::BrushMove(const glm::vec2 pos) {

    float opacityRatio = 0.01 * GetOpacity();

    // Set the color
    glm::vec4 color = GetColor(pos);
    color.a = opacityRatio;
    UseColor(color);

    // Points to draw
    std::vector<GLfloat> vertex = {
        pos.x, pos.y
    };

    // Upload the vertex data to GPU buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);

    // Set the size of the points and draw them
    glPointSize(GetSize());
    glDrawArrays(GL_POINTS, 0, 1);
}

void PointBrush::BrushEnd(const glm::vec2 pos) {

}
