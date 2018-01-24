#ifndef UWBRUSH_H
#define UWBRUSH_H

#include <brushes/brush.h>

class UWBrush : public Brush {
public:
    UWBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;
};

#endif // UWBRUSH_H
