#ifndef POINTBRUSH_H
#define POINTBRUSH_H

#include <brushes/brush.h>

class PointBrush : public Brush {
public:
    PointBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;
};

#endif // POINTBRUSH_H
