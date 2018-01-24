#include "scatterpointbrush.h"
#include <paintview.h>
#include <random>
#include <qlabeledslider.h>
#include <QFormLayout>

ScatterPointBrush::ScatterPointBrush(const std::string& name) :
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

    SetSize(5);
}


    // Added functionality
    unsigned int ScatterPointBrush::GetRadius() const {
        return radius_slider_->GetValue();
    }

    unsigned int ScatterPointBrush::GetDensity() const {
        return density_slider_->GetValue();
    }


void ScatterPointBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void ScatterPointBrush::BrushMove(const glm::vec2 pos) {

    int numPoints = GetDensity();
    float size = GetSize() * 1.0;
    int offsetRange = GetRadius() * 0.5;
    float opacityRatio = 0.01 * GetOpacity();

    for(int i = 0; i < numPoints; i++) {
        // Range (-0.5 - 0.5) * offsetRange
        float offsetX = offsetRange * (((float) rand() / (RAND_MAX)) - 0.5);
        float offsetY = offsetRange * (((float) rand() / (RAND_MAX)) - 0.5);

        // Set the color
        glm::vec2 currPos = glm::vec2(pos.x + offsetX, pos.y + offsetY);
        glm::vec4 color = GetColor(currPos);
        color.a = opacityRatio;
        UseColor(color);

        // Point to draw
        std::vector<GLfloat> vertex = {
            pos.x + offsetX, pos.y + offsetY,
        };

        // Upload the vertex data to GPU buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);

        // Set the size of the points and draw them
        glPointSize(size);
        glDrawArrays(GL_POINTS, 0, 1);
    }
}

void ScatterPointBrush::BrushEnd(const glm::vec2 pos) {

}
