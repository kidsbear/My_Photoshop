#include "scattercirclebrush.h"
#include <paintview.h>
#include <random>
#include <qlabeledslider.h>
#include <QFormLayout>
#include <math.h>

ScatterCircleBrush::ScatterCircleBrush(const std::string& name) :
    Brush(name),
    radius_slider_(new QLabeledSlider),
    density_slider_(new QLabeledSlider)

{
    // Radius slider
    radius_slider_->SetRange(1, 100);
    layout_->addRow("Radius", radius_slider_);
    radius_slider_->SetValue(20);

    // Density Slider
    density_slider_->SetRange(1, 50);
    layout_->addRow("Density", density_slider_);
    density_slider_->SetValue(3);
}


// Added functionality
unsigned int ScatterCircleBrush::GetRadius() const {
    return radius_slider_->GetValue();
}

unsigned int ScatterCircleBrush::GetDensity() const {
  return density_slider_->GetValue();
}


void ScatterCircleBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void ScatterCircleBrush::BrushMove(const glm::vec2 pos) {

    int numCircles = GetDensity();
    float offsetRange = GetRadius() * 0.5;
    float size = GetSize() * 1.0;
    int numTriangles = 90;
    float radius = size / 2.0;
    float opacityRatio = 0.01 * GetOpacity();

    for(int i = 0; i < numCircles; i++) {
        // Range (-0.5 - 0.5) * offsetRange
        float offsetX = offsetRange * (((float) rand() / (RAND_MAX)) - 0.5);
        float offsetY = offsetRange * (((float) rand() / (RAND_MAX)) - 0.5);

        // Set color
        glm::vec2 currPos = glm::vec2(pos.x + offsetX, pos.y + offsetY);
        glm::vec4 color = GetColor(currPos);
        color.a = opacityRatio;
        UseColor(color);

        std::vector<GLfloat> vertex = {currPos.x, currPos.y};
        for(int i = 0; i <= numTriangles; i++) {
            float endPosX = currPos.x + (radius * cos(i * 2 * M_PI / numTriangles));
            float endPosY = currPos.y + (radius * sin(i * 2 * M_PI / numTriangles));
            // Vertex of circle
            vertex.push_back(endPosX);
            vertex.push_back(endPosY);
        }

        // Upload the vertex data to GPU buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);
        // draw triangles to represent the circle
        glDrawArrays(GL_TRIANGLE_FAN, 0, numTriangles + 2);
    }
}

void ScatterCircleBrush::BrushEnd(const glm::vec2 pos) {

}
