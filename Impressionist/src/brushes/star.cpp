#include "star.h"
#include <paintview.h>
#include <qlabeledslider.h>
#include <QFormLayout>
#include <iostream>
#include <math.h>

//When using small apertures on camera (say f/18 or f/22), there will be a star-like effect on
//high-light points; I really like the effect so I implemented this brush to mimic it

StarBrush::StarBrush(const std::string& name) :
    Brush(name)
{
}

void StarBrush::BrushBegin(const glm::vec2 pos) {
    BrushMove(pos);
}

void StarBrush::BrushMove(const glm::vec2 pos) {

    float size = GetSize() * 1.0;
    float radius = size * 0.04;
    int numTriangles = 30;
    float opacityRatio = 0.01 * GetOpacity();

    // Set the color
    glm::vec4 color = GetColor(pos);
    color.a = opacityRatio;
    UseColor(color);

    std::vector<GLfloat> vertex = {pos.x, pos.y};
    for(int i = 0; i <= numTriangles; i++) {
        float endPosX = pos.x - (radius / 2 * cos(i * 2 * M_PI / numTriangles));
        float endPosY = pos.y - (radius / 2 * sin(i * 2 * M_PI / numTriangles));
        // Vertex of circle
        vertex.push_back(endPosX);
        vertex.push_back(endPosY);
    }

    // Upload the vertex data to GPU buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);
    // draw them
    glDrawArrays(GL_TRIANGLE_FAN, 0, numTriangles + 2);

    const int count = 16;

    for(int i = 0; i <= count; i++) {
        vertex = {};
        float alpha = i * 2 * M_PI / count + M_PI / count;
        float x1 = pos.x - size / 2 * cos(alpha);
        float y1 = pos.y - size / 2 * sin(alpha);
        float x2 = pos.x + radius * sin(alpha);
        float y2 = pos.y - radius * cos(alpha);
        float x3 = pos.x - radius * sin(alpha);
        float y3 = pos.y + radius * cos(alpha);
        vertex.push_back(x1);
        vertex.push_back(y1);
        vertex.push_back(x2);
        vertex.push_back(y2);
        vertex.push_back(x3);
        vertex.push_back(y3);
        // Upload the vertex data to GPU buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex.data(), GL_STREAM_DRAW);
        // draw them
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void StarBrush::BrushEnd(const glm::vec2 pos) {

}
