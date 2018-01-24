#ifndef CIRCLEBRUSH_H
#define CIRCLEBRUSH_H

#include <brushes/brush.h>

class CircleBrush : public Brush {
public:
    CircleBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;
};

#endif // CIRCLEBRUSH_H
