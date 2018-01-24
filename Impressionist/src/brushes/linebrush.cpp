#include "linebrush.h"
#include <paintview.h>
#include <qlabeledslider.h>
#include <QFormLayout>
#include <iostream>
#include <math.h>

LineBrush::LineBrush(const std::string& name) :
    Brush(name),
    thickness_slider_(new QLabeledSlider)
{

  // Thickness Slider
  thickness_slider_->SetRange(1, 100);
  layout_->addRow("Thickness", thickness_slider_);
  thickness_slider_->SetValue(2);

  SetSize(24);
}

// Added functionality
unsigned int LineBrush::GetThickness() const {
    return thickness_slider_->GetValue();
}

void LineBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void LineBrush::BrushMove(const glm::vec2 pos) {

    float size = GetSize() * 1.0;
    float thickness = GetThickness() * 1.0;
    float angle = 360.0 - GetAngle() * 1.0;
    float opacityRatio = 0.01 * GetOpacity();

    // Set the color
    glm::vec4 color = GetColor(pos);  
    color.a = opacityRatio; 
    UseColor(color);  

    float x1 = pos.x - (size * 0.5f) * cos(angle/180 * M_PI) + (0.5f * thickness) * sin(angle/180 * M_PI);
    float y1 = pos.y + (size * 0.5f) * sin(angle/180 * M_PI) + (0.5f * thickness) * cos(angle/180 * M_PI);
    float x2 = pos.x - (size * 0.5f) * cos(angle/180 * M_PI) - (0.5f * thickness) * sin(angle/180 * M_PI);
    float y2 = pos.y + (size * 0.5f) * sin(angle/180 * M_PI) - (0.5f * thickness) * cos(angle/180 * M_PI);
    float x3 = pos.x + (size * 0.5f) * cos(angle/180 * M_PI) - (0.5f * thickness) * sin(angle/180 * M_PI);
    float y3 = pos.y - (size * 0.5f) * sin(angle/180 * M_PI) - (0.5f * thickness) * cos(angle/180 * M_PI);
    float x4 = pos.x + (size * 0.5f) * cos(angle/180 * M_PI) + (0.5f * thickness) * sin(angle/180 * M_PI);
    float y4 = pos.y - (size * 0.5f) * sin(angle/180 * M_PI) + (0.5f * thickness) * cos(angle/180 * M_PI);

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

    // Draw triangles to represent the line
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void LineBrush::BrushEnd(const glm::vec2 pos) {

}
