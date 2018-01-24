#include "scatterlinebrush.h"
#include <paintview.h>
#include <qlabeledslider.h>
#include <QFormLayout>
#include <math.h>

ScatterLineBrush::ScatterLineBrush(const std::string& name) :
    Brush(name),
    thickness_slider_(new QLabeledSlider),
    radius_slider_(new QLabeledSlider),
    density_slider_(new QLabeledSlider)

{

    // Thickness Slider
    thickness_slider_->SetRange(1, 100);
    layout_->addRow("Thickness", thickness_slider_);
    thickness_slider_->SetValue(2);

    // Radius slider
    radius_slider_->SetRange(1, 100);
    layout_->addRow("Radius", radius_slider_);
    radius_slider_->SetValue(20);

    // Density Slider
    density_slider_->SetRange(1, 50);
    layout_->addRow("Density", density_slider_);
    density_slider_->SetValue(3);

    SetSize(24);
}

// Added functionality
unsigned int ScatterLineBrush::GetThickness() const {
    return thickness_slider_->GetValue();
}

unsigned int ScatterLineBrush::GetRadius() const {
    return radius_slider_->GetValue();
}

unsigned int ScatterLineBrush::GetDensity() const {
    return density_slider_->GetValue();
}


void ScatterLineBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void ScatterLineBrush::BrushMove(const glm::vec2 pos) {

    int numLines = GetDensity();
    float offsetRange = GetRadius() * 0.5;
    float size = GetSize() * 1.0;
    float thickness = GetThickness() * 1.0;
    float opacityRatio = 0.01 * GetOpacity();
    float angle = 360.0 - GetAngle() * 1.0;

    for(int i = 0; i < numLines; i++) {
        // Range (-0.5 - 0.5) * offsetRange
        float offsetX = offsetRange * (((float) rand() / (RAND_MAX)) - 0.5);
        float offsetY = offsetRange * (((float) rand() / (RAND_MAX)) - 0.5);

        // Set the color
        glm::vec2 currPos = glm::vec2(pos.x + offsetX, pos.y + offsetY);
        glm::vec4 color = GetColor(currPos);
        color.a = opacityRatio;
        UseColor(color);

        float x1 = currPos.x - (size * 0.5f) * cos(angle/180 * M_PI) + (0.5f * thickness) * sin(angle/180 * M_PI);
        float y1 = currPos.y + (size * 0.5f) * sin(angle/180 * M_PI) + (0.5f * thickness) * cos(angle/180 * M_PI);
        float x2 = currPos.x - (size * 0.5f) * cos(angle/180 * M_PI) - (0.5f * thickness) * sin(angle/180 * M_PI);
        float y2 = currPos.y + (size * 0.5f) * sin(angle/180 * M_PI) - (0.5f * thickness) * cos(angle/180 * M_PI);
        float x3 = currPos.x + (size * 0.5f) * cos(angle/180 * M_PI) - (0.5f * thickness) * sin(angle/180 * M_PI);
        float y3 = currPos.y - (size * 0.5f) * sin(angle/180 * M_PI) - (0.5f * thickness) * cos(angle/180 * M_PI);
        float x4 = currPos.x + (size * 0.5f) * cos(angle/180 * M_PI) + (0.5f * thickness) * sin(angle/180 * M_PI);
        float y4 = currPos.y - (size * 0.5f) * sin(angle/180 * M_PI) + (0.5f * thickness) * cos(angle/180 * M_PI);

        // Points to draw
        std::vector<GLfloat> vertex = {};
        vertex.push_back(x1);
        vertex.push_back(y1);
        vertex.push_back(x2);
        vertex.push_back(y2);
        vertex.push_back(x4);
        vertex.push_back(y4);
        vertex.push_back(x3);
        vertex.push_back(y3);

        // Upload the vertex data to GPU buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);

        // Draw triangles to represent the lines
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void ScatterLineBrush::BrushEnd(const glm::vec2 pos) {

}
