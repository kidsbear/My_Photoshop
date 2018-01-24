#include "linesegmentbrush.h"

LineSegmentBrush::LineSegmentBrush(const std::string& name) :
    Brush(name)
{
}

void LineSegmentBrush::BrushBegin(const glm::vec2 pos) {
    start_position_ = pos;
}

void LineSegmentBrush::BrushMove(const glm::vec2 pos) {
    // Set the color
    UseColor(GetColor(pos));

    // Line to draw
    std::vector<GLfloat> vertices = {
        start_position_.x, start_position_.y,
        pos.x, pos.y
    };

    // Upload the vertex data to GPU buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STREAM_DRAW);

    // Draw the line segment
    glDrawArrays(GL_LINES, 0, 2);
}

void LineSegmentBrush::BrushEnd(const glm::vec2 pos) {

}
