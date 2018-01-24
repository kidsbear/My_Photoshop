#include "uwbrush.h"
#include <paintview.h>

UWBrush::UWBrush(const std::string& name) :
    Brush(name)
{
}

void UWBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void UWBrush::BrushMove(const glm::vec2 pos) {
    // Set the color
    UseColor(GetColor(pos));
    int size = GetSize();

    // Triangle to draw
    std::vector<GLfloat> vertex = {
        pos.x - (size * 0.5f), pos.y - (size * 0.5f),
        pos.x - (size * 0.25f), pos.y + (size * 0.5f),
        pos.x, pos.y,
        pos.x + (size * 0.25f), pos.y + (size * 0.5f),
        pos.x + (size * 0.5f), pos.y - (size * 0.5f),
    };

    // Upload the vertex data to GPU buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);

    // Set the size of the points and draw them
    glDrawArrays(GL_LINE_STRIP, 0, 5);
}

void UWBrush::BrushEnd(const glm::vec2 pos) {

}
