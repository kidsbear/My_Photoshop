#ifndef LINESEGMENTBRUSH_H
#define LINESEGMENTBRUSH_H

#include <brushes/brush.h>
#include <vectors.h>

class LineSegmentBrush : public Brush {
public:
    LineSegmentBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;

private:
    glm::vec2 start_position_;
};

#endif // LINESEGMENTBRUSH_H
